// filepath: Games/RType/Core/States/LoadingVideo/LoadingVideo.cpp
#include "LoadingVideo.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <map>
#include "Constants.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "Core/KeyBindingManager/KeyBindingManager.hpp"
#include "UI/ThemeManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Audio/AudioManager.hpp"

void LoadingVideoState::enter()
{
    std::cout << "[LoadingVideoState] Entering state" << std::endl;

    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
        const std::string spriteLib = "build/lib/systems/libsprite_system.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
        const std::string spriteLib = "build/lib/systems/libsprite_system.so";
    #endif

    _systemLoader->load_components(ecsLib, _registry);
    _systemLoader->load_system(uiSys, ILoader::RenderSystem);
    _systemLoader->load_system(spriteLib, ILoader::RenderSystem);

    setup_ui();

    _skipEventCallbackId = MessagingManager::instance().get_event_bus().subscribe(EventTypes::KEY_PRESSED, [this](const Event& event) {
        std::string key = event.get<std::string>("key");
        if (key == "SPACE") {
            this->switch_to_main_menu();
            MessagingManager::instance().get_event_bus().unsubscribe_deferred(_skipEventCallbackId);
        }
    });

    auto& audioManager = AudioManager::instance();

    if (audioManager.is_initialized()) {
        try {
            auto& musicCache = audioManager.get_music().getMusicCache();
            if (musicCache.find("loading_theme") == musicCache.end()) {
                std::string menuMusicPath = std::string(RTYPE_PATH_ASSETS) + "Audio/Loading.mp3";
                audioManager.get_music().load("loading_theme", menuMusicPath);
                audioManager.get_music().play("loading_theme", audioManager.get_music_volume());
            } else {
                audioManager.get_music().play("loading_theme", audioManager.get_music_volume());
            }
        } catch (const std::exception& ex) {
            std::cerr << "[LoadingVideoState] Error playing music: " << ex.what() << std::endl;
        }
    }

    _initialized = true;
}

void LoadingVideoState::exit()
{
    std::cout << "[LoadingVideoState] Exiting state" << std::endl;
    _imagesPaths.clear();
    MessagingManager::instance().get_event_bus().unsubscribe_deferred(_skipEventCallbackId);
    _initialized = false;

    auto& audioManager = AudioManager::instance();
    if (audioManager.is_initialized()) {
        try {
            audioManager.get_music().stopAll();
            audioManager.get_music().clear();
        } catch (const std::exception& ex) {
            std::cerr << "[LoadingVideoState] Error stopping music: " << ex.what() << std::endl;
        }
    }
}

void LoadingVideoState::pause()
{
    std::cout << "[LoadingVideoState] Pausing state" << std::endl;
}

void LoadingVideoState::resume()
{
    std::cout << "[LoadingVideoState] Resuming state" << std::endl;
}

void LoadingVideoState::switch_to_main_menu()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("MenusBackground");
        this->_stateManager->push_state("MainMenu");
    }
}

void LoadingVideoState::update(float delta_time)
{
    (void)delta_time;
    if (!_initialized || _imagesPaths.empty()) {
        switch_to_main_menu();
        return;
    }

    if (_currentImageIndex >= static_cast<int>(_imagesPaths.size())) {
        switch_to_main_menu();
        return;
    }

    _timeAccumulator += delta_time;

    if (_timeAccumulator >= delta_time * 2) {
        auto& spr = _registry.get_component<sprite>(_imgEntity);
        spr.texture_path = _imagesPaths[_currentImageIndex];
        _timeAccumulator = 0.0f;
        _currentImageIndex++;
    }
}

void LoadingVideoState::setup_image_sequence(const std::string& directoryPath)
{
    auto winInfos = RenderManager::instance().get_screen_infos();
    std::map<int, std::string> imageMap;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            std::string filename = entry.path().stem().string();
            if (filename.starts_with("dedsecjoinus")) {
                std::string numStr = filename.substr(12);
                try {
                    int num = std::stoi(numStr);
                    imageMap[num] = entry.path().string();
                } catch (const std::exception&) {
                    std::cerr << "[LoadingVideoState] Invalid number in filename: " << filename << std::endl;
                }
            }
        }
    }

    for (const auto& pair : imageMap) {
        _imagesPaths.push_back(pair.second);
    }

    if (!_imagesPaths.empty()) {
        _imgEntity = _registry.spawn_entity();
        _registry.emplace_component<sprite>(_imgEntity, _imagesPaths[0], winInfos.getWidth(), winInfos.getHeight(), 1.0f, 1.0f);
        _registry.emplace_component<position>(_imgEntity, winInfos.getWidth() / 2, winInfos.getHeight() / 2);
    }
}

void LoadingVideoState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();
    auto theme = ThemeManager::instance().getTheme();

    std::filesystem::path animPath = std::string(RTYPE_PATH_ASSETS) + "joinusAnim/";
    if (!std::filesystem::exists(animPath) || !std::filesystem::is_directory(animPath)) {
        switch_to_main_menu();
        return;
    }
    setup_image_sequence(animPath.string());

    auto text = TextBuilder()
        .at(renderManager.scalePosX(5), renderManager.scalePosY(90))
        .text("Press SPACE to skip")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto textEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(textEntity, UI::UIComponent(text));
}
