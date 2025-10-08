#pragma once

#include <cstddef>
#include <type_traits>

class entity {
    public:
        // construction must be explicit (no implicit conversion from size_t)
        explicit entity(std::size_t id) noexcept : id_(id) {}

        // optionally allow default construction (identity 0). Remove if you want to force explicit init.
        entity() noexcept = default;

        // implicitly convertible to size_t
        operator std::size_t() const noexcept { return id_; }

        // explicit accessor
        std::size_t value() const noexcept { return id_; }

        // comparisons as member functions (no friend required)
        bool operator==(const entity& other) const noexcept { return id_ == other.id_; }
        bool operator!=(const entity& other) const noexcept { return !(*this == other); }
        bool operator<(const entity& other) const noexcept { return id_ < other.id_; }
        bool operator>(const entity& other) const noexcept { return other < *this; }
        bool operator<=(const entity& other) const noexcept { return !(*this > other); }
        bool operator>=(const entity& other) const noexcept { return !(*this < other); }

    private:
        std::size_t id_{0};
};