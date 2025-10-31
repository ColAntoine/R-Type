/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ALoader
*/

#include "ECS/ALoader.hpp"
#include <utility>

ALoader::ALoader(ALoader&& other) noexcept
    : library_handle_(other.library_handle_)
    , factory_(other.factory_)
    , _logicSystems(std::move(other._logicSystems))
    , _renderSystems(std::move(other._renderSystems)) {
    other.library_handle_ = nullptr;
    other.factory_ = nullptr;
}

ALoader& ALoader::operator=(ALoader&& other) noexcept {
    if (this != &other) {
        // Clean up current resources   
        this->~ALoader();

        // Move from other
        library_handle_ = other.library_handle_;
        factory_ = other.factory_;
        _logicSystems = std::move(other._logicSystems);
        _renderSystems = std::move(other._renderSystems);

        // Reset other
        other.library_handle_ = nullptr;
        other.factory_ = nullptr;
    }
    return *this;
}

void ALoader::update_all_systems(registry& r, float dt, SystemType type) {
    auto &systems = (type == LogicSystem) ? _logicSystems : _renderSystems;
    for (auto& loaded_sys : systems) {
        if (loaded_sys.system) {
            try {
                loaded_sys.system->update(r, dt);
            } catch (const std::exception& e) {
                std::cerr << "Exception in system " << loaded_sys.system->get_name()
                          << ": " << e.what() << std::endl;
            }
        }
    }
}

void ALoader::update_system_by_name(const std::string& name, registry& r, float dt, SystemType type) {
    auto &systems = (type == LogicSystem) ? _logicSystems : _renderSystems;
    for (auto& loaded_sys : systems) {
        if (loaded_sys.system && loaded_sys.system->get_name() == name) {
            try {
                loaded_sys.system->update(r, dt);
            } catch (const std::exception& e) {
                std::cerr << "Exception in system " << name << ": " << e.what() << std::endl;
            }
            return;
        }
    }
    std::cerr << "System not found: " << name << std::endl;
}

IComponentFactory* ALoader::get_factory() const {
    return factory_;
}

bool ALoader::is_loaded() const {
    return library_handle_ != nullptr;
}

std::vector<std::string> ALoader::get_system_names(SystemType type) const {
    std::vector<std::string> names;
    switch (type) {
        case LogicSystem:
            for (const auto& loaded_sys : _logicSystems) {
                if (loaded_sys.system) {
                    names.push_back(loaded_sys.system->get_name());
                }
            }
            break;
        case RenderSystem:
            for (const auto& loaded_sys : _renderSystems) {
                if (loaded_sys.system) {
                    names.push_back(loaded_sys.system->get_name());
                }
            }
            break;
        default:
            break;
    }
    return names;
}


size_t ALoader::get_system_count(SystemType type) const {
    switch (type) {
        case LogicSystem: return _logicSystems.size();
        case RenderSystem: return _renderSystems.size();
        default: return 0;
    }
}