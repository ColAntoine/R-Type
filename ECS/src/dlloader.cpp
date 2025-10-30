#include "ECS/DLLoader.hpp"
#include "ECS/Registry.hpp"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>

DLLoader::DLLoader() : library_handle_(nullptr), factory_(nullptr) {}

DLLoader::~DLLoader() {
    // Clean up systems - destroy objects before unloading libraries
    for (auto& loaded_sys : _logicSystems) {
        // Explicitly reset the unique_ptr to destroy the system object first
        loaded_sys.system.reset();
        // Now it's safe to close the library handle
        if (loaded_sys.handle) {
            dlclose(loaded_sys.handle);
        }
    }
    _logicSystems.clear();

    for (auto& loaded_sys : _renderSystems) {
        // Explicitly reset the unique_ptr to destroy the system object first
        loaded_sys.system.reset();
        // Now it's safe to close the library handle
        if (loaded_sys.handle) {
            dlclose(loaded_sys.handle);
        }
    }
    _renderSystems.clear();

    // Clean up component library
    if (library_handle_) {
        dlclose(library_handle_);
        library_handle_ = nullptr;
        factory_ = nullptr;
    }
}

bool DLLoader::load_system_from_so(const std::string &so_path, SystemType type) {
    auto &systems = (type == LogicSystem) ? _logicSystems : _renderSystems;
    // Check if file exists
    if (!std::filesystem::exists(so_path)) {
        std::cerr << "System library file does not exist: " << so_path << std::endl;
        return false;
    }

    // Load the shared library
    void* handle = dlopen(so_path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot load system library " << so_path << ": " << dlerror() << std::endl;
        return false;
    }

    // Clear any existing error
    dlerror();

    // Get the create_system function
    typedef std::unique_ptr<ISystem> (*create_system_t)();
    create_system_t create_system = reinterpret_cast<create_system_t>(dlsym(handle, "create_system"));

    const char* dlsym_error = dlerror();
    if (dlsym_error || !create_system) {
        std::cerr << "Cannot load symbol create_system from " << so_path << ": "
                  << (dlsym_error ? dlsym_error : "Symbol not found") << std::endl;
        dlclose(handle);
        return false;
    }

    // Create the system instance
    std::unique_ptr<ISystem> system;
    try {
        system = create_system();
    } catch (const std::exception& e) {
        std::cerr << "Exception while creating system from " << so_path << ": " << e.what() << std::endl;
        dlclose(handle);
        return false;
    }

    if (!system) {
        std::cerr << "Failed to create system from " << so_path << " (returned null)" << std::endl;
        dlclose(handle);
        return false;
    }

    // Store the loaded system
    LoadedSystem loaded_sys;
    loaded_sys.handle = handle;
    loaded_sys.system = std::move(system);
    loaded_sys.name = std::filesystem::path(so_path).stem().string();

    systems.push_back(std::move(loaded_sys));

    std::cout << "Loaded system: " << systems.back().system->get_name()
              << " from " << so_path << std::endl;
    return true;
}

void DLLoader::update_all_systems(registry& r, float dt, SystemType type) {
    auto &systems = (type == LogicSystem) ? _logicSystems : _renderSystems;
    for (auto& loaded_sys : systems) {
        if (loaded_sys.system) {
            try {
                loaded_sys.system->update(r, dt);
            } catch (const std::exception& e) {
                std::cerr << "Exception in system " << loaded_sys.system->get_name()
                          << ": " << e.what() << std::endl;
            }
        }
    }
}

void DLLoader::update_system_by_name(const std::string& name, registry& r, float dt, SystemType type) {
    auto &systems = (type == LogicSystem) ? _logicSystems : _renderSystems;
    for (auto& loaded_sys : systems) {
        if (loaded_sys.system && loaded_sys.system->get_name() == name) {
            try {
                loaded_sys.system->update(r, dt);
            } catch (const std::exception& e) {
                std::cerr << "Exception in system " << name << ": " << e.what() << std::endl;
            }
            return;
        }
    }
    std::cerr << "System not found: " << name << std::endl;
}

bool DLLoader::load_components_from_so(const std::string &so_path, registry &reg) {
    if (!std::filesystem::exists(so_path)) {
        std::cerr << "Component library file does not exist: " << so_path << std::endl;
        return false;
    }

    library_handle_ = dlopen(so_path.c_str(), RTLD_LAZY);
    if (!library_handle_) {
        std::cerr << "Cannot open library: " << so_path << ": " << dlerror() << std::endl;
        return false;
    }

    // Clear any existing error
    dlerror();

    // Load register_components function
    typedef void (*register_components_t)(registry&);
    register_components_t register_components = reinterpret_cast<register_components_t>(
        dlsym(library_handle_, "register_components"));

    const char* dlsym_error = dlerror();
    if (dlsym_error || !register_components) {
        std::cerr << "Cannot load symbol register_components: "
                  << (dlsym_error ? dlsym_error : "Symbol not found") << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    // Load get_component_factory function
    // * This is a pointer to a singletone so that is okay
    typedef IComponentFactory* (*get_factory_t)();
    get_factory_t get_factory = reinterpret_cast<get_factory_t>(
        dlsym(library_handle_, "get_component_factory"));

    dlsym_error = dlerror();
    if (dlsym_error || !get_factory) {
        std::cerr << "Cannot load symbol get_component_factory: "
                  << (dlsym_error ? dlsym_error : "Symbol not found") << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    // Register components and get factory
    try {
        register_components(reg);
        factory_ = get_factory();
        if (!factory_) {
            std::cerr << "get_component_factory returned null" << std::endl;
            dlclose(library_handle_);
            library_handle_ = nullptr;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception while loading components: " << e.what() << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    std::cout << "[DLLoader] Components loaded successfully from " << so_path << std::endl;
    return true;
}

IComponentFactory* DLLoader::get_factory() const {
    return factory_;
}

bool DLLoader::is_loaded() const {
    return library_handle_ != nullptr;
}

size_t DLLoader::get_system_count(SystemType type) const {
    switch (type) {
        case LogicSystem: return _logicSystems.size();
        case RenderSystem: return _renderSystems.size();
        default: return 0;
    }
}

std::vector<std::string> DLLoader::get_system_names(SystemType type) const {
    std::vector<std::string> names;
    switch (type) {
        case LogicSystem:
            for (const auto& loaded_sys : _logicSystems) {
                if (loaded_sys.system) {
                    names.push_back(loaded_sys.system->get_name());
                }
            }
            break;
        case RenderSystem:
            for (const auto& loaded_sys : _renderSystems) {
                if (loaded_sys.system) {
                    names.push_back(loaded_sys.system->get_name());
                }
            }
            break;
        default:
            break;
    }
    return names;
}

// Move constructor and assignment operator
DLLoader::DLLoader(DLLoader&& other) noexcept
    : library_handle_(other.library_handle_)
    , factory_(other.factory_)
    , _logicSystems(std::move(other._logicSystems))
    , _renderSystems(std::move(other._renderSystems))
{
    other.library_handle_ = nullptr;
    other.factory_ = nullptr;
}

DLLoader& DLLoader::operator=(DLLoader&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        this->~DLLoader();

        // Move from other
        library_handle_ = other.library_handle_;
        factory_ = other.factory_;
        _logicSystems = std::move(other._logicSystems);
        _renderSystems = std::move(other._renderSystems);

        // Reset other
        other.library_handle_ = nullptr;
        other.factory_ = nullptr;
    }
    return *this;
}