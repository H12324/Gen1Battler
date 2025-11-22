#include <catch2/catch.hpp>
#include "core/battle.hpp"
#include "data/game_data.hpp"

TEST_CASE("battle runs") {
    // Setup minimal data for test
    GameData::getInstance().addSpecies("TestMon", {"TestMon", 100, 100, 100, 100, 100, PokeType::Normal, PokeType::None});
    
    Battle b;
    // Give them at least one move
    MoveData testMove = {"TestMove", PokeType::Normal, MoveCategory::Physical, 50, 100, 35};
    b.active1.add_move(Move(&testMove));
    b.active2.add_move(Move(&testMove));
    
    b.execute_turn(0, 0);
    SUCCEED("turn executed");
}
