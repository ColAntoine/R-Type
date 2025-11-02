#include "GameOver.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"
#include <iostream>

void GameOverState::enter()
{
    std::cout << "[InGameExit] Entering state" << std::endl;
    _systemLoader->load_components("build/lib/libECS" + ext, _registry);
    _systemLoader->load_system("build/lib/systems/librender_UISystem" + ext, ILoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();

    // Subscribe to GAME_OVER event to capture the score
    auto& eventBus = MessagingManager::instance().get_event_bus();
    _gameOverEventCallbackId = eventBus.subscribe("GAME_OVER", [this](const Event& event) {
        try {
            _finalScore = event.get<int>("score");
            std::cout << "[GameOverState] Received final score: " << _finalScore << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "[GameOverState] Error getting score from event: " << ex.what() << std::endl;
            _finalScore = 0;
        }
    });

    setup_ui();
    subscribe_to_ui_event();
    _initialized = true;
}

void GameOverState::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    eventBus.unsubscribe(_gameOverEventCallbackId);
    _initialized = false;
}

void GameOverState::pause()
{
}

void GameOverState::resume()
{
}

void GameOverState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();
    auto &theme = ThemeManager::instance().getTheme();


    auto dialogPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(60), renderManager.scaleSizeH(50))
        .backgroundColor(theme.panelColor)
        .border(5, theme.panelBorderColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto dialogEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(dialogEnt, UI::UIComponent(dialogPanel));

    auto gameOverTitle = TextBuilder()
        .centered(renderManager.scalePosY(-15))
        .text("GAME OVER")
        .fontSize(renderManager.scaleSizeW(10))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto gameOverTitleEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(gameOverTitleEntity, UI::UIComponent(gameOverTitle));

    std::string scoreText = "FINAL SCORE: " + std::to_string(_finalScore);
    auto scoreDisplay = TextBuilder()
        .centered(renderManager.scalePosY(0))
        .text(scoreText)
        .fontSize(renderManager.scaleSizeW(4))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto scoreDisplayEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(scoreDisplayEntity, UI::UIComponent(scoreDisplay));

    auto backButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(15))
        .size(renderManager.scaleSizeW(25), renderManager.scaleSizeH(8))
        .text("GO BACK TO MENU")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2.5))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->go_back_to_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(backButtonEntity, UI::UIComponent(backButton));
}

void GameOverState::update(float delta_time)
{
    if (!_initialized)
        return;

    _systemLoader->update_all_systems(_registry, delta_time, ILoader::LogicSystem);
}

void GameOverState::go_back_to_menu()
{
    std::cout << "[GameOverState] Going back to menu" << std::endl;
    if (this->_stateManager) {
        auto& audioManager = AudioManager::instance();
        audioManager.get_music().stopAll();

        this->_stateManager->clear_states();
        this->_stateManager->push_state("MenusBackground");
        this->_stateManager->push_state("MainMenu");
    }
}

