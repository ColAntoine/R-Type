/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/


#ifndef ZIPPER_HPP_
#define ZIPPER_HPP_


#include <tuple>
#include <cstddef>

// Helper to get reference type from sparse_set
template <typename Set>
using set_ref_t = typename Set::reference_type;

// Variadic zipper for N sparse_sets
template <typename FirstSet, typename... OtherSets>
class ZipperN {
    public:
        using entity_type = typename FirstSet::size_type;
        using value_type = std::tuple<set_ref_t<FirstSet>, set_ref_t<OtherSets>..., entity_type>;

        ZipperN(FirstSet &first, OtherSets&... others)
            : firstSet(first), otherSets(std::forward<OtherSets&>(others)...) {}

        class iterator {
            public:
                iterator(FirstSet &first, std::tuple<OtherSets&...> others, std::size_t idx)
                    : firstSet(first), otherSets(others), idx(idx) { advance(); }

                iterator& operator++() { ++idx; advance(); return *this; }
                bool operator!=(const iterator &other) const { return idx != other.idx; }
                value_type operator*() const {
                    auto entity = firstSet.entity_at(idx);
                    return deref(entity, std::index_sequence_for<OtherSets...>{});
                }
            private:
                void advance() {
                    while (idx < firstSet.size()) {
                        auto entity = firstSet.entity_at(idx);
                        if (has_all(entity, std::index_sequence_for<OtherSets...>{})) break;
                        ++idx;
                    }
                }

                template <std::size_t... Is>
                bool has_all(__attribute_maybe_unused__ entity_type entity, std::index_sequence<Is...>) const {
                    return (... && (std::get<Is>(otherSets).has(entity)));
                }

                template <std::size_t... Is>
                value_type deref(entity_type entity, std::index_sequence<Is...>) const {
                    return std::tuple<set_ref_t<FirstSet>, set_ref_t<OtherSets>..., entity_type>(
                        firstSet[idx],
                        std::get<Is>(otherSets).get(entity)...,
                        entity
                    );
                }

                FirstSet &firstSet;
                std::tuple<OtherSets&...> otherSets;
                std::size_t idx;
        };

        iterator begin() { return iterator(firstSet, otherSets, 0); }
        iterator end() { return iterator(firstSet, otherSets, firstSet.size()); }

    private:
        FirstSet &firstSet;
        std::tuple<OtherSets&...> otherSets;
};

template <typename FirstSet, typename... OtherSets>
ZipperN<FirstSet, OtherSets...> zipper(FirstSet &first, OtherSets&... others) {
    return ZipperN<FirstSet, OtherSets...>(first, others...);
}

#endif /* !ZIPPER_HPP_ */
