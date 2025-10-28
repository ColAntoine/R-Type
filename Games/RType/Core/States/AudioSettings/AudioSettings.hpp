#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

class AudioSettingsState : public AGameState {
    public:
        AudioSettingsState() = default;
        ~AudioSettingsState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        std::string get_name() const override { return "AudioSettings"; }

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }
    private:
        void applyGeneralVolumeChange(MoveDirection direction);
        void applyMusicVolumeChange(MoveDirection direction);
        void applySFXVolumeChange(MoveDirection direction);

        void updateVolumeText(entity entity, const std::string& label, float volume);

        entity _generalVolumeTextEntity;
        entity _musicVolumeTextEntity;
        entity _sfxVolumeTextEntity;
};
