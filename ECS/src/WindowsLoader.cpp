/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowsLoader
*/

#include "ECS/WindowsLoader.hpp"

bool WindowsLoader::load_system(const std::string &so_path) {
    // Check if file exists
    if (!std::filesystem::exists(so_path)) {
        std::cerr << "System library file does not exist: " << so_path << std::endl;
        return false;
    }

    // Load the DLL
    HMODULE handle = LoadLibraryA(so_path.c_str());
    if (!handle) {
        std::cerr << "Cannot load system library " << so_path << std::endl;
        return false;
    }

    // Get the create_system function
    typedef std::unique_ptr<ISystem> (*create_system_t)();
    create_system_t create_system = reinterpret_cast<create_system_t>(
        GetProcAddress(handle, "create_system")
    );

    if (!create_system) {
        std::cerr << "Cannot load symbol create_system from " << so_path << std::endl;
        FreeLibrary(handle);
        return false;
    }

    // Create the system instance
    std::unique_ptr<ISystem> system;
    try {
        system = create_system();
    } catch (const std::exception& e) {
        std::cerr << "Exception while creating system from " << so_path << ": " << e.what() << std::endl;
        FreeLibrary(handle);
        return false;
    }

    if (!system) {
        std::cerr << "Failed to create system from " << so_path << " (returned null)" << std::endl;
        FreeLibrary(handle);
        return false;
    }

    // Store the loaded system
    ALoader::LoadedSystem loaded_sys;
    loaded_sys.handle = handle;
    loaded_sys.system = std::move(system);
    loaded_sys.name = std::filesystem::path(so_path).stem().string();

    systems_.push_back(std::move(loaded_sys));

    std::cout << "Loaded system: " << systems_.back().system->get_name()
              << " from " << so_path << std::endl;
    return true;
}

bool WindowsLoader::load_components(const std::string &so_path, registry &reg) {
    if (!std::filesystem::exists(so_path)) {
        std::cerr << "Component library file does not exist: " << so_path << std::endl;
        return false;
    }

    library_handle_ = LoadLibraryA(so_path.c_str());
    if (!library_handle_) {
        std::cerr << "Cannot open library: " << so_path << std::endl;
        return false;
    }

    // Load register_components function
    typedef void (*register_components_t)(registry&);
    register_components_t register_components = reinterpret_cast<register_components_t>(
        GetProcAddress(library_handle_, "register_components")
    );

    if (!register_components) {
        std::cerr << "Cannot load symbol register_components" << std::endl;
        FreeLibrary(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    // Load get_component_factory function
    typedef IComponentFactory* (*get_factory_t)();
    get_factory_t get_factory = reinterpret_cast<get_factory_t>(
        GetProcAddress(library_handle_, "get_component_factory")
    );

    if (!get_factory) {
        std::cerr << "Cannot load symbol get_component_factory" << std::endl;
        FreeLibrary(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    // Register components and get factory
    try {
        register_components(reg);
        factory_ = get_factory();
        if (!factory_) {
            std::cerr << "get_component_factory returned null" << std::endl;
            FreeLibrary(library_handle_);
            library_handle_ = nullptr;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception while loading components: " << e.what() << std::endl;
        FreeLibrary(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    std::cout << "Components loaded successfully from " << so_path << std::endl;
    return true;
}
