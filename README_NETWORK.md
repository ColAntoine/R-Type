# R-Type UDP Server & Client

Simple UDP-based server and client for R-Type multiplayer game.

## Build

```bash
# Build server
make -C Network

# Build client  
make -C Game
```

## Usage

### Start the Server

```bash
# Default port 8080
./Network/bin/r-type_server

# Custom port
./Network/bin/r-type_server 9999
```

Server commands:
- `info` - Show connected clients
- `broadcast <message>` - Send message to all clients
- `quit` - Stop server

### Start Client(s)

```bash
# Connect to localhost:8080
./Game/bin/r-type_client

# Connect to custom server
./Game/bin/r-type_client 192.168.1.100 9999
```

Client commands:
- `info` - Show client connection info
- `msg <message>` - Send message to server
- `help` - Show help
- `quit` - Disconnect
- Any other text - Sent as message to server

### Multiple Clients

You can run multiple clients simultaneously:

```bash
# Terminal 1
./Game/bin/r-type_client

# Terminal 2  
./Game/bin/r-type_client

# Terminal 3
./Game/bin/r-type_client
```

Each client gets its own local port and can communicate independently with the server.

## Features

### Server
- ✅ UDP-based networking
- ✅ Multiple client connections  
- ✅ Client tracking (IP, port, last ping)
- ✅ Message broadcasting
- ✅ Graceful shutdown (Ctrl+C)
- ✅ Receive timeout for clean thread exits

### Client
- ✅ UDP connection to server
- ✅ Automatic local port assignment
- ✅ Threaded message receiving
- ✅ Interactive command interface
- ✅ Graceful disconnect
- ✅ Signal handling (Ctrl+C)

## Testing

1. Start the server:
   ```bash
   ./Network/bin/r-type_server
   ```

2. Start a client:
   ```bash
   ./Game/bin/r-type_client
   ```

3. Type messages in the client - they'll be echoed back by the server

4. Use server's `broadcast` command to send messages to all clients

5. Open multiple client terminals to test multiple connections