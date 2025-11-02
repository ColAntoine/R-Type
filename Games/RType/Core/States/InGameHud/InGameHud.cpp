#include "InGameHud.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "UI/ThemeManager.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"

#include "ECS/Messaging/MessagingManager.hpp"

void InGameHudState::enter()
{
    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
    #endif

    _systemLoader->load_components(ecsLib, _registry);
    _systemLoader->load_system(uiSys, ILoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();
    this->_registry.register_component<FPSText>();

    setup_ui();
    subscribe_to_ui_event();

    auto& eventBus = MessagingManager::instance().get_event_bus();
    
    // Subscribe to score changes
    eventBus.subscribe(EventTypes::SCORE_INCREASED, [this](const Event& event) {
        _score += event.get<int>("amount");
        set_score_text();
    });
    
    // Subscribe to health changes
    _healthCallbackId = eventBus.subscribe("PLAYER_HEALTH_CHANGED", [this](const Event& event) {
        if (event.has("health")) {
            _health = event.get<int>("health");
            set_health_text(_health);
        }
    });
    
    // Subscribe to stats changes
    _statsCallbackId = eventBus.subscribe("PLAYER_STATS_CHANGED", [this](const Event& event) {
        if (event.has("speed")) _speed = event.get<int>("speed");
        if (event.has("firerate")) _firerate = event.get<int>("firerate");
        if (event.has("damage")) _damage = event.get<int>("damage");
        set_stats_text(_speed, _firerate, _damage);
    });
    
    _initialized = true;
}

void InGameHudState::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    if (_healthCallbackId != -1) eventBus.unsubscribe(_healthCallbackId);
    if (_statsCallbackId != -1) eventBus.unsubscribe(_statsCallbackId);
    _initialized = false;
}

void InGameHudState::pause() {}

void InGameHudState::resume() {}

void InGameHudState::set_score_text()
{
    auto* ui_comps = _registry.get_if<UI::UIComponent>();
    auto* score_tags = _registry.get_if<ScoreText>();

    if (!ui_comps || !score_tags)
        return;

    for (auto [ui_comp, score_tag, ent] : zipper(*ui_comps, *score_tags)) {
        auto text_ui = std::dynamic_pointer_cast<UI::UIText>(ui_comp._ui_element);
        if (text_ui) {
            text_ui->setText("Score: " + std::to_string(_score));
        }
        return;
    }
}

void InGameHudState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    auto scoreText = TextBuilder()
        .at(renderManager.scalePosX(1), renderManager.scalePosY(1))
        .text("Score: " + std::to_string(_score))
        .textColor(theme.gameColors.info)
        .fontSize(renderManager.scaleSizeW(3))
    .build(winInfos.getWidth(), winInfos.getWidth());

    _scoreTextEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(_scoreTextEntity, UI::UIComponent(scoreText));
    _registry.add_component<ScoreText>(_scoreTextEntity, ScoreText());    // ? Tag to access it quickly

    // Health text - displayed in bottom left
    auto healthText = TextBuilder()
        .at(renderManager.scalePosX(1), renderManager.scalePosY(95))
        .text("Health: 100")
        .textColor(theme.gameColors.primary)
        .fontSize(renderManager.scaleSizeW(2.5f))
    .build(winInfos.getWidth(), winInfos.getWidth());

    _healthTextEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(_healthTextEntity, UI::UIComponent(healthText));
    _registry.add_component<HealthText>(_healthTextEntity, HealthText());

    // Stats text - displayed in bottom center
    auto statsText = TextBuilder()
        .at(renderManager.scalePosX(35), renderManager.scalePosY(95))
        .text("Speed: 0 | Fire: 0 | Dmg: 0")
        .textColor(theme.gameColors.secondary)
        .fontSize(renderManager.scaleSizeW(2.5f))
    .build(winInfos.getWidth(), winInfos.getWidth());

    _statsTextEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(_statsTextEntity, UI::UIComponent(statsText));
    _registry.add_component<StatsText>(_statsTextEntity, StatsText());

    auto fpsText = TextBuilder()
        .at(renderManager.scalePosX(99), renderManager.scalePosY(1))
        .text("FPS: " + std::to_string(GetFPS()))
        .textColor(theme.gameColors.info)
        .fontSize(renderManager.scaleSizeW(2))
        .alignment(UI::TextAlignment::Right)
    .build(winInfos.getWidth(), winInfos.getWidth());

    auto fpsTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(fpsTextent, UI::UIComponent(fpsText));
    _registry.add_component<FPSText>(fpsTextent, FPSText());    // ? Tag to access it quickly
}

void InGameHudState::update(float delta_time)
{
    if  (!_initialized)
        return;

    auto fps_text = get_fps_text();
    if (fps_text) {
        fps_text->setText("FPS: " + std::to_string(GetFPS()));
    }

    _systemLoader->update_all_systems(_registry, delta_time, ILoader::LogicSystem);
}

void InGameHudState::set_health_text(int health)
{
    auto health_text = get_health_text();
    if (health_text) {
        health_text->setText("Health: " + std::to_string(health));
    }
}

void InGameHudState::set_stats_text(int speed, int firerate, int damage)
{
    auto stats_text = get_stats_text();
    if (stats_text) {
        std::string stats_str = "Speed: " + std::to_string(speed) + 
                               " | Fire: " + std::to_string(firerate) + 
                               " | Dmg: " + std::to_string(damage);
        stats_text->setText(stats_str);
    }
}