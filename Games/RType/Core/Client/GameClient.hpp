#pragma once

#include "../AGameCore.hpp"

class GameClient : public AGameCore {
    public:
        GameClient();
        ~GameClient();

        bool init() override;
        void run() override;
        void update(float delta) override;
        void shutdown() override;
};
