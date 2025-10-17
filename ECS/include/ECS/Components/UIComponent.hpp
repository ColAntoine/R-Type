/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Component wrapper for ECS
*/

#pragma once

#include "IComponent.hpp"
#include "../UI/IUIComponent.hpp"
#include <memory>

namespace UI {
    class UIComponent : public IComponent {
    public:
        std::shared_ptr<UI::IUIComponent> _ui_element;

        UIComponent() = default;

        // No implicit conversion allowed to avoid the creation of an Abstract class
        explicit UIComponent(std::shared_ptr<UI::IUIComponent> element)
            : _ui_element(std::move(element)) {}

        template<typename T, typename... Args>
        static UIComponent create(Args&&... args) {
            return UIComponent(std::make_shared<T>(std::forward<Args>(args)...));
        }
    };
}