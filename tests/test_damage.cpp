#include <catch2/catch.hpp>
#include "engine/damage.hpp"

TEST_CASE("damage basic") {
    REQUIRE(calculate_damage(40, 50, 30) >= 1);
}
