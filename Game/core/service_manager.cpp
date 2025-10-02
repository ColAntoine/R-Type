#include "service_manager.hpp"

ServiceManager::~ServiceManager() {
    shutdown();
}

void ServiceManager::initialize() {
    if (initialized_) return;

    for (auto& [type, service] : services_) {
        service->initialize();
    }
    initialized_ = true;
}

void ServiceManager::shutdown() {
    if (!initialized_) return;

    // Shutdown services (can't iterate in reverse for unordered_map)
    for (auto& [type, service] : services_) {
        service->shutdown();
    }
    services_.clear();
    initialized_ = false;
}

void ServiceManager::update(float delta_time) {
    if (!initialized_) return;

    for (auto& [type, service] : services_) {
        service->update(delta_time);
    }
}