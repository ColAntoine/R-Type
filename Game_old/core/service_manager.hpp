#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

// Base service interface
class IService {
public:
    virtual ~IService() = default;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update(float delta_time) = 0;
};

// Service manager for dependency injection and lifecycle
class ServiceManager {
private:
    std::unordered_map<std::type_index, std::unique_ptr<IService>> services_;
    bool initialized_ = false;

public:
    ServiceManager() = default;
    ~ServiceManager();

    // Register a service
    template<typename T, typename... Args>
    void register_service(Args&&... args) {
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");

        auto type_index = std::type_index(typeid(T));
        if (services_.find(type_index) != services_.end()) {
            throw std::runtime_error("Service already registered");
        }

        services_[type_index] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    // Get a service
    template<typename T>
    T& get_service() {
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");

        auto type_index = std::type_index(typeid(T));
        auto it = services_.find(type_index);
        if (it == services_.end()) {
            throw std::runtime_error("Service not found");
        }

        return *static_cast<T*>(it->second.get());
    }

    // Check if service exists
    template<typename T>
    bool has_service() const {
        auto type_index = std::type_index(typeid(T));
        return services_.find(type_index) != services_.end();
    }

    // Initialize all services - implementation in .cpp
    void initialize();

    // Shutdown all services - implementation in .cpp
    void shutdown();

    // Update all services - implementation in .cpp
    void update(float delta_time);
};