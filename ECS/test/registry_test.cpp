#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/Registry.hpp"
#include <cstddef>

// Helpers sûrs : on ne suppose pas la valeur de départ des IDs.
// On compare seulement des relations (égalité / inégalité) et la réutilisation.

TEST_CASE("spawn_entity() attribue des IDs uniques tant qu'aucune entité n'est libérée")
{
    registry r;

    auto e1 = r.spawn_entity();
    auto e2 = r.spawn_entity();
    auto e3 = r.spawn_entity();

    CHECK(static_cast<std::size_t>(e1) != static_cast<std::size_t>(e2));
    CHECK(static_cast<std::size_t>(e2) != static_cast<std::size_t>(e3));
    CHECK(static_cast<std::size_t>(e1) != static_cast<std::size_t>(e3));
}

TEST_CASE("kill_entity() puis spawn_entity() réutilise l'ID libéré (LIFO)")
{
    registry r;

    auto e1 = r.spawn_entity();
    auto e2 = r.spawn_entity();
    auto e3 = r.spawn_entity();

    r.kill_entity(e2);
    r.kill_entity(e3);

    auto a = r.spawn_entity(); // doit réutiliser l'ID de e3
    auto b = r.spawn_entity(); // puis l'ID de e2

    CHECK(static_cast<std::size_t>(a) == static_cast<std::size_t>(e3));
    CHECK(static_cast<std::size_t>(b) == static_cast<std::size_t>(e2));

    // L'ID de e1 ne doit pas être réutilisé tant qu'on ne l'a pas tué
    CHECK(static_cast<std::size_t>(a) != static_cast<std::size_t>(e1));
    CHECK(static_cast<std::size_t>(b) != static_cast<std::size_t>(e1));
}

TEST_CASE("entity_from_index() construit une entité correspondant exactement à l'index donné")
{
    registry r;

    // On prend quelques valeurs arbitraires
    const std::size_t idx1 = 0;
    const std::size_t idx2 = 42;
    const std::size_t idx3 = 9999;

    auto e1 = r.entity_from_index(idx1);
    auto e2 = r.entity_from_index(idx2);
    auto e3 = r.entity_from_index(idx3);

    CHECK(static_cast<std::size_t>(e1) == idx1);
    CHECK(static_cast<std::size_t>(e2) == idx2);
    CHECK(static_cast<std::size_t>(e3) == idx3);
}
