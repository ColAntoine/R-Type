#pragma once

#include "event_manager.hpp"
#include <raylib.h>
#include <vector>
#include <cstdint>

// Input Events
struct KeyPressEvent : Event {
    int key;
    KeyPressEvent(int k) : key(k) {}
};

struct KeyReleaseEvent : Event {
    int key;
    KeyReleaseEvent(int k) : key(k) {}
};

struct MouseMoveEvent : Event {
    float x, y;
    MouseMoveEvent(float x_, float y_) : x(x_), y(y_) {}
};

struct MouseButtonEvent : Event {
    int button;
    bool pressed;
    float x, y;
    MouseButtonEvent(int b, bool p, float x_, float y_) : button(b), pressed(p), x(x_), y(y_) {}
};

// Game Events
struct PlayerMoveEvent : Event {
    int player_id;
    float x, y;
    float vx, vy;
    PlayerMoveEvent(int id, float x_, float y_, float vx_, float vy_)
        : player_id(id), x(x_), y(y_), vx(vx_), vy(vy_) {}
};

struct PlayerJoinEvent : Event {
    int player_id;
    float x, y;
    PlayerJoinEvent(int id, float x_, float y_) : player_id(id), x(x_), y(y_) {}
};

struct PlayerLeaveEvent : Event {
    int player_id;
    PlayerLeaveEvent(int id) : player_id(id) {}
};

struct EntityCreateEvent : Event {
    int entity_id;
    int entity_type;
    float x, y;
    EntityCreateEvent(int id, int type, float x_, float y_)
        : entity_id(id), entity_type(type), x(x_), y(y_) {}
};

struct EntityDestroyEvent : Event {
    int entity_id;
    EntityDestroyEvent(int id) : entity_id(id) {}
};

// Network Events
struct NetworkConnectedEvent : Event {
    int player_id;
    NetworkConnectedEvent(int id) : player_id(id) {}
};

struct NetworkDisconnectedEvent : Event {};

struct NetworkMessageEvent : Event {
    std::vector<uint8_t> data;
    NetworkMessageEvent(const std::vector<uint8_t>& d) : data(d) {}
};

// System Events
struct GameStateChangeEvent : Event {
    enum State { MENU, PLAYING, PAUSED, GAME_OVER };
    State old_state;
    State new_state;
    GameStateChangeEvent(State old_s, State new_s) : old_state(old_s), new_state(new_s) {}
};

struct FrameUpdateEvent : Event {
    float delta_time;
    FrameUpdateEvent(float dt) : delta_time(dt) {}
};