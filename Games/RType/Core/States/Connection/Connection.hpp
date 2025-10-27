#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"

#include "Constants.hpp"

class Connection : public AGameState {
    struct IpFieldTag {};
    struct PortFieldTag {};
    struct NameFieldTag {};
    public:
        Connection() = default;
        ~Connection() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        std::string get_name() const override { return "Connection"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

    private:
        std::string PlayerName_;
        std::string ServerIp_;
        std::string ServerPort_;
        // Prevent multiple simultaneous connection attempts from repeated clicks
        bool connecting_ = false;
        // Quick access helpers (similar to InGameHud::FPSText pattern)
        std::shared_ptr<UI::UIInputField> GetIpInput();
        std::shared_ptr<UI::UIInputField> GetPortInput();
        std::shared_ptr<UI::UIInputField> GetNameInput();

        void connection_callback();
};