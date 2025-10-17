/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** PluginMetadata - Plugin information structure
*/

#pragma once

#include <string>
#include <vector>

struct PluginMetadata {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::vector<std::string> dependencies;

    PluginMetadata(
        const std::string& n = "Unknown",
        const std::string& v = "1.0.0",
        const std::string& a = "Unknown",
        const std::string& d = ""
    ) : name(n), version(v), author(a), description(d) {}
};

// Plugin API - Every plugin must export these functions
extern "C" {
    typedef PluginMetadata (*GetMetadataFunc)();
    typedef void* (*CreateSystemFunc)();
    typedef void (*DestroySystemFunc)(void*);
    typedef void (*OnLoadFunc)();
    typedef void (*OnUnloadFunc)();
}

#define PLUGIN_EXPORT extern "C"

// Helper macros for plugin creation
#define DECLARE_PLUGIN_METADATA(name, version, author, desc) \
    PLUGIN_EXPORT PluginMetadata get_plugin_metadata() { \
        return PluginMetadata(name, version, author, desc); \
    }

#define DECLARE_PLUGIN_LIFECYCLE() \
    PLUGIN_EXPORT void on_plugin_load() {} \
    PLUGIN_EXPORT void on_plugin_unload() {}
