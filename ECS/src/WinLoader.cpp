/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WinLoader
*/

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#ifndef NOGDI
  #define NOGDI
#endif

#include <windows.h>
#include <filesystem>
#include <iostream>
#include "ECS/WinLoader.hpp"

bool WinLoader::load_system(const std::string &so_path, SystemType type) {
    auto &systems = (type == LogicSystem) ? _logicSystems : _renderSystems;

    if (!std::filesystem::exists(so_path)) {
        std::cerr << "System library file does not exist: " << so_path << std::endl;
        return false;
    }

    HMODULE handle = LoadLibraryA(so_path.c_str());
    if (!handle) {
        std::cerr << "Cannot load system library " << so_path << std::endl;
        return false;
    }

    // ---- C factory signatures: raw pointer + destroy function
    using create_system_t  = ISystem* (*)();
    using destroy_system_t = void     (*)(ISystem*);

    auto create_system  = reinterpret_cast<create_system_t>(GetProcAddress(handle, "create_system"));
    auto destroy_system = reinterpret_cast<destroy_system_t>(GetProcAddress(handle, "destroy_system"));

    if (!create_system || !destroy_system) {
        std::cerr << "Cannot load symbols create_system/destroy_system from " << so_path << std::endl;
        FreeLibrary(handle);
        return false;
    }

    // Create instance (raw pointer from the DLL)
    ISystem* raw = nullptr;
    try {
        raw = create_system();
    } catch (const std::exception& e) {
        std::cerr << "Exception while creating system from " << so_path << ": " << e.what() << std::endl;
        FreeLibrary(handle);
        return false;
    }

    if (!raw) {
        std::cerr << "Failed to create system from " << so_path << " (returned null)" << std::endl;
        FreeLibrary(handle);
        return false;
    }

    // Wrap it in a unique_ptr with custom deleter that calls destroy_system (from the same DLL)
    std::unique_ptr<ISystem, std::function<void(ISystem*)>> sys(raw, [destroy_system](ISystem* p){
        if (p) destroy_system(p);
    });

    // Store
    ALoader::LoadedSystem loaded_sys;
    loaded_sys.handle = reinterpret_cast<void*>(handle);
    loaded_sys.system = std::move(sys);
    loaded_sys.name   = std::filesystem::path(so_path).stem().string();

    systems.push_back(std::move(loaded_sys));

    std::cout << "Loaded system: " << systems.back().system->get_name()
              << " from " << so_path << std::endl;
    return true;
}


bool WinLoader::load_components(const std::string &so_path, registry &reg) {
    if (!std::filesystem::exists(so_path)) {
        std::cerr << "Component library file does not exist: " << so_path << std::endl;
        return false;
    }

    HMODULE h = LoadLibraryA(so_path.c_str());
    if (!h) {
        std::cerr << "Cannot open library: " << so_path << std::endl;
        return false;
    }
    library_handle_ = reinterpret_cast<void*>(h);

    // Load register_components function
    using register_components_t = void (*)(registry&);
    auto register_components = reinterpret_cast<register_components_t>(
        GetProcAddress(h, "register_components")
    );

    if (!register_components) {
        std::cerr << "Cannot load symbol register_components" << std::endl;
        FreeLibrary(h);
        library_handle_ = nullptr;
        return false;
    }

    // Load get_component_factory function
    using get_factory_t = IComponentFactory* (*)();
    auto get_factory = reinterpret_cast<get_factory_t>(
        GetProcAddress(h, "get_component_factory")
    );

    if (!get_factory) {
        std::cerr << "Cannot load symbol get_component_factory" << std::endl;
        FreeLibrary(h);
        library_handle_ = nullptr;
        return false;
    }

    // Register components and get factory
    try {
        register_components(reg);
        factory_ = get_factory();
        if (!factory_) {
            std::cerr << "get_component_factory returned null" << std::endl;
            FreeLibrary(h);
            library_handle_ = nullptr;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception while loading components: " << e.what() << std::endl;
        FreeLibrary(h);
        library_handle_ = nullptr;
        return false;
    }

    return true;
}

#endif // _WIN32
