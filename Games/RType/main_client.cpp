#include "Core/Client/GameClient.hpp"

int main()
{
    GameClient client;
    if (!client.init())
        return 1;
    client.run();
    client.shutdown();
    return 0;
}
