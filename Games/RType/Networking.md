# R-Type Networking — Design & Implementation

This document describes how networking works for the R-Type game specifically: authoritative server model, random seed usage, client-side prediction, and rollback behavior for player position. It complements the general Keller Protocol (KPN) message and packet definitions in `/protocol.md`.

## Goals / Contract

- Input from clients must be low-latency and responsive for the local player.
- Server remains authoritative for game rules, spawning, damage and global events.
- Clients should render smoothly and hide small network corrections via prediction, interpolation or rollback.
- Keep the rollback scope small and deterministic to keep complexity manageable.

Inputs/Outputs
- Inputs: client -> server: PlayerInput messages (timestamped, sequence ids)
- Outputs: server -> clients: authoritative snapshots, input acknowledgements, events (spawn, destroy, score)

Success criteria
- Local player feels responsive under typical latencies (<=150ms)
- Corrections from server do not cause large visible snaps
- Server can reproduce and validate client-reported actions where required

## High-level architecture

- Server: authoritative game loop (fixed tick). Processes queued inputs, advances authoritative ECS, broadcasts snapshots/diffs.
- Client: local ECS running at frame rate; client applies local inputs immediately (prediction), sends inputs to server with sequence id and timestamp. Client reconciles when authoritative snapshots arrive.
- Transport: UDP (KPN) for low-latency. Important messages (join, disconnect, critical events) can be sent reliably via the KPN reliability layer.

## Tick rates and timing

- Server tick: fixed (configurable). Typical value: 60Hz (16ms). Server advances authoritative simulation at this rate.
- Client send rate: match server tick or send at client frame rate but tag messages with the input sequence and timestamp. A common approach: send inputs at 60Hz.
- Snapshot rate: server may send full or delta snapshots at a lower rate (e.g., 20–30Hz) and send entity deltas or reliable critical messages at event times.

## Message flow (simplified)

1. Client captures input and immediately applies it locally (prediction). Input is appended to a pending list with an incrementing sequence id and timestamp.
2. Client sends PlayerInput packet: { sequence_id, timestamp, input_bits, optional movement vector }
3. Server receives inputs (may arrive out-of-order) and queues them for the next server tick. Server validates inputs (sanity checks, rate limits) and applies them in sequence.
4. Server advances authoritative ECS, resolves collisions, spawns/destroys entities.
5. Server sends periodic authoritative messages: snapshot/delta messages that include authoritative positions/velocities for synchronized entities and an acknowledgement field indicating the last processed input sequence per client.
6. Client receives authoritative snapshot. For remote entities: interpolate between previously received snapshots for smooth display. For local player: use the acknowledgement number to reconcile — if server state differs from predicted, perform correction (rollback or smoothing).

## Client-side prediction & reconciliation

Client-side prediction workflow:

- Maintain a buffer of recent local inputs (sequence id, timestamp, input data).
- Apply inputs locally to the player-controlled entity immediately (no waiting for server).
- When server snapshot arrives, check the acknowledged input sequence for this client.
- If the server state for the player entity differs from the locally-predicted state beyond a small threshold, perform reconciliation:
  - Rewind the local player entity state to the server-authoritative state (store earlier snapshots or component-state history for the entity).
  - Reapply all unacknowledged inputs in order to recreate the predicted current state.
  - Optionally smooth small residual errors using a short interpolation or ease to avoid visual snapping.

Notes:
- Only player-controlled entities are typically fully reconciled via rollback — other entities are interpolated.
- Keep prediction deterministic: the client must simulate movement with the same rules used on the server for those features being predicted.

## Rollback strategy (player position)

Why rollback?
- When server corrects player position (due to collision, validation, or other corrections), rollback + replay of inputs avoids stuttering more effectively than immediate snapping.

How rollback is implemented:

1. Each player-controlled entity stores a circular buffer of historical transform states (position, velocity, relevant networked components) keyed by server tick or timestamp.
2. On receiving an authoritative state from the server with last_processed_sequence N and authoritative_transform T:
   - Replace the local history entry for N with T (or record authoritative baseline).
   - Re-simulate (replay) all local inputs with sequence > N in chronological order, advancing the simulation forward to the current frame. This generates the post-replay predicted transform.
   - If the difference between the pre-replay local transform and the post-replay transform is small (below a threshold), apply a short smoothing interpolation; if large, snap to the authoritative transform then reapply unacked inputs.

Implementation hints:
- Keep stored history wide enough to cover expected round-trip time + jitter (e.g., 200–500ms of history depending on expected latency) but bounded to avoid memory blowup.
- Only store the minimal component state required to allow deterministic replay for the player entity (position, velocity, movement state, last input seq processed).
- Avoid rolling back non-deterministic systems (random effects, complex AI). If you need deterministic replay across client and server, ensure the systems are deterministic and use the same fixed-step integration.

## Authority & validation

- The server validates client inputs to prevent obviously invalid states (teleports, extreme velocities, out-of-bounds positions). Invalid clients can be rate-limited or disconnected.
- The server is responsible for authoritative events like spawning enemies, scoring, and handling collisions that affect game state for all players.
- For trust minimization, the server should not trust client-reported authoritative events; clients only send inputs, not final game events.

## Determinism & random seed

- At `GameStart`, the server broadcasts a seed to all clients. Deterministic game systems (enemy spawn patterns, wave composition) should be derived from this seed so that simulations that rely on the seed remain reproducible.
- Use deterministic PRNGs (e.g., xorshift / PCG) with explicit state. Avoid using thread-local or OS-random sources during the deterministic simulation.

## Snapshotting and delta compression

- Server may send full snapshots occasionally and smaller delta snapshots between to reduce bandwidth.
- Delta snapshots should be keyed by entity id and carry bitmasks indicating which fields changed to allow sparse updates.

Example (simplified snapshot):

```
struct Snapshot {
  uint32_t tick_id;                 // server tick id
  uint32_t ack_input_sequence;      // last processed input sequence for this client
  uint16_t entity_count;
  EntityDelta entities[];           // delta entries per entity
};

struct EntityDelta {
  uint32_t entity_id;
  uint8_t changed_mask;             // bitmask for changed fields
  // fields follow depending on mask (position, velocity, health, etc.)
};
```

## Interpolation for remote players

- For non-local entities, clients buffer recent snapshots and interpolate between two snapshots based on a fixed interpolation delay (e.g., 100ms) to hide jitter.
- Keep interpolation delay configurable and small to balance latency and smoothness.

## Bandwidth & reliability considerations

- Send frequent small input packets (stateless) from client to server. These can be sent unreliably (UDP) as they are frequently superseded by newer inputs.
- Authoritative snapshots and critical events (spawn/despawn, score updates) should use the protocol's reliability features or be retransmitted until acknowledged.
- Use sequence ids and timestamps for packets to allow reordering and replay protection.

## Implementation checklist (developer)

- [ ] Ensure player movement logic is deterministic and matches server integration.
- [ ] Implement input queue and acknowledgement handling on both client and server.
- [ ] Implement per-entity history buffers for player entities with configurable length.
- [ ] Implement a replay/reapply system that replays local inputs deterministically for reconciliation.
- [ ] Implement interpolation for remote entities and smoothing for small corrections.
- [ ] Include server-side validation and rate limiting for inputs.
- [ ] Publish seeds at `GameStart` and ensure deterministic PRNG usage.

## Tradeoffs and known limitations

- Full rollback of entire game state is expensive and complex; this implementation uses a targeted rollback limited primarily to player-controlled entities to keep complexity and CPU cost manageable.
- Some non-deterministic systems (complex AI, physics-based effects) may not be rolled back. Instead, rely on server authoritative corrections and client-side smoothing for these.

## Troubleshooting

- If players experience frequent snaps:
  - Increase history buffer size and ensure inputs are being sent at the configured rate.
  - Check server tick stability and ensure server runs fixed-step updates.
  - Verify deterministic movement integration between client and server.

- If remote entities jitter heavily:
  - Increase interpolation delay slightly and check snapshot/delta compression correctness.

---

For message-level format and the canonical packet definitions, see `/protocol.md` in the repository root. That file contains the wire-level details, flags, headers, and optional features supported by the Keller Protocol.

**Last updated**: 2025-11-02
