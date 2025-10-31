#include "MenusBG.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "UI/ThemeManager.hpp"

#include <iostream>
#include <random>
#include <cmath>

void MenusBackgroundState::enter()
{
    std::cout << "[MenusBackground] Entering state" << std::endl;

    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
    #endif

    this-> _systemLoader->load_components(ecsLib, _registry);
    this->_systemLoader->load_system(uiSys, ILoader::RenderSystem);

    this->_registry.register_component<UI::UIText>();

    setup_ui();
    subscribe_to_ui_event();
    this->_initialized = true;
}

void MenusBackgroundState::exit()
{
    std::cout << "[Menus Background] Exiting state" << std::endl;
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    this->_initialized = false;
}

void MenusBackgroundState::pause()
{
    std::cout << "[Menus Background] Pausing state" << std::endl;
}

void MenusBackgroundState::resume()
{
    std::cout << "[Menus Background] Resuming state" << std::endl;
}

void MenusBackgroundState::update(float delta_time)
{
    if (!this->_initialized)
        return;

    this->_asciiTimer += delta_time;
    if (this->_asciiTimer >= this->_asciiInterval) {
        this->_asciiTimer = 0.0f;

        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<> char_dist(0, static_cast<int>(this->_asciiCharset.size() - 1));
        std::uniform_int_distribution<> row_dist(0, this->_asciiRows - 1);
        std::uniform_int_distribution<> col_dist(0, this->_asciiCols - 1);

        // Change random characters
        int changes = std::max(1, static_cast<int>(this->_asciiRows * this->_asciiCols / 20));
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(rng);
            int c = col_dist(rng);
            this->_asciiGrid[r][c] = this->_asciiCharset[char_dist(rng)];
        }
    }
    this->_systemLoader->update_all_systems(this->_registry, delta_time, ILoader::LogicSystem);
}

void MenusBackgroundState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    int sw = winInfos.getWidth();
    int sh = winInfos.getHeight();
    int font_w = std::max(1, this->_asciiFontSize / 2);
    int font_h = this->_asciiFontSize;

    this->_asciiCols = std::max(10, (sw + font_w - 1) / font_w);
    this->_asciiRows = std::max(8, (sh + font_h - 1) / font_h);
    this->_asciiGrid.assign(this->_asciiRows, std::string(this->_asciiCols, ' '));

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> char_dist(0, static_cast<int>(this->_asciiCharset.size() - 1));
    std::uniform_int_distribution<> row_dist(0, this->_asciiRows - 1);
    std::uniform_int_distribution<> col_dist(0, this->_asciiCols - 1);

    for (int i = 0; i < static_cast<int>(this->_asciiRows * this->_asciiCols / 4); ++i) {
        int r = row_dist(rng);
        int c = col_dist(rng);
        this->_asciiGrid[r][c] = this->_asciiCharset[char_dist(rng)];
    }

    auto asciiText = TextBuilder()
        .at(0, 0)
        .text(" ")
        .fontSize(this->_asciiFontSize)
        .textColor(theme.textBackgroundColor)
        .alignment(UI::TextAlignment::Left)
    .build(sw, sh);

    asciiText->setCustomRender([this](const UI::UIText& text) {
        Vector2 pos = text.getPosition();
        Color color = text.getStyle().getTextColor();
        int font_size = text.getStyle().getFontSize();
        int font_w = std::max(1, font_size / 2);

        for (int r = 0; r < static_cast<int>(this->_asciiRows); ++r) {
            for (int c = 0; c < static_cast<int>(this->_asciiCols); ++c) {
                char ch = this->_asciiGrid[r][c];
                if (ch == ' ') continue;

                int x = static_cast<int>(pos.x) + c * font_w;
                int y = static_cast<int>(pos.y) + r * font_size;

                std::string ch_str(1, ch);
                auto &renderer = RenderManager::instance();
                renderer.draw_text(ch_str.c_str(), x, y, font_size, color);
            }
        }
    });

    auto asciiTextEntity = this->_registry.spawn_entity();
    this->_registry.add_component(asciiTextEntity, UI::UIComponent(asciiText));
    this->_asciiTextEntity = asciiTextEntity;
    this->_asciiTimer = 0.0f;
}
