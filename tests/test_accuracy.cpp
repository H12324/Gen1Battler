#include <catch2/catch.hpp>
#include "engine/accuracy.hpp"

TEST_CASE("accuracy edges") {
    REQUIRE(check_hit(100) == true);
    REQUIRE(check_hit(0) == false);
}
