#pragma once

#include <iostream>

class SceneManager {
    public:
        enum SceneType {
            MAIN_MENU = 0,
            LOBBY,
            IN_GAME,
            PAUSE_MENU,
            GAME_OVER
        };

        SceneManager() = default;
        ~SceneManager() = default;

        void changeScene(const SceneType &newScene);
    private:
        SceneType currentScene{MAIN_MENU};
};
