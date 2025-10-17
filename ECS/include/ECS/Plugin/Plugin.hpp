/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** Plugin - Loaded plugin wrapper
*/

#pragma once

#include "PluginMetadata.hpp"
#include <memory>
#include <string>

class Plugin {
    public:
        Plugin(const std::string& path, void* handle);
        ~Plugin();

        bool load_metadata();
        void* create_system();
        void destroy_system(void* system);

        const std::string& get_path() const { return path_; }
        const PluginMetadata& get_metadata() const { return metadata_; }
        void* get_handle() const { return handle_; }
        bool is_loaded() const { return handle_ != nullptr; }

        void call_on_load();
        void call_on_unload();

    private:
        std::string path_;
        void* handle_;
        PluginMetadata metadata_;

        GetMetadataFunc get_metadata_func_;
        CreateSystemFunc create_system_func_;
        DestroySystemFunc destroy_system_func_;
        OnLoadFunc on_load_func_;
        OnUnloadFunc on_unload_func_;
};
