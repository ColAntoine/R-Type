/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LinuxLoader
*/

#ifdef __linux__

#include "ECS/LinuxLoader.hpp"

bool LinuxLoader::load_system(const std::string &so_path, SystemType type) {
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
    ALoader::LoadedSystem loaded_sys;
    loaded_sys.handle = handle;
    loaded_sys.system = std::move(system);
    loaded_sys.name = std::filesystem::path(so_path).stem().string();

    systems.push_back(std::move(loaded_sys));

    std::cout << "Loaded system: " << systems.back().system->get_name()
              << " from " << so_path << std::endl;
    return true;
}


bool LinuxLoader::load_components(const std::string &so_path, registry &reg) {
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

    return true;
}

#endif // __linux__