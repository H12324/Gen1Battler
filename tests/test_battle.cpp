#include <catch2/catch.hpp>
#include "core/battle.hpp"

TEST_CASE("battle runs") {
    Battle b;
    b.run();
    SUCCEED("run completed");
}
