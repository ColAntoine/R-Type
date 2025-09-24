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

using RegisterComponentsFunc = void (*)(registry &);
using GetFactoryFunc = IComponentFactory* (*)();

DLLoader::DLLoader()
    : library_handle_(nullptr), factory_(nullptr) {
}

DLLoader::~DLLoader() {
    if (library_handle_) {
        dlclose(library_handle_);
        library_handle_ = nullptr;
        factory_ = nullptr;
    }
}

DLLoader::DLLoader(DLLoader&& other) noexcept
    : library_handle_(other.library_handle_), factory_(other.factory_) {
    other.library_handle_ = nullptr;
    other.factory_ = nullptr;
}

DLLoader& DLLoader::operator=(DLLoader&& other) noexcept {
    if (this != &other) {
        if (library_handle_) {
            dlclose(library_handle_);
        }
        library_handle_ = other.library_handle_;
        factory_ = other.factory_;
        other.library_handle_ = nullptr;
        other.factory_ = nullptr;
    }
    return *this;
}

bool DLLoader::load_components_from_so(const std::string &so_path, registry &reg) {
    if (library_handle_) {
        dlclose(library_handle_);
        library_handle_ = nullptr;
        factory_ = nullptr;
    }

    library_handle_ = dlopen(so_path.c_str(), RTLD_LAZY);
    if (!library_handle_) {
        std::cerr << "Failed to load shared library: " << dlerror() << std::endl;
        return false;
    }

    auto register_components = (RegisterComponentsFunc)dlsym(library_handle_, "register_components");
    if (!register_components) {
        std::cerr << "Failed to find register_components: " << dlerror() << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }

    auto get_factory = (GetFactoryFunc)dlsym(library_handle_, "get_component_factory");
    if (!get_factory) {
        std::cerr << "Failed to find get_component_factory: " << dlerror() << std::endl;
        dlclose(library_handle_);
        library_handle_ = nullptr;
        return false;
    }
    register_components(reg);
    factory_ = get_factory();
    return factory_ != nullptr;
}

IComponentFactory* DLLoader::get_factory() const {
    return factory_;
}

bool DLLoader::is_loaded() const {
    return library_handle_ != nullptr && factory_ != nullptr;
}