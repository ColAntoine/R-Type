/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Controlable system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class Controlable : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Controlable"; }
private:
    void movePlayer(registry &r);
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);