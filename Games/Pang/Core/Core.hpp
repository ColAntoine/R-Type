/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Sat, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     Core                                                                             *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *             ███████╗██████╗ ██╗████████╗███████╗ ██████╗██╗  ██╗                     *
 *             ██╔════╝██╔══██╗██║╚══██╔══╝██╔════╝██╔════╝██║  ██║                     *
 *             █████╗  ██████╔╝██║   ██║   █████╗  ██║     ███████║                     *
 *             ██╔══╝  ██╔═══╝ ██║   ██║   ██╔══╝  ██║     ██╔══██║                     *
 *             ███████╗██║     ██║   ██║   ███████╗╚██████╗██║  ██║                     *
 *             ╚══════╝╚═╝     ╚═╝   ╚═╝   ╚══════╝ ╚═════╝╚═╝  ╚═╝                     *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#ifndef INCLUDED_CORE_HPP
    #define INCLUDED_CORE_HPP

    #include <iostream>
    #include <map>
    #include <raylib.h>
    #include <memory>

// ====================================ECS======================================
#include "ECS/Registry.hpp"
#include "ECS/ILoader.hpp"
// =============================================================================

// =====================================Entity==================================
#include "Entity/Components/Ball/Ball.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/Gravity/Gravity.hpp"

#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
// =============================================================================

// ===================================SCENE=====================================
#include "Scene/IScene.hpp"
#include "Scene/MenuScene/MenuScene.hpp"
#include "Scene/InGameScene/InGameScene.hpp"
#include "Scene/EndScene/EndScene.hpp"
// =============================================================================

#ifdef _WIN32
  #include "ECS/WinLoader.hpp"
  using PlatformLoader = WinLoader;
#else
  #include "ECS/LinuxLoader.hpp"
  using PlatformLoader = LinuxLoader;
#endif

class Core
{
    public:
        static Core& getInstance();
        void run();

        void changeState(GameState newState);

    private:
        Core();
        ~Core() = default;

        // * INIT
        void initWindow();
        void loadSystems();

        // * Game
        void loop();

        // * State Management
        void initScenes();
        std::map<GameState, std::unique_ptr<IScene>> _scenes;
        GameState _currentState;

        // * VARS
        registry _reg;
        IComponentFactory *_componentFactory;
        PlatformLoader _systemLoader;
        entity _ballEntity;
        bool _systemsLoaded;
};

#endif

/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * MIT License                                                                          *
 * Copyright (c) 2025 paul1.emeriau                                                     *
 *                                                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a copy         *
 * of this software and associated documentation files (the "Software"), to deal        *
 * in the Software without restriction, including without limitation the rights         *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            *
 * copies of the Software, and to permit persons to whom the Software is                *
 * furnished to do so, subject to the following conditions:                             *
 *                                                                                      *
 * The above copyright notice and this permission notice shall be included in all       *
 * copies or substantial portions of the Software.                                      *
 *                                                                                      *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        *
 * SOFTWARE.                                                                            *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */
