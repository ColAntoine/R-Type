/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowsLoader
*/

#pragma once

#include "ALoader.hpp"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>
#include <windows.h>

class WindowsLoader : public ALoader {
    public:
        bool load_components(const std::string& so_path, registry& reg) override;
        bool load_system(const std::string& so_path) override;
};