/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** Plugin Implementation
*/

#include "ECS/Plugin/Plugin.hpp"
#include <dlfcn.h>
#include <iostream>

Plugin::Plugin(const std::string& path, void* handle)
    : path_(path)
    , handle_(handle)
    , get_metadata_func_(nullptr)
    , create_system_func_(nullptr)
    , destroy_system_func_(nullptr)
    , on_load_func_(nullptr)
    , on_unload_func_(nullptr) {
}

Plugin::~Plugin() {
    if (handle_) {
        call_on_unload();
        dlclose(handle_);
        handle_ = nullptr;
    }
}

bool Plugin::load_metadata() {
    if (!handle_) return false;

    // Load metadata function (optional)
    get_metadata_func_ = (GetMetadataFunc)dlsym(handle_, "get_plugin_metadata");
    if (get_metadata_func_) {
        metadata_ = get_metadata_func_();
        std::cout << "Plugin metadata: " << metadata_.name
                  << " v" << metadata_.version << std::endl;
    } else {
        metadata_ = PluginMetadata(path_, "1.0.0", "Unknown", "");
    }

    // Load system creation functions
    create_system_func_ = (CreateSystemFunc)dlsym(handle_, "create_system");
    destroy_system_func_ = (DestroySystemFunc)dlsym(handle_, "destroy_system");

    // Load lifecycle functions (optional)
    on_load_func_ = (OnLoadFunc)dlsym(handle_, "on_plugin_load");
    on_unload_func_ = (OnUnloadFunc)dlsym(handle_, "on_plugin_unload");

    return create_system_func_ != nullptr;
}

void* Plugin::create_system() {
    if (create_system_func_) {
        return create_system_func_();
    }
    return nullptr;
}

void Plugin::destroy_system(void* system) {
    if (destroy_system_func_ && system) {
        destroy_system_func_(system);
    }
}

void Plugin::call_on_load() {
    if (on_load_func_) {
        std::cout << "Calling on_load for plugin: " << metadata_.name << std::endl;
        on_load_func_();
    }
}

void Plugin::call_on_unload() {
    if (on_unload_func_) {
        std::cout << "Calling on_unload for plugin: " << metadata_.name << std::endl;
        on_unload_func_();
    }
}
