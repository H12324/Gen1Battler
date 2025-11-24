#include "data/game_data.hpp"
#include "data/loader.hpp"
#include "engine/damage.hpp"
#include <catch2/catch.hpp>


TEST_CASE("damage calculation basic") {
  // Load data to ensure we have types
  load_type_chart("data/type_chart.json"); // Path relative to execution?
  // Actually, we can just manually set effectiveness if needed, or rely on
  // loader if paths work. For unit tests, it's better to be self-contained or
  // mock. Let's manually inject some data for the test.
  GameData::getInstance().addSpecies(
      "TestMon",
      {"TestMon", 100, 100, 100, 100, 100, PokeType::Normal, PokeType::None});
  GameData::getInstance().setTypeEffectiveness(PokeType::Normal,
                                               PokeType::Normal, 1.0f);
  GameData::getInstance().setTypeEffectiveness(PokeType::Fire, PokeType::Grass,
                                               2.0f);

  Pokemon attacker("TestMon", 50);
  Pokemon defender("TestMon", 50);

  MoveData moveData;
  moveData.name = "TestMove";
  moveData.type = PokeType::Normal;
  moveData.category = MoveCategory::Physical;
  moveData.power = 50;
  moveData.accuracy = 100;
  moveData.max_pp = 35;
  moveData.primary_effect.type = MoveEffectType::Damage;

  Move move(&moveData);

  SECTION("Normal effective move") {
    DamageResult res = calculate_damage(attacker, defender, move);
    REQUIRE(res.damage > 0);
    REQUIRE(res.type_effectiveness == 1.0f);
  }

  SECTION("Super effective move") {
    moveData.type = PokeType::Fire;
    // Hack defender to be Grass
    // Since we can't easily modify species of existing pokemon, let's make a
    // new one.
    GameData::getInstance().addSpecies(
        "GrassMon",
        {"GrassMon", 100, 100, 100, 100, 100, PokeType::Grass, PokeType::None});
    Pokemon grassDef("GrassMon", 50);

    DamageResult res = calculate_damage(attacker, grassDef, move);
    REQUIRE(res.type_effectiveness == 2.0f);
    REQUIRE(res.damage > 0);
  }
}
