
#include "GameClient.hpp"
#include <iostream>

GameClient::GameClient() {}
GameClient::~GameClient() {}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;
    AGameCore::RegisterComponents(ecs_registry_);
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
