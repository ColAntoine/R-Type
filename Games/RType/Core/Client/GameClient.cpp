
#include "GameClient.hpp"
#include <iostream>
#include "Network/UDPClient.hpp"

GameClient::GameClient() {}
GameClient::~GameClient() {}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;
    AGameCore::RegisterComponents(ecs_registry_);

    // Try to connect to server on localhost:8080 for a smoke-test
    UdpClient client;
    auto resp = client.connect("127.0.0.1", 8080, "Tester");
    if (resp) {
        std::cout << "Connected to server, assigned player_id=" << resp->player_id << std::endl;
    } else {
        std::cout << "Server did not respond to connect request" << std::endl;
    }

    return true;
}

void GameClient::run()
{
    std::cout << "GameClient::run" << std::endl;
    AGameCore::run();
}

void GameClient::update(float delta)
{
    // placeholder update
    (void)delta;
}

void GameClient::shutdown()
{
    std::cout << "GameClient::shutdown" << std::endl;
}

registry& GameClient::GetRegistry() { return ecs_registry_; }
