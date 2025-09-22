#pragma once

#include <algorithm>
#include <SFML/Graphics.hpp>

#include "ecs/registry.hpp"
#include "ecs/components.hpp"

// Demo application that owns a registry and an SFML window, and runs simple systems.
class DemoApp {
  public:
    DemoApp();
    ~DemoApp() = default;

    // Run the main loop; returns exit code
    int run();

  private:
    void setup_scene();
    void handle_events();
    void update(float dt);
    void render();

  private:
    registry reg_;
    sf::RenderWindow window_;
};
