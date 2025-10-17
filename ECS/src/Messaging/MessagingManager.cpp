/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MessagingManager Implementation
*/

#include "ECS/Messaging/MessagingManager.hpp"
#include <iostream>

MessagingManager& MessagingManager::instance() {
    static MessagingManager instance;
    return instance;
}

void MessagingManager::init() {
    event_bus_.clear_all();
    command_dispatcher_.clear_all();
    message_queue_.clear();
    std::cout << "MessagingManager initialized" << std::endl;
}

void MessagingManager::shutdown() {
    event_bus_.clear_all();
    command_dispatcher_.clear_all();
    message_queue_.clear();
    std::cout << "MessagingManager shutdown" << std::endl;
}

void MessagingManager::update() {
    event_bus_.process_deferred();
}

void MessagingManager::clear_all() {
    event_bus_.clear_all();
    command_dispatcher_.clear_all();
    message_queue_.clear();
}
