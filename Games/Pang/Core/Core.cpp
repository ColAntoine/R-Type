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

Core::Core()
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

    loop();
}

void Core::initWindow()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pang");
    SetTargetFPS(60);
}

void Core::loadSystems()
{
    std::cout << "Loading game systems dynamically..." << std::endl;
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Gravity.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libposition_system.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_BallSys.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Controlable.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_InvincibilitySys.so");
    _systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Render.so");
    std::cout << "Loaded " << _systemLoader.get_system_count() << " systems total." << std::endl;

    _componentFactory = _systemLoader.get_factory();

    Ball ball;
    ball.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f, 100.f));
    ball.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f + 5.f, 150.f));

    Player player;
    player.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 30.f));
}

void Core::loop()
{
    float deltaTime = 0.f;

    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Pang Game", 10, 10, 20, RAYWHITE);
        _systemLoader.update_all_systems(_reg, deltaTime);

        EndDrawing();
    }

    CloseWindow();
}
