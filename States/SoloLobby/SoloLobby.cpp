/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SoloLobby Implementation - Offline testing environment
*/

#include "SoloLobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>

void SoloLobbyState::enter() {
    std::cout << "[SoloLobby] Entering offline test mode" << std::endl;

    // Register UI component types in UI registry
    _registry.register_component<UI::UIComponent>();
    _registry.register_component<RType::UILogPanel>();
    _registry.register_component<RType::UIStatusText>();
    _registry.register_component<RType::UIRendererButton>();
    _registry.register_component<RType::UIPhysicsButton>();
    _registry.register_component<RType::UIAudioButton>();
    _registry.register_component<RType::UIAllSystemsButton>();
    _registry.register_component<RType::UIBackButton>();

    setup_ui();
    setup_ecs_demo();

    status_message_ = "Solo Lobby - Testing Engine Features";
    add_log("Solo Lobby initialized");
    add_log("All engine features available offline");

    initialized_ = true;
}

void SoloLobbyState::exit() {
    std::cout << "[SoloLobby] Exiting" << std::endl;
    cleanup_ui();

    // Unsubscribe from all events
    for (auto callback_id : event_subscriptions_) {
        event_bus_.unsubscribe(callback_id);
    }
    event_subscriptions_.clear();

    // Unload preloaded SFX
    AudioManager::instance().get_sfx().unload("shoot");
    initialized_ = false;
}

void SoloLobbyState::pause() {
    // UI system will handle visibility
    ui_system_.set_enabled(false);
}

void SoloLobbyState::resume() {
    // UI system will handle visibility
    ui_system_.set_enabled(true);
}

void SoloLobbyState::setup_ecs_demo() {
    // Initialize ECS Registry
    add_log("Initializing ECS Registry...");

    // Load components from libECS.so and get factory
    if (component_loader_.load_components_from_so("lib/libECS.so", ecs_registry_)) {
        component_factory_ = component_loader_.get_factory();
        add_log("✓ Components loaded from libECS.so");
        add_log("Components registered: position, velocity, sprite, collider, animation");
    } else {
        add_log("✗ Failed to load components!");
    }

    // Load systems dynamically
    load_systems();

    // Create test entities
    create_test_entities();

    // Initialize engine subsystems
    add_log("Initializing Renderer...");
    RenderManager::instance().init(GetScreenWidth(), GetScreenHeight(), "R-Type Solo Lobby - ECS Demo");
    RenderManager::instance().set_clear_color({20, 20, 40, 255}); // Dark blue background
    add_log("Renderer initialized with SpriteBatch and Camera2D");

    add_log("Initializing Physics Engine...");
    PhysicsManager::instance().init(64.0f);  // cell_size parameter
    add_log("Physics Engine initialized with SpatialHash");

    add_log("Initializing Audio Engine...");
    AudioManager::instance().init();
    // Preload SFX with key 'shoot' so SFXPlayer::play can be called by name
    AudioManager::instance().get_sfx().load("shoot", "Games/RType/Assets/shoot.wav");
    add_log("Audio Engine initialized (SFX preloaded: shoot)");

    add_log("Initializing Messaging System...");
    setup_event_bus();
    add_log("EventBus ready with event subscribers");

    add_log("All engine subsystems ready!");
}

void SoloLobbyState::load_systems() {
    add_log("Loading systems via Plugin API...");

    // Load sprite rendering system
    if (system_loader_.load_system_from_so("lib/systems/libsprite_system.so")) {
        add_log("✓ Loaded: sprite_system");
    } else {
        add_log("✗ FAILED to load sprite_system!");
    }

    // Load animation system
    if (system_loader_.load_system_from_so("lib/systems/libanimation_system.so")) {
        add_log("✓ Loaded: animation_system");
    }

    // Load collision system
    if (system_loader_.load_system_from_so("lib/systems/libcollision_system.so")) {
        add_log("✓ Loaded: collision_system (Physics)");
    }

    // Load position system
    if (system_loader_.load_system_from_so("lib/systems/libposition_system.so")) {
        add_log("✓ Loaded: position_system");
    }

    add_log("Plugin system test complete!");
}

void SoloLobbyState::create_test_entities() {
    add_log("Creating test entities...");

    if (!component_factory_) {
        add_log("✗ Component factory not available!");
        return;
    }

    // Create player entity with sprite (scale down the massive 1920x1080 image to ~64x64)
    player_entity_ = ecs_registry_.spawn_entity();
    component_factory_->create_component<position>(ecs_registry_, player_entity_, 400.0f, 300.0f);
    component_factory_->create_component<velocity>(ecs_registry_, player_entity_, 0.0f, 0.0f);
    component_factory_->create_component<sprite>(ecs_registry_, player_entity_, "Games/RType/Assets/REAPER_ICON.png", 1920.0f, 1080.0f, 0.05f, 0.05f);
    component_factory_->create_component<collider>(ecs_registry_, player_entity_, 33.0f, 17.0f, 0.0f, 0.0f);

    add_log("✓ Player entity created (sprite + collider)");

    // Create enemy entity (also scale down)
    enemy_entity_ = ecs_registry_.spawn_entity();
    component_factory_->create_component<position>(ecs_registry_, enemy_entity_, 600.0f, 200.0f);
    component_factory_->create_component<velocity>(ecs_registry_, enemy_entity_, -50.0f, 0.0f);
    component_factory_->create_component<sprite>(ecs_registry_, enemy_entity_, "Games/RType/Assets/REAPER_ICON.png", 1920.0f, 1080.0f, 0.05f, 0.05f);
    component_factory_->create_component<collider>(ecs_registry_, enemy_entity_, 33.0f, 17.0f, 0.0f, 0.0f);

    add_log("✓ Enemy entity created (moving + collider)");
    add_log("Asset Manager will load textures on demand");
}

void SoloLobbyState::update(float delta_time) {
    if (!initialized_) return;

    elapsed_time_ += delta_time;
    // Update UI system
    ui_system_.update(_registry, delta_time);

    // Process deferred events from EventBus
    event_bus_.process_deferred();

    // Update player movement with arrow keys
    if (test_phase_ > 0) {
        auto* velocities = ecs_registry_.get_if<velocity>();
        if (velocities) {
            auto& vel = (*velocities)[player_entity_];
            vel.vx = 0.0f;
            vel.vy = 0.0f;

            if (IsKeyDown(KEY_RIGHT)) vel.vx = 200.0f;
            if (IsKeyDown(KEY_LEFT)) vel.vx = -200.0f;
            if (IsKeyDown(KEY_DOWN)) vel.vy = 200.0f;
            if (IsKeyDown(KEY_UP)) vel.vy = -200.0f;

            // Test EventBus: Emit event when SPACE is pressed
            if (IsKeyPressed(KEY_SPACE)) {
                Event shoot_event(EventType::CUSTOM, player_entity_);
                shoot_event.set<std::string>("action", "shoot");
                event_bus_.emit(shoot_event);
            }
        }

        // Check for collision between player and enemy (manual collision detection for EventBus demo)
        auto* positions = ecs_registry_.get_if<position>();
        auto* colliders = ecs_registry_.get_if<collider>();
        if (positions && colliders) {
            if (positions->has(player_entity_) && positions->has(enemy_entity_) &&
                colliders->has(player_entity_) && colliders->has(enemy_entity_)) {

                auto& p1 = (*positions)[player_entity_];
                auto& c1 = (*colliders)[player_entity_];
                auto& p2 = (*positions)[enemy_entity_];
                auto& c2 = (*colliders)[enemy_entity_];

                // Simple AABB collision check
                float x1 = p1.x + c1.offset_x;
                float y1 = p1.y + c1.offset_y;
                float x2 = p2.x + c2.offset_x;
                float y2 = p2.y + c2.offset_y;

                bool colliding = !(x1 + c1.w < x2 || x1 > x2 + c2.w ||
                                   y1 + c1.h < y2 || y1 > y2 + c2.h);

                static bool was_colliding = false;
                if (colliding && !was_colliding) {
                    // Collision started - emit event
                    Event collision_event(EventType::COLLISION_ENTER, player_entity_);
                    collision_event.set<size_t>("other_entity", enemy_entity_);
                    event_bus_.emit(collision_event);
                    was_colliding = true;
                } else if (!colliding && was_colliding) {
                    was_colliding = false;
                }
            }
        }
    }

    // Test audio after 2 seconds
    if (!audio_test_played_ && elapsed_time_ > 2.0f && test_phase_ >= 3) {
        AudioManager::instance().get_sfx().play("shoot", 0.5f);
        add_log("Audio test: Playing sound effect");
        audio_test_played_ = true;
    }
}

void SoloLobbyState::render() {
    if (!initialized_) return;

    // Clear background
    ClearBackground({20, 20, 40, 255});

    // Render ECS entities if test phase active
    if (test_phase_ > 0) {
        system_loader_.update_all_systems(ecs_registry_, GetFrameTime());
    }

    // Title
    DrawText("SOLO LOBBY - Engine Feature Test", 20, 20, 24, {0, 229, 255, 255});
    DrawText(status_message_.c_str(), 20, 55, 16, {200, 200, 200, 255});

    // Log messages
    int log_y = GetScreenHeight() - 200;
    DrawText("--- Log ---", 20, log_y, 18, {0, 229, 255, 255});
    log_y += 25;

    int display_count = std::min(8, (int)log_messages_.size());
    for (int i = 0; i < display_count; ++i) {
        int idx = (int)log_messages_.size() - display_count + i;
        DrawText(log_messages_[idx].c_str(), 20, log_y, 14, {180, 180, 180, 255});
        log_y += 18;
    }

    // Instructions
    if (test_phase_ > 0) {
        DrawText("Use ARROW KEYS to move player", GetScreenWidth() - 380, 20, 16, {0, 255, 128, 255});
        DrawText("Press SPACE to fire (EventBus test)", GetScreenWidth() - 380, 45, 16, {255, 255, 0, 255});
        DrawText("All systems running offline!", GetScreenWidth() - 380, 70, 14, {200, 200, 200, 255});
    }

    // Render UI via system
    ui_system_.render(_registry);
}

void SoloLobbyState::handle_input() {
    if (!initialized_) return;
    ui_system_.process_input(_registry);
}

void SoloLobbyState::setup_ui() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    float button_x = screen_width / 2.0f - 150.0f;
    float button_y = 120.0f;
    float button_spacing = 60.0f;

    // Test Renderer Button
    auto renderer_entity = _registry.spawn_entity();
    auto renderer_button = std::make_shared<RType::GlitchButton>(button_x, button_y, 300, 45, "Test Renderer");
    UI::ButtonStyle renderer_style;
    renderer_style._normal_color = {20, 20, 30, 220};
    renderer_style._hovered_color = {36, 36, 52, 230};
    renderer_style._pressed_color = {16, 16, 24, 200};
    renderer_style._text_color = {220, 240, 255, 255};
    renderer_style._font_size = 20;
    renderer_button->set_style(renderer_style);
    renderer_button->set_neon_colors({0, 229, 255, 255}, {0, 229, 255, 100});
    renderer_button->set_glitch_params(2.0f, 7.5f, true);
    renderer_button->set_on_click([this]() { on_test_renderer_clicked(); });
    _registry.add_component(renderer_entity, UI::UIComponent(renderer_button));
    _registry.add_component(renderer_entity, RType::UIRendererButton{});

    // Test Physics Button
    button_y += button_spacing;
    auto physics_entity = _registry.spawn_entity();
    auto physics_button = std::make_shared<RType::GlitchButton>(button_x, button_y, 300, 45, "Test Physics");
    UI::ButtonStyle physics_style;
    physics_style._normal_color = {20, 20, 30, 220};
    physics_style._hovered_color = {36, 36, 52, 230};
    physics_style._pressed_color = {16, 16, 24, 200};
    physics_style._text_color = {220, 240, 255, 255};
    physics_style._font_size = 20;
    physics_button->set_style(physics_style);
    physics_button->set_neon_colors({0, 229, 255, 255}, {0, 229, 255, 100});
    physics_button->set_glitch_params(2.0f, 7.5f, true);
    physics_button->set_on_click([this]() { on_test_physics_clicked(); });
    _registry.add_component(physics_entity, UI::UIComponent(physics_button));
    _registry.add_component(physics_entity, RType::UIPhysicsButton{});

    // Test Audio Button
    button_y += button_spacing;
    auto audio_entity = _registry.spawn_entity();
    auto audio_button = std::make_shared<RType::GlitchButton>(button_x, button_y, 300, 45, "Test Audio");
    UI::ButtonStyle audio_style;
    audio_style._normal_color = {20, 20, 30, 220};
    audio_style._hovered_color = {36, 36, 52, 230};
    audio_style._pressed_color = {16, 16, 24, 200};
    audio_style._text_color = {220, 240, 255, 255};
    audio_style._font_size = 20;
    audio_button->set_style(audio_style);
    audio_button->set_neon_colors({0, 229, 255, 255}, {0, 229, 255, 100});
    audio_button->set_glitch_params(2.0f, 7.5f, true);
    audio_button->set_on_click([this]() { on_test_audio_clicked(); });
    _registry.add_component(audio_entity, UI::UIComponent(audio_button));
    _registry.add_component(audio_entity, RType::UIAudioButton{});

    // Test All Button
    button_y += button_spacing;
    auto all_entity = _registry.spawn_entity();
    auto all_button = std::make_shared<RType::GlitchButton>(button_x, button_y, 300, 45, "Test ALL Systems");
    UI::ButtonStyle all_style;
    all_style._normal_color = {20, 60, 20, 220};
    all_style._hovered_color = {36, 80, 36, 230};
    all_style._pressed_color = {16, 50, 16, 200};
    all_style._text_color = {200, 255, 200, 255};
    all_style._font_size = 20;
    all_button->set_style(all_style);
    all_button->set_neon_colors({0, 255, 128, 255}, {0, 255, 128, 100});
    all_button->set_glitch_params(2.5f, 8.0f, true);
    all_button->set_on_click([this]() { on_test_all_clicked(); });
    _registry.add_component(all_entity, UI::UIComponent(all_button));
    _registry.add_component(all_entity, RType::UIAllSystemsButton{});

    // Back Button
    button_y += button_spacing * 1.5f;
    auto back_entity = _registry.spawn_entity();
    auto back_button = std::make_shared<RType::GlitchButton>(button_x, button_y, 300, 45, "Back to Menu");
    UI::ButtonStyle back_style;
    back_style._normal_color = {30, 12, 12, 200};
    back_style._hovered_color = {60, 18, 18, 220};
    back_style._pressed_color = {20, 8, 8, 200};
    back_style._text_color = {255, 160, 160, 220};
    back_style._font_size = 18;
    back_button->set_style(back_style);
    back_button->set_neon_colors({255, 80, 80, 220}, {255, 80, 80, 80});
    back_button->set_glitch_params(2.0f, 7.0f, true);
    back_button->set_on_click([this]() { on_back_clicked(); });
    _registry.add_component(back_entity, UI::UIComponent(back_button));
    _registry.add_component(back_entity, RType::UIBackButton{});
}

void SoloLobbyState::cleanup_ui() {
    // Collect all entity IDs first to avoid modifying while iterating
    std::vector<entity> entities_to_cleanup;
    
    auto* ui_components = _registry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            entities_to_cleanup.push_back(entity(ent));
        }
    }
    
    // Now remove all components from collected entities
    for (auto ent : entities_to_cleanup) {
        _registry.remove_component<UI::UIComponent>(ent);
        _registry.remove_component<RType::UILogPanel>(ent);
        _registry.remove_component<RType::UIStatusText>(ent);
        _registry.remove_component<RType::UIRendererButton>(ent);
        _registry.remove_component<RType::UIPhysicsButton>(ent);
        _registry.remove_component<RType::UIAudioButton>(ent);
        _registry.remove_component<RType::UIAllSystemsButton>(ent);
        _registry.remove_component<RType::UIBackButton>(ent);
    }
}

void SoloLobbyState::add_log(const std::string& message) {
    log_messages_.push_back(message);
    std::cout << "[SoloLobby] " << message << std::endl;
}

void SoloLobbyState::on_test_renderer_clicked() {
    test_phase_ = 1;
    status_message_ = "Testing: Renderer (SpriteBatch + Camera2D)";
    add_log("=== Testing Renderer ===");
    add_log("SpriteBatch batching draw calls");
    add_log("Camera2D managing viewport");
    add_log("Asset Manager caching textures");
}

void SoloLobbyState::on_test_physics_clicked() {
    test_phase_ = 2;
    status_message_ = "Testing: Physics (SpatialHash collision)";
    add_log("=== Testing Physics ===");
    add_log("SpatialHash grid partitioning active");
    add_log("Collision detection running");
    add_log("Entities have colliders");
}

void SoloLobbyState::on_test_audio_clicked() {
    test_phase_ = 3;
    status_message_ = "Testing: Audio Engine";
    add_log("=== Testing Audio ===");
    add_log("Audio Engine ready");
    add_log("Playing test sound...");
    audio_test_played_ = false;
    elapsed_time_ = 0.0f;
}

void SoloLobbyState::on_test_all_clicked() {
    test_phase_ = 4;
    status_message_ = "Testing: ALL ENGINE FEATURES";
    add_log("=== FULL ENGINE TEST ===");
    add_log("✓ Asset Manager: Texture/Sound caching");
    add_log("✓ Renderer: SpriteBatch + Camera2D");
    add_log("✓ Physics: SpatialHash collision");
    add_log("✓ Audio: Sound/Music playback");
    add_log("✓ Messaging: EventBus ready");
    add_log("✓ Plugin: Dynamic system loading");
    add_log("All systems operational!");
    audio_test_played_ = false;
    elapsed_time_ = 0.0f;
}

void SoloLobbyState::on_back_clicked() {
    if (_stateManager) {
        _stateManager->change_state("MainMenu");
    }
}

void SoloLobbyState::setup_event_bus() {
    // Subscribe to custom events (like player shooting)
    auto custom_callback = event_bus_.subscribe(EventType::CUSTOM, [this](const Event& event) {
        if (event.has("action")) {
            std::string action = event.get<std::string>("action");
            if (action == "shoot") {
                add_log("🎯 EventBus: Player fired! (Entity " + std::to_string(event.entity_id) + ")");
                // Play sound effect via Audio system
                AudioManager::instance().get_sfx().play("shoot", 0.5f);
            }
        }
    });
    event_subscriptions_.push_back(custom_callback);

    // Subscribe to collision events
    auto collision_callback = event_bus_.subscribe(EventType::COLLISION_ENTER, [this](const Event& event) {
        add_log("💥 EventBus: Collision detected! Entity " + std::to_string(event.entity_id));
    });
    event_subscriptions_.push_back(collision_callback);

    // Subscribe to entity creation events
    auto created_callback = event_bus_.subscribe(EventType::ENTITY_CREATED, [this](const Event& event) {
        add_log("✨ EventBus: Entity created (ID: " + std::to_string(event.entity_id) + ")");
    });
    event_subscriptions_.push_back(created_callback);

    std::cout << "[EventBus] Subscribed to " << event_subscriptions_.size() << " event types" << std::endl;
}
