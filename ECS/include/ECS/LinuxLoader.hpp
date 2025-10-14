/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LinuxLoader
*/

#pragma once

#ifdef __linux__

#include "ALoader.hpp"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>

class LinuxLoader : public ALoader {
    public:
        bool load_components(const std::string& so_path, registry& reg) override;
        bool load_system(const std::string& so_path) override; 
};

#endif // __linux__
