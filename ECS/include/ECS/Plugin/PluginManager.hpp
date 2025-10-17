/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** PluginManager - Plugin discovery, loading, and hot-reload
*/

#pragma once

#include "Plugin.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class PluginManager {
    public:
        static PluginManager& instance();

        void init(const std::string& plugin_directory = "./lib/systems");
        void shutdown();

        bool discover_plugins();
        bool load_plugin(const std::string& path);
        bool unload_plugin(const std::string& name);
        bool reload_plugin(const std::string& name);

        Plugin* get_plugin(const std::string& name);
        std::vector<std::string> get_plugin_names() const;
        size_t get_plugin_count() const { return plugins_.size(); }

        void set_plugin_directory(const std::string& dir) { plugin_directory_ = dir; }
        const std::string& get_plugin_directory() const { return plugin_directory_; }

    private:
        PluginManager() = default;
        ~PluginManager() = default;
        PluginManager(const PluginManager&) = delete;
        PluginManager& operator=(const PluginManager&) = delete;

        std::vector<std::string> scan_directory(const std::string& directory);
        bool is_plugin_file(const std::string& filename);

        std::unordered_map<std::string, std::unique_ptr<Plugin>> plugins_;
        std::string plugin_directory_;
};
