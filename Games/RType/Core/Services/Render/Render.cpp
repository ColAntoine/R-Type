#include "Render.hpp"

RenderService::RenderService(EventManager* event_manager) : event_manager_(event_manager) {
}

void RenderService::initialize() {
    window_ = std::make_unique<GameWindow>(1024, 768, "R-Type - Service Architecture");
    if (!window_->initialize()) {
        throw std::runtime_error("Failed to initialize render window");
    }
}

void RenderService::shutdown() {
    if (window_) {
        window_->close();
    }
}

void RenderService::update(__attribute_maybe_unused__ float delta_time) {
    // Check if window should close
    if (window_ && window_->should_window_close()) {
        window_should_close_ = true;
    }
}

void RenderService::begin_frame() {
    if (window_) {
        window_->begin_drawing();
        window_->clear_background(DARKBLUE);
    }
}

void RenderService::end_frame() {
    if (window_) {
        window_->end_drawing();
    }
}

void RenderService::render_entities(__attribute_maybe_unused__ registry& ecs_registry) {
    // Entity rendering is now handled by the draw_system through DLLoader
    // This method is kept for compatibility but does nothing
}

void RenderService::render_ui(const std::string& status_text) {
    if (window_) {
        window_->draw_text("R-Type - Service Architecture", 10, 10, 20, WHITE);
        window_->draw_text(status_text, 10, 40, 16, LIGHTGRAY);
        window_->draw_text("Red = You, Blue = Other players", 10, 65, 14, LIGHTGRAY);
        window_->draw_text("Controls: Arrow Keys to move, ESC to quit", 10, window_->get_height() - 30, 16, LIGHTGRAY);
    }
}

bool RenderService::should_close() const {
    return window_should_close_ || (window_ && window_->should_window_close());
}

int RenderService::get_fps() const {
    return window_ ? window_->get_fps() : 0;
}

int RenderService::get_width() const {
    return window_ ? window_->get_width() : 0;
}

int RenderService::get_height() const {
    return window_ ? window_->get_height() : 0;
}

float RenderService::get_frame_time() const {
    return window_ ? window_->get_frame_time() : 0.0f;
}