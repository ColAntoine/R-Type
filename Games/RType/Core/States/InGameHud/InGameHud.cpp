#include "InGameHud.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"

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
    _initialized = true;
}

void InGameHudState::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    _initialized = false;
}

void InGameHudState::pause()
{

}

void InGameHudState::resume()
{

}


void InGameHudState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto warningText = TextBuilder()
        .at(winInfos.getWidth() / 2, 20)
        .textColor(RED)
        .text("!!!!The HUD is just a mockup!!!!")
        .alignment(UI::TextAlignment::Center)
        .fontSize(30)
    .build(winInfos.getWidth(), winInfos.getWidth());

    auto warningTextEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(warningTextEnt, UI::UIComponent(warningText));

    auto posText = TextBuilder()
        .at(10.f, 10.f)
        .text("Position: (FAKEPOS, FAKEPOS)")
        .fontSize(30)
    .build(winInfos.getWidth(), winInfos.getWidth());

    auto posTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(posTextent, UI::UIComponent(posText));

    auto scoreText = TextBuilder()
        .at(10.f, 45.f)
        .text("Score: FAKE SCORE")
        .fontSize(30)
    .build(winInfos.getWidth(), winInfos.getWidth());

    auto scoreTextEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(scoreTextEnt, UI::UIComponent(scoreText));

    auto fpsText = TextBuilder()
        .at(winInfos.getWidth() - 10.f, 10.f)
        .text("FPS: " + std::to_string(GetFPS()))
        .textColor(GREEN)
        .fontSize(30)
        .alignment(UI::TextAlignment::Right)
    .build(winInfos.getWidth(), winInfos.getWidth());

    auto fpsTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(fpsTextent, UI::UIComponent(fpsText));
    _registry.add_component<FPSText>(fpsTextent, FPSText());    // ? Tag to access it quickly

    auto weaponText = TextBuilder()
        .at(10.f, winInfos.getHeight() - 85.f)
        .text("Weapon: Plasma Cannon")
        .textColor(YELLOW)
        .fontSize(30)
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto weaponTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(weaponTextent, UI::UIComponent(weaponText));

    auto ammoText = TextBuilder()
        .at(10.f, winInfos.getHeight() - 55.f)
        .text("Ammo: 25/100")
        .textColor(WHITE)
        .fontSize(25)
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto ammoTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(ammoTextent, UI::UIComponent(ammoText));

    auto damageText = TextBuilder()
        .at(10.f, winInfos.getHeight() - 30.f)
        .text("Damage: 45")
        .textColor(ORANGE)
        .fontSize(20)
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto damageTextent = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(damageTextent, UI::UIComponent(damageText));
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

