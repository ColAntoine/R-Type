#include "Credits.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>
#include <sstream>

const char* creditsText = R"(
This project was born from a simple desire:
to bring a timeless classic back to life,
by learning, creating, and sharing
the passion for code and video games.

Thanks to everyone who supported this
development, tested the builds,
shared ideas, and reminded us that the joy
of creating always comes from teamwork.


                            - The development team
)";



void CreditsState::enter()
{
    std::cout << "[CreditsState] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    _initialized = true;
}

void CreditsState::exit()
{
    std::cout << "[CreditsState] Exiting state" << std::endl;
    _initialized = false;
}

void CreditsState::pause()
{
    std::cout << "[CreditsState] Pausing state" << std::endl;
}

void CreditsState::resume()
{
    std::cout << "[CreditsState] Resuming state" << std::endl;
}

void CreditsState::play_back_settings()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("Settings");
    }
}

void CreditsState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto title = TextBuilder()
        .at(renderManager.scalePosX(15), renderManager.scalePosY(10))
        .text(creditsText)
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    title->setCustomRender([](const UI::UIText& text) {
        Vector2 pos = text.getPosition();
        Color color = text.getStyle().getTextColor();
        int font_size = text.getStyle().getFontSize();
        std::string fullText = text.getText();
        std::stringstream ss(fullText);
        std::string line;
        int line_number = 0;
        while (std::getline(ss, line)) {
            if (line.empty()) {
                line_number++;
                continue;
            }
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y) + line_number * font_size;
            auto &renderer = RenderManager::instance();
            renderer.draw_text(line.c_str(), x, y, font_size, color);
            line_number++;
        }
    });

    auto titleEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(titleEntity, UI::UIComponent(title));

    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(11), renderManager.scalePosY(80))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK TO SETTINGS")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            this->play_back_settings();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(backButtonEntity, UI::UIComponent(backButton));
}
