/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** MacOs
*/

#pragma once

#ifdef __APPLE__

#include "ALoader.hpp"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>

class MacOs : public ALoader{
    public:
        bool load_components(const std::string& so_path, registry& reg) override;
        bool load_system(const std::string& so_path) override;

};

#endif // __APPLE__