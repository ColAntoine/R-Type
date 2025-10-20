/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Sat, Oct, 2025                                                     *
 * Title           - RTYPE                                                              *
 * Description     -                                                                    *
 *     Core                                                                             *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       ▄▀▀█▄▄▄▄  ▄▀▀▄▀▀▀▄  ▄▀▀█▀▄    ▄▀▀▀█▀▀▄  ▄▀▀█▄▄▄▄  ▄▀▄▄▄▄   ▄▀▀▄ ▄▄             *
 *      ▐  ▄▀   ▐ █   █   █ █   █  █  █    █  ▐ ▐  ▄▀   ▐ █ █    ▌ █  █   ▄▀            *
 *        █▄▄▄▄▄  ▐  █▀▀▀▀  ▐   █  ▐  ▐   █       █▄▄▄▄▄  ▐ █      ▐  █▄▄▄█             *
 *        █    ▌     █          █        █        █    ▌    █         █   █             *
 *       ▄▀▄▄▄▄    ▄▀        ▄▀▀▀▀▀▄   ▄▀        ▄▀▄▄▄▄    ▄▀▄▄▄▄▀   ▄▀  ▄▀             *
 *       █    ▐   █         █       █ █          █    ▐   █     ▐   █   █               *
 *       ▐        ▐         ▐       ▐ ▐          ▐        ▐         ▐   ▐               *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "Core.hpp"

Core::Core() : _currentState(GameState::MENU), _systemsLoaded(false), _componentFactory(nullptr)
{
    _reg.register_component<position>();
    _reg.register_component<velocity>();
    _reg.register_component<Ball>();
    _reg.register_component<Gravity>();
}

Core& Core::getInstance()
{
    static Core core;

    return core;
}

void Core::run()
{
    initWindow();
    loadSystems();
    initScenes();

    loop();
}

void Core::initWindow()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pang");
    SetTargetFPS(60);
}

void Core::loadSystems()
{
    if (_systemsLoaded) return;

    std::cout << "Loading game systems dynamically..." << std::endl;
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Gravity.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libposition_system.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_BallSys.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Controlable.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_InvincibilitySys.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Render.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Shoot.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_BallSpawner.so");
    std::cout << "Loaded " << _systemLoader.get_system_count() << " systems total." << std::endl;

    _componentFactory = _systemLoader.get_factory();
    _systemsLoaded = true;
}

void Core::initScenes()
{
    _scenes[GameState::MENU] = std::make_unique<MenuScene>();
    _scenes[GameState::INGAME] = std::make_unique<InGameScene>(_reg, _systemLoader, _componentFactory);
    _scenes[GameState::END] = std::make_unique<EndScene>();
    _scenes[_currentState]->init(0.0f);
}

void Core::loop()
{
    float deltaTime = 0.f;

    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();

        auto nextState = _scenes[_currentState]->update(deltaTime);
        if (nextState.has_value()) {
            if (nextState.value() == GameState::QUIT) {
                break;
            }

            if (nextState.value() != _currentState) {
                _scenes[_currentState]->destroy(deltaTime);
                changeState(nextState.value());
                _scenes[_currentState]->init(deltaTime);
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        _scenes[_currentState]->render(deltaTime);

        EndDrawing();
    }

    CloseWindow();
}void Core::changeState(GameState newState)
{
    _currentState = newState;
}
