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

    // auto warningText = TextBuilder()
    //     .at(renderManager.scalePosX(50), renderManager.scalePosY(1))
    //     .textColor(RED)
    //     .text("R-TYPE /!\\ WARNING /!\\")
    //     .alignment(UI::TextAlignment::Center)
    //     .fontSize(renderManager.scaleSizeW(2))
    // .build(winInfos.getWidth(), winInfos.getWidth());

    // auto warningTextEnt = _registry.spawn_entity();
    // _registry.add_component<UI::UIComponent>(warningTextEnt, UI::UIComponent(warningText));

    auto scoreText = TextBuilder()
        .at(renderManager.scalePosX(1), renderManager.scalePosY(1))
        .text("Score: " + std::to_string(_score))
        .fontSize(renderManager.scaleSizeW(3))
    .build(winInfos.getWidth(), winInfos.getWidth());

    _scoreTextEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(_scoreTextEntity, UI::UIComponent(scoreText));
    _registry.add_component<ScoreText>(_scoreTextEntity, ScoreText());    // ? Tag to access it quickly

    auto fpsText = TextBuilder()
        .at(renderManager.scalePosX(99), renderManager.scalePosY(1))
        .text("FPS: " + std::to_string(GetFPS()))
        .textColor(GREEN)
        .fontSize(renderManager.scaleSizeW(2))
        .alignment(UI::TextAlignment::Right)
    .build(winInfos.getWidth(), winInfos.getWidth());

    auto fpsTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(fpsTextent, UI::UIComponent(fpsText));
    _registry.add_component<FPSText>(fpsTextent, FPSText());    // ? Tag to access it quickly

    // auto weaponText = TextBuilder()
    //     .at(10.f, winInfos.getHeight() - 85.f)
    //     .text("Weapon: Plasma Cannon")
    //     .textColor(YELLOW)
    //     .fontSize(30)
    // .build(winInfos.getWidth(), winInfos.getHeight());

    // auto weaponTextent = _registry.spawn_entity();
    // _registry.add_component<UI::UIComponent>(weaponTextent, UI::UIComponent(weaponText));

    // auto ammoText = TextBuilder()
    //     .at(10.f, winInfos.getHeight() - 55.f)
    //     .text("Ammo: 25/100")
    //     .textColor(WHITE)
    //     .fontSize(25)
    // .build(winInfos.getWidth(), winInfos.getHeight());

    // auto ammoTextent = _registry.spawn_entity();
    // _registry.add_component<UI::UIComponent>(ammoTextent, UI::UIComponent(ammoText));

    // auto damageText = TextBuilder()
    //     .at(10.f, winInfos.getHeight() - 30.f)
    //     .text("Damage: 45")
    //     .textColor(ORANGE)
    //     .fontSize(20)
    // .build(winInfos.getWidth(), winInfos.getHeight());

    // auto damageTextent = _registry.spawn_entity();
    // _registry.add_component<UI::UIComponent>(damageTextent, UI::UIComponent(damageText));
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

