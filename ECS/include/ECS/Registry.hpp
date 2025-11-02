#pragma once

#include <typeindex>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <any>
#include <stdexcept>
#include <vector>
#include <functional>
#include <utility>

#include "SparseSet.hpp"
#include "Entity.hpp"

// Registry that stores one sparse_set<Component> per component type using std::any.
// Also responsible for managing entities (spawn/kill) and for adding/removing components.
class registry {
    public:
        registry();
        ~registry();
        registry(registry const&) = delete;
        registry& operator=(registry const&) = delete;

        // Adds a new component array for Component if absent, returns reference.
        template<typename Component>
        sparse_set<Component>& register_component() {
            auto key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) {
                auto [ins_it, ok] = _components_arrays.emplace(key, std::any(sparse_set<Component>{}));
                it = ins_it;

                _erasers.emplace_back([this](entity const& e) {
                    auto *arr = this->get_if<Component>();
                    if (!arr) return;
                    auto idx = static_cast<typename sparse_set<Component>::size_type>(static_cast<size_t>(e));
                    if (arr->has(idx)) arr->erase(idx);
                });
            }
            return std::any_cast<sparse_set<Component>&>(it->second);
        }

        template<typename Component>
        sparse_set<Component>& get_components() {
            auto key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end())
                throw std::out_of_range("registry::get_components: component not registered");
            return std::any_cast<sparse_set<Component>&>(it->second);
        }

        // Const overload: retrieve const reference to the component array. Throws if not registered.
        template<typename Component>
        sparse_set<Component> const& get_components() const {
            auto key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end())
                throw std::out_of_range("registry::get_components const: component not registered");
            return std::any_cast<sparse_set<Component> const&>(it->second);
        }

        template<typename Component>
        Component& get_component(entity const& e) {
            auto& arr = get_components<Component>();
            using idx_t = typename sparse_set<Component>::size_type;
            idx_t idx = static_cast<idx_t>(static_cast<size_t>(e));
            if (!arr.has(idx)) {
                throw std::out_of_range("registry::get_component: entity does not have component");
            }
            return arr[idx];
        }

        template<typename Component>
        Component const& get_component(entity const& e) const {
            auto const& arr = get_components<Component>();
            using idx_t = typename sparse_set<Component>::size_type;
            idx_t idx = static_cast<idx_t>(static_cast<size_t>(e));
            if (!arr.has(idx)) {
                throw std::out_of_range("registry::get_component const: entity does not have component");
            }
            return arr[idx];
        }

        // add_component accepts both lvalue and rvalue (universal reference)
        template<typename Component>
        typename sparse_set<Component>::reference_type
        add_component(entity const& to, Component&& c) {
            auto &arr = register_component<Component>();
            using idx_t = typename sparse_set<Component>::size_type;
            return arr.insert_at(static_cast<idx_t>(static_cast<size_t>(to)), std::forward<Component>(c));
        }

        template<typename Component, typename... Params>
        typename sparse_set<Component>::reference_type
        emplace_component(entity const& to, Params&&... p) {
            auto &arr = register_component<Component>();
            using idx_t = typename sparse_set<Component>::size_type;
            return arr.emplace_at(static_cast<idx_t>(static_cast<size_t>(to)), std::forward<Params>(p)...);
        }

        template<typename Component>
        void remove_component(entity const& from) {
            auto *arr = get_if<Component>();
            if (!arr) return;
            using idx_t = typename sparse_set<Component>::size_type;
            idx_t idx = static_cast<idx_t>(static_cast<size_t>(from));
            if (idx < arr->size()) arr->erase(idx);
        }

        // helper: get_if
        template<typename Component>
        sparse_set<Component>* get_if() noexcept {
            auto key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) return nullptr;
            return std::any_cast<sparse_set<Component>>(&it->second);
        }

        template<class... Components, typename Function>
        void add_system(Function&& f) {
            using Fn = std::decay_t<Function>;
            _systems.emplace_back([func = Fn(std::forward<Function>(f))](registry &r) mutable {
                func(r, r.get_components<Components>()...);
            });
        }

        template<class... Components, typename Function>
        void add_system(Function const& f) {
            _systems.emplace_back([f](registry &r) {
                f(r, r.get_components<Components>()...);
            });
        }

        void run_systems();
        entity spawn_entity();
        entity entity_from_index(std::size_t idx) const;
        void kill_entity(entity const& e);

        // Random seed management for deterministic gameplay
        void set_random_seed(unsigned int seed);
        unsigned int get_random_seed() const;
        bool has_random_seed() const;

    private:
        std::unordered_map<std::type_index, std::any> _components_arrays;
        std::vector<std::function<void(entity const&)>> _erasers;
        std::vector<std::function<void(registry&)>> _systems;
        std::vector<std::size_t> _free_ids;
        std::size_t _next_id{0};
        
        // Random seed for deterministic gameplay (server-controlled in multiplayer)
        unsigned int _random_seed{0};
        bool _seed_set{false};
};
