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
    
    // Load gravity system - applies gravity to entities with velocity + gravity components
    if (_systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_Gravity.so")) {
        std::cout << "✓ Loaded: GravitySystem" << std::endl;
    } else {
        std::cerr << "✗ Failed to load GravitySystem!" << std::endl;
    }

    // Load position system - updates positions based on velocity
    if (_systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libposition_system.so")) {
        std::cout << "✓ Loaded: PositionSystem" << std::endl;
    } else {
        std::cerr << "✗ Failed to load PositionSystem!" << std::endl;
    }

    // Load ball bounce system - handles ball collision with screen bounds
    if (_systemLoader.load_system_from_so("Games/Pang/build/lib/systems/libpang_BallSys.so")) {
        std::cout << "✓ Loaded: BallSystem" << std::endl;
    } else {
        std::cerr << "✗ Failed to load BallSystem!" << std::endl;
    }

    std::cout << "Loaded " << _systemLoader.get_system_count() << " systems total." << std::endl;

    _componentFactory = _systemLoader.get_factory();

    // Spawn initial ball entity
    Ball ball;
    ball.spawn(_componentFactory, _reg, position(SCREEN_WIDTH / 2.f, 100.f));
}

void Core::loop()
{
    float deltaTime = 0.f;

    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();

        // Update all systems (GravitySystem and PositionSystem)
        _systemLoader.update_all_systems(_reg, deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Pang Game", 10, 10, 20, DARKGRAY);

        // Draw all balls
        auto *pos_arr = _reg.get_if<position>();
        auto *ball_arr = _reg.get_if<Ball>();

        if (pos_arr && ball_arr) {
            for (size_t i = 0; i < pos_arr->size(); ++i) {
                auto entity_id = pos_arr->entity_at(i);
                if (ball_arr->has(entity_id)) {
                    auto& pos = (*pos_arr)[i];
                    auto& ball = ball_arr->get(entity_id);
                    DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), ball._radius, ball._color);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
