/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** PluginManager Implementation
*/

#include "ECS/Plugin/PluginManager.hpp"
#include <dlfcn.h>
#include <iostream>
#include <dirent.h>
#include <algorithm>

PluginManager& PluginManager::instance() {
    static PluginManager instance;
    return instance;
}

void PluginManager::init(const std::string& plugin_directory) {
    plugin_directory_ = plugin_directory;
    std::cout << "PluginManager initialized (directory: " << plugin_directory_ << ")" << std::endl;
}

void PluginManager::shutdown() {
    plugins_.clear();
    std::cout << "PluginManager shutdown" << std::endl;
}

bool PluginManager::discover_plugins() {
    auto files = scan_directory(plugin_directory_);
    
    std::cout << "Discovered " << files.size() << " potential plugins" << std::endl;
    
    int loaded = 0;
    for (const auto& file : files) {
        if (is_plugin_file(file)) {
            if (load_plugin(plugin_directory_ + "/" + file)) {
                loaded++;
            }
        }
    }
    
    std::cout << "Successfully loaded " << loaded << " plugins" << std::endl;
    return loaded > 0;
}

bool PluginManager::load_plugin(const std::string& path) {
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load plugin: " << path << std::endl;
        std::cerr << "Error: " << dlerror() << std::endl;
        return false;
    }

    auto plugin = std::make_unique<Plugin>(path, handle);
    
    if (!plugin->load_metadata()) {
        std::cerr << "Plugin missing create_system function: " << path << std::endl;
        return false;
    }

    plugin->call_on_load();

    std::string name = plugin->get_metadata().name;
    plugins_[name] = std::move(plugin);
    
    std::cout << "Loaded plugin: " << name << " from " << path << std::endl;
    return true;
}

bool PluginManager::unload_plugin(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        std::cerr << "Plugin not found: " << name << std::endl;
        return false;
    }

    plugins_.erase(it);
    std::cout << "Unloaded plugin: " << name << std::endl;
    return true;
}

bool PluginManager::reload_plugin(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        std::cerr << "Plugin not found for reload: " << name << std::endl;
        return false;
    }

    std::string path = it->second->get_path();
    
    if (!unload_plugin(name)) {
        return false;
    }

    std::cout << "Reloading plugin: " << name << std::endl;
    return load_plugin(path);
}

Plugin* PluginManager::get_plugin(const std::string& name) {
    auto it = plugins_.find(name);
    return it != plugins_.end() ? it->second.get() : nullptr;
}

std::vector<std::string> PluginManager::get_plugin_names() const {
    std::vector<std::string> names;
    for (const auto& [name, plugin] : plugins_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> PluginManager::scan_directory(const std::string& directory) {
    std::vector<std::string> files;
    
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << directory << std::endl;
        return files;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename != "." && filename != "..") {
            files.push_back(filename);
        }
    }

    closedir(dir);
    return files;
}

bool PluginManager::is_plugin_file(const std::string& filename) {
    return filename.size() > 3 && 
           filename.substr(filename.size() - 3) == ".so" &&
           filename.substr(0, 3) == "lib";
}
