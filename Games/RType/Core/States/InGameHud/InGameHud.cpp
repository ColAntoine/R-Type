#include "InGameHud.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "UI/ThemeManager.hpp"

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

    MessagingManager::instance().get_event_bus().subscribe(EventTypes::SCORE_INCREASED, [this](const Event& event) {
        _score += event.get<int>("amount");
        set_score_text();
    });
    _initialized = true;
}

void InGameHudState::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
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

