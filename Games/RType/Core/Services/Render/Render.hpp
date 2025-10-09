#pragma once

#include "Core/Services/ServiceManager.hpp"
#include "Core/Client/EventManager.hpp"
#include "Core/Client/Events.hpp"
#include "Core/Window/GameWindow.hpp"
#include "ECS/Registry.hpp"
#include <memory>

class RenderService : public IService {
    private:
        EventManager* event_manager_;
        std::unique_ptr<GameWindow> window_;
        bool window_should_close_ = false;

    public:
        RenderService(EventManager* event_manager);

        void initialize() override;
        void shutdown() override;
        void update(float delta_time) override;

        // Rendering methods
        void begin_frame();
        void end_frame();
        void render_entities(registry& ecs_registry);
        void render_ui(const std::string& status_text);

        // Window properties
        bool should_close() const;
        int get_fps() const;
        int get_width() const;
        int get_height() const;
        float get_frame_time() const;
};