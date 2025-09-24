/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#pragma once

#include <string>
#include "component_factory.hpp"

class registry;

class DLLoader {
    public:
        DLLoader();
        ~DLLoader();
        bool load_components_from_so(const std::string &so_path, registry &reg);
        IComponentFactory* get_factory() const;
        bool is_loaded() const;

    private:
        void* library_handle_;
        IComponentFactory* factory_;

        // Non-copyable
        DLLoader(const DLLoader&) = delete;
        DLLoader& operator=(const DLLoader&) = delete;

        // Movable
        DLLoader(DLLoader&& other) noexcept;
        DLLoader& operator=(DLLoader&& other) noexcept;
};