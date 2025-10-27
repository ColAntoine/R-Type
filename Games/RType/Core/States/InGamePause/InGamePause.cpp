#include "InGamePause.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"

void InGamePauseState::enter()
{
    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);


    this->_registry.register_component<UI::UIButton>();

    setup_ui();
    _initialized = true;
}

void InGamePauseState::exit()
{
    _initialized = false;
}

void InGamePauseState::pause()
{

}

void InGamePauseState::resume()
{

}


void InGamePauseState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto bgPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(100), renderManager.scaleSizeH(100))
        .backgroundColor(Color{0, 0, 0, 125})
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto bgPanelEntity = this->_registry.spawn_entity();
    this->_registry.add_component(bgPanelEntity, UI::UIComponent(bgPanel));

    auto menuButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(-10))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("MENU")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .neonColors(Color{0, 229, 255, 255}, Color{0, 229, 255, 100})
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            // this->play_solo();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto menuButtonEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(menuButtonEntity, UI::UIComponent(menuButton));

    auto continueButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("CONTINUE")
        .green()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        // .neonColors(Color{0, 229, 255, 255}, Color{0, 229, 255, 100})
        // .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            // this->play_solo();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto continueButtonEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(continueButtonEntity, UI::UIComponent(continueButton));


    auto quitButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(10))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("QUIT")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .neonColors(Color{0, 229, 255, 255}, Color{0, 229, 255, 100})
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            // this->play_solo();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto quitButtonEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(quitButtonEntity, UI::UIComponent(quitButton));
}
