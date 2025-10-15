/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowsLoader
*/

#pragma once

#ifdef _WIN32

#include "ALoader.hpp"
#include <iostream>
#include <filesystem>
// windows.h is already included in ALoader.hpp with proper conflict prevention

class WindowsLoader : public ALoader {
    public:
        bool load_components(const std::string& so_path, registry& reg) override;
        bool load_system(const std::string& so_path) override;
};

#endif // _WIN32