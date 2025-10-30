#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

class LoadingVideoState : public AGameState {
    public:
        LoadingVideoState() = default;
        ~LoadingVideoState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;
        void update(float delta_time) override;

        std::string get_name() const override { return "LoadingVideo"; }
    private:
        void setup_image_sequence(const std::string& directoryPath);
        void switch_to_main_menu();

        std::vector<std::string> _imagesPaths;
        int _currentImageIndex{0};
        entity _imgEntity;

        float _timeAccumulator = 0.0f;
        EventBus::CallbackId _skipEventCallbackId;
};
