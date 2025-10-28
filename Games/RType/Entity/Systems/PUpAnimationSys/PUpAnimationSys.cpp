/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PUpAnimationSys system implementation
*/

#include <memory>
#include <cmath>
#include <iostream>

#include "PUpAnimationSys.hpp"

void PUpAnimationSys::update(registry& r, float dt)
{
    getAnimations(r);
    updateAnimation(r, dt);
}

void PUpAnimationSys::getAnimations(registry &r)
{
    auto pUpArr = r.get_if<PUpAnimation>();
    std::vector<entity> entToKill;

    if (!pUpArr) return;

    for (auto [pUp, ent] : zipper(*pUpArr)) {
        _animationQueue.push(pUp);
        entToKill.push_back(entity(ent));
    }

    for (auto ent : entToKill) {
        r.kill_entity(ent);
    }
}

void PUpAnimationSys::updateAnimation(registry &r, float dt)
{
    RenderManager &renderManager = RenderManager::instance();

    if (!pendingAnimation(r)) return;
    _current._elapsed += dt;

    auto animArr = r.get_if<animationArrow>();
    auto posArr = r.get_if<position>();
    if (animArr && posArr) {
        for (auto [anim, pos, ent] : zipper(*animArr, *posArr)) {
            if (entity(ent) != _arrowEnt) continue;
            float amp = renderManager.scaleSizeH(2.0f);
            float freq = 2.0f;
            pos.y = _arrowBaseY + amp * std::sin(_current._elapsed * freq * 2.0f * 3.14159265f);
        }
    }



    if (_current._duration > 0.0f && _current._elapsed >= _current._duration) {
        if (_arrowEnt.value() != 0) r.kill_entity(_arrowEnt);
        _arrowEnt = entity(0);
        _isDone = true;

        auto textArr = r.get_if<animationText>();
        if (!textArr) return;

        for (auto [text, ent] : zipper(*textArr)) {
            r.kill_entity(entity(ent));
        }
    }
}

bool PUpAnimationSys::pendingAnimation(registry &r)
{
    RenderManager &renderManager = RenderManager::instance();

    if (_isDone) {
        if (!_animationQueue.empty()) {
            _current = _animationQueue.front();
            _animationQueue.pop();
            _current._elapsed = 0.0f;
            _current._duration = 3.0f;
            _isDone = false;

            _arrowEnt = r.spawn_entity();

            float orig_w = 500.f, orig_h = 500.f;
            float screen_w = renderManager.get_screen_infos().getWidth();
            float screen_h = renderManager.get_screen_infos().getHeight();
            float target_w = screen_w * 0.05f;
            float target_h = screen_h * 0.10f;
            float sx = target_w / orig_w;
            float sy = target_h / orig_h;
            auto &sp = r.emplace_component<sprite>(_arrowEnt, std::string(RTYPE_PATH_ASSETS) + "pUpArrow.gif", orig_w, orig_h, sx, sy);
            sp.rotation = _current._up ? 180.f : 0.f;
            r.emplace_component<animationArrow>(_arrowEnt);

            float startX = renderManager.scalePosX(45);
            float startY = renderManager.scalePosY(75);
            r.emplace_component<position>(_arrowEnt, startX, startY);
            _arrowBaseY = startY;

            float textX = renderManager.scalePosX(44);
            float textY = renderManager.scalePosY(65);

            auto uiText = TextBuilder()
                .at(textX, textY)
                .text(_current._text)
                .fontSize(static_cast<int>(renderManager.scaleSizeW(2.0f)))
                .textColor(WHITE)
            .build(screen_w, screen_h);

            uiText->setCustomRender([](const UI::UIText& text) {
                auto &renderer = RenderManager::instance();
                Vector2 pos = text.getPosition();
                int font_size = text.getStyle().getFontSize();
                double t = GetTime();
                Color color = (fmod(t, 0.5) < 0.25) ? RED : WHITE;
                renderer.draw_text(text.getText().c_str(), static_cast<int>(pos.x), static_cast<int>(pos.y), font_size, color);
            });

            auto textEnt = r.spawn_entity();
            r.emplace_component<UI::UIComponent>(textEnt, UI::UIComponent(uiText));
            r.emplace_component<animationText>(textEnt);
            return true;
        }
        return false;
    }
    return true;
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<PUpAnimationSys>();
    }
}
