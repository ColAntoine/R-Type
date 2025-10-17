#include "Core/Server/GameServer.hpp"

int main()
{
    GameServer server;
    if (!server.init())
        return 1;
    server.run();
    server.shutdown();
    return 0;
}
