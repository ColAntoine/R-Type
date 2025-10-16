/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ALoader
*/

#include "ECS/ALoader.hpp"

ALoader::ALoader(ALoader&& other) noexcept
    : library_handle_(other.library_handle_)
    , factory_(other.factory_)
    , systems_(std::move(other.systems_)) {
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
        systems_ = std::move(other.systems_);

        // Reset other
        other.library_handle_ = nullptr;
        other.factory_ = nullptr;
    }
    return *this;
}

void ALoader::update_all_systems(registry& r, float dt) {
    for (auto& loaded_sys : systems_) {
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

void ALoader::update_system_by_name(const std::string& name, registry& r, float dt) {
    for (auto& loaded_sys : systems_) {
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

std::vector<std::string> ALoader::get_system_names() const {
    std::vector<std::string> names;
    for (const auto& loaded_sys : systems_) {
        if (loaded_sys.system) {
            names.push_back(loaded_sys.system->get_name());
        }
    }
    return names;
}


size_t ALoader::get_system_count() const {
    return systems_.size();
}