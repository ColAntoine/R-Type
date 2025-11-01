#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Zipper.hpp"

#include "UI/Components/GlitchButton.hpp"

#include "Constants.hpp"

class InGameHudState : public AGameState {

    struct FPSText {}; // Tag struct for quick access to FPS text
    struct ScoreText {}; // Tag struct for quick access to Score text

    public:
        InGameHudState() = default;
        ~InGameHudState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "InGameHud"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

    private:
        // Helper to quickly access FPS text
        std::shared_ptr<UI::UIText> get_fps_text() {
            auto* ui_comps = _registry.get_if<UI::UIComponent>();
            auto* fps_tags = _registry.get_if<FPSText>();

            if (!ui_comps || !fps_tags) return nullptr;

            for (auto [ui_comp, fps_tag, ent] : zipper(*ui_comps, *fps_tags)) {
                return std::dynamic_pointer_cast<UI::UIText>(ui_comp._ui_element);
            }
            return nullptr;
        }

        void set_score_text();

        int _score = 0;
        entity _scoreTextEntity;
};