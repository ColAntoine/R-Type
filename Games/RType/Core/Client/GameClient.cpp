
#include "GameClient.hpp"
#include <iostream>

GameClient::GameClient() {}
GameClient::~GameClient() {}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;
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
