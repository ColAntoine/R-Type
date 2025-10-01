/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#include "ecs/dlloader.hpp"
#include "ecs/registry.hpp"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>

DLLoader::DLLoader() : library_handle_(nullptr), factory_(nullptr) {}

DLLoader::~DLLoader() {
    // Unload systems first
    for (auto& loaded_sys : systems_) {
        typedef void (*destroy_system_t)(ISystem*);
        destroy_system_t destroy_system = (destroy_system_t) dlsym(loaded_sys.handle, "destroy_system");

        if (destroy_system) {
            destroy_system(loaded_sys.system.release());
        }

        dlclose(loaded_sys.handle);
    }
    systems_.clear();

    // Then unload components library
    if (library_handle_) {
        dlclose(library_handle_);
        library_handle_ = nullptr;
        factory_ = nullptr;
    }
}

// Existing component loading functionality
bool DLLoader::load_components_from_so(const std::string &so_path, registry &reg) {
    library_handle_ = dlopen(so_path.c_str(), RTLD_LAZY);
    if (!library_handle_) {
        std::cerr << "Cannot open library: " << dlerror() << std::endl;
        return false;
    }

    // Load the registration function
    typedef void (*register_components_t)(registry &);
    register_components_t register_components = (register_components_t) dlsym(library_handle_, "register_components");

    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol register_components: " << dlsym_error << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    // Load the factory getter
    typedef IComponentFactory* (*get_component_factory_t)();
    get_component_factory_t get_component_factory = (get_component_factory_t) dlsym(library_handle_, "get_component_factory");

    dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol get_component_factory: " << dlsym_error << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    // Register components and get factory
    register_components(reg);
    factory_ = get_component_factory();

    return true;
}

// New system loading functionality
bool DLLoader::load_system_from_so(const std::string &so_path) {
    // Load the shared library
    void* handle = dlopen(so_path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot load system library " << so_path << ": " << dlerror() << std::endl;
        return false;
    }

    // Get the create_system function
    typedef ISystem* (*create_system_t)();
    create_system_t create_system = (create_system_t) dlsym(handle, "create_system");

    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol create_system: " << dlsym_error << std::endl;
        dlclose(handle);
        return false;
    }

    // Create the system instance
    ISystem* system = create_system();
    if (!system) {
        std::cerr << "Failed to create system from " << so_path << std::endl;
        dlclose(handle);
        return false;
    }

    // Store the loaded system
    LoadedSystem loaded_sys;
    loaded_sys.handle = handle;
    loaded_sys.system = std::unique_ptr<ISystem>(system);
    loaded_sys.name = std::filesystem::path(so_path).stem().string();

    systems_.push_back(std::move(loaded_sys));

    std::cout << "Loaded system: " << system->get_name() << " from " << so_path << std::endl;
    return true;
}

void DLLoader::update_all_systems(registry& r, float dt) {
    for (auto& loaded_sys : systems_) {
        loaded_sys.system->update(r, dt);
    }
}

void DLLoader::update_system_by_name(const std::string& name, registry& r, float dt) {
    for (auto& loaded_sys : systems_) {
        if (loaded_sys.system->get_name() == name) {
            loaded_sys.system->update(r, dt);
            return;
        }
    }
    std::cerr << "System not found: " << name << std::endl;
}

// Existing and new utility methods
IComponentFactory* DLLoader::get_factory() const {
    return factory_;
}

bool DLLoader::is_loaded() const {
    return library_handle_ != nullptr;
}

size_t DLLoader::get_system_count() const {
    return systems_.size();
}

std::vector<std::string> DLLoader::get_system_names() const {
    std::vector<std::string> names;
    for (const auto& loaded_sys : systems_) {
        names.push_back(loaded_sys.system->get_name());
    }
    return names;
}

// Move constructor and assignment (existing functionality)
DLLoader::DLLoader(DLLoader&& other) noexcept
    : library_handle_(other.library_handle_),
      factory_(other.factory_),
      systems_(std::move(other.systems_)) {
    other.library_handle_ = nullptr;
    other.factory_ = nullptr;
}

DLLoader& DLLoader::operator=(DLLoader&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        if (library_handle_) {
            dlclose(library_handle_);
        }
        for (auto& loaded_sys : systems_) {
            dlclose(loaded_sys.handle);
        }

        // Move from other
        library_handle_ = other.library_handle_;
        factory_ = other.factory_;
        systems_ = std::move(other.systems_);

        // Reset other
        other.library_handle_ = nullptr;
        other.factory_ = nullptr;
    }
    return *this;
}