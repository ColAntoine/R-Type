#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"

#include "Constants.hpp"
#include <vector>
#include "Core/Server/Protocol/Protocol.hpp"

class Browser : public AGameState {
    public:
        Browser() = default;
        ~Browser() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        std::string get_name() const override { return "Browser"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

    private:
        void join_room_callback();
        std::vector<entity> _instanceEntities;
        void rebuild_instance_ui(const std::vector<RType::Protocol::InstanceInfo>& list);
        // Event callback id for instance-connected UI action
        EventBus::CallbackId _instanceConnectedCallbackId{0};
};