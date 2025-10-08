
#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <iterator>

// SparseSet: stores components densely and maps entity IDs to indices in packed array.
template <typename Component, typename Allocator = std::allocator<Component>>
class sparse_set {
    public:
        using value_type = Component;
        using reference_type = value_type&;
        using const_reference_type = value_type const&;
        using size_type = std::size_t;


        sparse_set() = default;
        sparse_set(sparse_set const&) = default;
        sparse_set(sparse_set&&) noexcept = default;
        ~sparse_set() = default;

        sparse_set& operator=(sparse_set const&) = default;
        sparse_set& operator=(sparse_set&&) noexcept = default;

        // Access component by packed index
        reference_type operator[](size_type idx) { return _packed[idx]; }
        const_reference_type operator[](size_type idx) const { return _packed[idx]; }

        // Iterators for packed array
        auto begin() { return _packed.begin(); }
        auto end() { return _packed.end(); }
        auto begin() const { return _packed.begin(); }
        auto end() const { return _packed.end(); }
        auto cbegin() const { return _packed.cbegin(); }
        auto cend() const { return _packed.cend(); }

        size_type size() const { return _packed.size(); }

        // Insert or update component for entity
        reference_type insert_at(size_type entity_id, Component const& comp) {
            ensure_sparse_size(entity_id);
            if (has(entity_id)) {
                return _packed[_sparse[entity_id]] = comp;
            }
            _packed.push_back(comp);
            _packed_entities.push_back(entity_id);
            _sparse[entity_id] = _packed.size() - 1;
            return _packed.back();
        }

        reference_type insert_at(size_type entity_id, Component&& comp) {
            ensure_sparse_size(entity_id);
            if (has(entity_id)) {
                return _packed[_sparse[entity_id]] = std::move(comp);
            }
            _packed.push_back(std::move(comp));
            _packed_entities.push_back(entity_id);
            _sparse[entity_id] = _packed.size() - 1;
            return _packed.back();
        }

        template <class... Params>
        reference_type emplace_at(size_type entity_id, Params&&... params) {
            ensure_sparse_size(entity_id);
            if (has(entity_id)) {
                return _packed[_sparse[entity_id]] = Component(std::forward<Params>(params)...);
            }
            _packed.emplace_back(std::forward<Params>(params)...);
            _packed_entities.push_back(entity_id);
            _sparse[entity_id] = _packed.size() - 1;
            return _packed.back();
        }

        // Remove component for entity
        void erase(size_type entity_id) {
            if (!has(entity_id)) return;
            size_type idx = _sparse[entity_id];
            size_type last = _packed.size() - 1;
            if (idx != last) {
                std::swap(_packed[idx], _packed[last]);
                std::swap(_packed_entities[idx], _packed_entities[last]);
                _sparse[_packed_entities[idx]] = idx;
            }
            _packed.pop_back();
            _sparse[entity_id] = npos;
            _packed_entities.pop_back();
        }

        // Check if entity has a component
        bool has(size_type entity_id) const {
            return entity_id < _sparse.size() && _sparse[entity_id] != npos;
        }

        // Get component for entity (nullptr if not present)
        reference_type get(size_type entity_id) {
        if (!has(entity_id)) throw std::out_of_range("sparse_set::get: entity has no component");
            return _packed[_sparse[entity_id]];
        }
        const_reference_type get(size_type entity_id) const {
        if (!has(entity_id)) throw std::out_of_range("sparse_set::get: entity has no component");
            return _packed[_sparse[entity_id]];
        }

        // Get entity ID for packed index
        size_type entity_at(size_type idx) const {
            return _packed_entities[idx];
        }

        static constexpr size_type npos = static_cast<size_type>(-1);

    private:
        void ensure_sparse_size(size_type entity_id) {
            if (entity_id >= _sparse.size())
                _sparse.resize(entity_id + 1, npos);
        }

        std::vector<Component, Allocator> _packed;
        std::vector<size_type> _packed_entities;
        std::vector<size_type> _sparse;
};