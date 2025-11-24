#include "core/battle.hpp"
#include "data/game_data.hpp"
#include <catch2/catch.hpp>

TEST_CASE("battle runs") {
  // Setup minimal data for test
  GameData::getInstance().addSpecies(
      "TestMon",
      {"TestMon", 100, 100, 100, 100, 100, PokeType::Normal, PokeType::None});

  // Create teams
  std::vector<Pokemon> team1;
  std::vector<Pokemon> team2;

  Pokemon p1("TestMon", 50);
  Pokemon p2("TestMon", 50);

  // Give them at least one move
  MoveData testMove;
  testMove.name = "TestMove";
  testMove.type = PokeType::Normal;
  testMove.category = MoveCategory::Physical;
  testMove.power = 50;
  testMove.accuracy = 100;
  testMove.max_pp = 35;
  testMove.primary_effect.type = MoveEffectType::Damage;

  p1.add_move(Move(&testMove));
  p2.add_move(Move(&testMove));

  team1.push_back(p1);
  team2.push_back(p2);

  Battle b(team1, team2);

  b.execute_turn(0, 0);
  SUCCEED("turn executed");
}
