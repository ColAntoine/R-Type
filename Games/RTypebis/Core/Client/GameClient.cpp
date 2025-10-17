#include "GameClient.hpp"

IGameCore& GameClient::getInstance() {
    static GameClient instance;
    return instance;
}

bool GameClient::init() {
    this->g_running = true;
    return true;
}

bool GameClient::start() {
    while (g_running);
    return true;
}

bool GameClient::update(float deltaTime) {
    // Update code for the game client
    return true;
}

bool GameClient::shutdown() {
    this->g_running = false;
    return true;
}
