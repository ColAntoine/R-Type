#pragma once

#include <string>

class SceneManager {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        void change_scene(const std::string &name) {}
};
