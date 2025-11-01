/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowsLoader
*/

#pragma once
#ifdef _WIN32

#include "ECS/ALoader.hpp"

class WinLoader : public ALoader {
    public:
        bool load_components(const std::string& so_path, registry& reg) override;
        bool load_system(const std::string& so_path, SystemType type) override;
};

#endif // _WIN32