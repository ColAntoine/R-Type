#include "Core/StatesBis/AGameState.hpp"

namespace RType {
    struct UIMainPanel : public IComponent {};
    struct UITitleText : public IComponent {};
    struct UIPlayButton : public IComponent {};
    struct UISettingsButton : public IComponent {};
    struct UIQuitButton : public IComponent {};
}

class MainMenuState : public AGameState {
    public:
        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "MainMenu"; }
    private:
        // MainMenu specific members
};