#include "data/game_data.hpp"
#include "data/loader.hpp"
#include "engine/damage.hpp"
#include <catch2/catch.hpp>

// Helper function to create a test Pokemon with specific types
Pokemon createTestPokemon(const std::string &name, PokeType type1,
                          PokeType type2 = PokeType::None, int level = 50) {
  GameData::getInstance().addSpecies(
      name, {name, 100, 100, 100, 100, 100, type1, type2});
  return Pokemon(name, level);
}

// Helper function to create a test move
Move createTestMove(const std::string &name, PokeType type, int power = 50) {
  auto moveData = std::make_unique<MoveData>();
  moveData->name = name;
  moveData->type = type;
  moveData->category = MoveCategory::Physical;
  moveData->power = power;
  moveData->accuracy = 100;
  moveData->max_pp = 35;
  moveData->primary_effect.type = MoveEffectType::Damage;

  GameData::getInstance().addMove(name, std::move(moveData));
  const MoveData *data = GameData::getInstance().getMove(name);
  return Move(data);
}

TEST_CASE("Type Effectiveness - Super Effective (2.0x)",
          "[type][effectiveness]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Fire vs Grass") {
    Pokemon attacker = createTestPokemon("FireMon", PokeType::Fire);
    Pokemon defender = createTestPokemon("GrassMon", PokeType::Grass);
    Move move = createTestMove("Ember", PokeType::Fire);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Fire vs Ice") {
    Pokemon attacker = createTestPokemon("FireMon2", PokeType::Fire);
    Pokemon defender = createTestPokemon("IceMon", PokeType::Ice);
    Move move = createTestMove("Ember2", PokeType::Fire);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Fire vs Bug") {
    Pokemon attacker = createTestPokemon("FireMon3", PokeType::Fire);
    Pokemon defender = createTestPokemon("BugMon", PokeType::Bug);
    Move move = createTestMove("Ember3", PokeType::Fire);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Water vs Fire") {
    Pokemon attacker = createTestPokemon("WaterMon", PokeType::Water);
    Pokemon defender = createTestPokemon("FireMon4", PokeType::Fire);
    Move move = createTestMove("WaterGun", PokeType::Water);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Water vs Ground") {
    Pokemon attacker = createTestPokemon("WaterMon2", PokeType::Water);
    Pokemon defender = createTestPokemon("GroundMon", PokeType::Ground);
    Move move = createTestMove("WaterGun2", PokeType::Water);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Water vs Rock") {
    Pokemon attacker = createTestPokemon("WaterMon3", PokeType::Water);
    Pokemon defender = createTestPokemon("RockMon", PokeType::Rock);
    Move move = createTestMove("WaterGun3", PokeType::Water);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Electric vs Water") {
    Pokemon attacker = createTestPokemon("ElectricMon", PokeType::Electric);
    Pokemon defender = createTestPokemon("WaterMon4", PokeType::Water);
    Move move = createTestMove("ThunderShock", PokeType::Electric);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Electric vs Flying") {
    Pokemon attacker = createTestPokemon("ElectricMon2", PokeType::Electric);
    Pokemon defender = createTestPokemon("FlyingMon", PokeType::Flying);
    Move move = createTestMove("ThunderShock2", PokeType::Electric);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Grass vs Water") {
    Pokemon attacker = createTestPokemon("GrassMon2", PokeType::Grass);
    Pokemon defender = createTestPokemon("WaterMon5", PokeType::Water);
    Move move = createTestMove("VineWhip", PokeType::Grass);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Ice vs Dragon") {
    Pokemon attacker = createTestPokemon("IceMon2", PokeType::Ice);
    Pokemon defender = createTestPokemon("DragonMon", PokeType::Dragon);
    Move move = createTestMove("IceBeam", PokeType::Ice);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Fighting vs Normal") {
    Pokemon attacker = createTestPokemon("FightingMon", PokeType::Fighting);
    Pokemon defender = createTestPokemon("NormalMon", PokeType::Normal);
    Move move = createTestMove("KarateChop", PokeType::Fighting);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Fighting vs Rock") {
    Pokemon attacker = createTestPokemon("FightingMon2", PokeType::Fighting);
    Pokemon defender = createTestPokemon("RockMon2", PokeType::Rock);
    Move move = createTestMove("KarateChop2", PokeType::Fighting);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Psychic vs Fighting") {
    Pokemon attacker = createTestPokemon("PsychicMon", PokeType::Psychic);
    Pokemon defender = createTestPokemon("FightingMon3", PokeType::Fighting);
    Move move = createTestMove("Confusion", PokeType::Psychic);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }

  SECTION("Psychic vs Poison") {
    Pokemon attacker = createTestPokemon("PsychicMon2", PokeType::Psychic);
    Pokemon defender = createTestPokemon("PoisonMon", PokeType::Poison);
    Move move = createTestMove("Confusion2", PokeType::Psychic);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 2.0f);
  }
}

TEST_CASE("Type Effectiveness - Not Very Effective (0.5x)",
          "[type][effectiveness]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Fire vs Fire") {
    Pokemon attacker = createTestPokemon("FireMon5", PokeType::Fire);
    Pokemon defender = createTestPokemon("FireMon6", PokeType::Fire);
    Move move = createTestMove("Ember4", PokeType::Fire);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }

  SECTION("Fire vs Water") {
    Pokemon attacker = createTestPokemon("FireMon7", PokeType::Fire);
    Pokemon defender = createTestPokemon("WaterMon6", PokeType::Water);
    Move move = createTestMove("Ember5", PokeType::Fire);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }

  SECTION("Fire vs Rock") {
    Pokemon attacker = createTestPokemon("FireMon8", PokeType::Fire);
    Pokemon defender = createTestPokemon("RockMon3", PokeType::Rock);
    Move move = createTestMove("Ember6", PokeType::Fire);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }

  SECTION("Water vs Grass") {
    Pokemon attacker = createTestPokemon("WaterMon7", PokeType::Water);
    Pokemon defender = createTestPokemon("GrassMon3", PokeType::Grass);
    Move move = createTestMove("WaterGun4", PokeType::Water);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }

  SECTION("Electric vs Grass") {
    Pokemon attacker = createTestPokemon("ElectricMon3", PokeType::Electric);
    Pokemon defender = createTestPokemon("GrassMon4", PokeType::Grass);
    Move move = createTestMove("ThunderShock3", PokeType::Electric);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }

  SECTION("Grass vs Flying") {
    Pokemon attacker = createTestPokemon("GrassMon5", PokeType::Grass);
    Pokemon defender = createTestPokemon("FlyingMon2", PokeType::Flying);
    Move move = createTestMove("VineWhip2", PokeType::Grass);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }

  SECTION("Fighting vs Psychic") {
    Pokemon attacker = createTestPokemon("FightingMon4", PokeType::Fighting);
    Pokemon defender = createTestPokemon("PsychicMon3", PokeType::Psychic);
    Move move = createTestMove("KarateChop3", PokeType::Fighting);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.5f);
  }
}

TEST_CASE("Type Effectiveness - No Effect (0.0x)", "[type][effectiveness]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Normal vs Ghost") {
    Pokemon attacker = createTestPokemon("NormalMon2", PokeType::Normal);
    Pokemon defender = createTestPokemon("GhostMon", PokeType::Ghost);
    Move move = createTestMove("Tackle", PokeType::Normal);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.0f);
  }

  SECTION("Electric vs Ground") {
    Pokemon attacker = createTestPokemon("ElectricMon4", PokeType::Electric);
    Pokemon defender = createTestPokemon("GroundMon2", PokeType::Ground);
    Move move = createTestMove("ThunderShock4", PokeType::Electric);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.0f);
  }

  SECTION("Fighting vs Ghost") {
    Pokemon attacker = createTestPokemon("FightingMon5", PokeType::Fighting);
    Pokemon defender = createTestPokemon("GhostMon2", PokeType::Ghost);
    Move move = createTestMove("KarateChop4", PokeType::Fighting);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.0f);
  }

  SECTION("Ground vs Flying") {
    Pokemon attacker = createTestPokemon("GroundMon3", PokeType::Ground);
    Pokemon defender = createTestPokemon("FlyingMon3", PokeType::Flying);
    Move move = createTestMove("Earthquake", PokeType::Ground);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.0f);
  }

  SECTION("Ghost vs Psychic (Gen 1 Bug)") {
    Pokemon attacker = createTestPokemon("GhostMon3", PokeType::Ghost);
    Pokemon defender = createTestPokemon("PsychicMon4", PokeType::Psychic);
    Move move = createTestMove("Lick", PokeType::Ghost);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.0f);
  }
}

TEST_CASE("Type Effectiveness - Dual Type Stacking", "[type][effectiveness]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("4x Effectiveness (Electric vs Water/Flying)") {
    Pokemon attacker = createTestPokemon("ElectricMon5", PokeType::Electric);
    Pokemon defender =
        createTestPokemon("WaterFlyingMon", PokeType::Water, PokeType::Flying);
    Move move = createTestMove("ThunderShock5", PokeType::Electric);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 4.0f);
  }

  SECTION("0.25x Effectiveness (Grass vs Fire/Flying)") {
    Pokemon attacker = createTestPokemon("GrassMon6", PokeType::Grass);
    Pokemon defender =
        createTestPokemon("FireFlyingMon", PokeType::Fire, PokeType::Flying);
    Move move = createTestMove("VineWhip3", PokeType::Grass);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.25f);
  }

  SECTION("0x Effectiveness with dual type (Normal vs Rock/Ghost)") {
    Pokemon attacker = createTestPokemon("NormalMon3", PokeType::Normal);
    Pokemon defender =
        createTestPokemon("RockGhostMon", PokeType::Rock, PokeType::Ghost);
    Move move = createTestMove("Tackle2", PokeType::Normal);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 0.0f);
  }

  SECTION("Ice vs Grass/Ground (4x)") {
    Pokemon attacker = createTestPokemon("IceMon3", PokeType::Ice);
    Pokemon defender =
        createTestPokemon("GrassGroundMon", PokeType::Grass, PokeType::Ground);
    Move move = createTestMove("IceBeam2", PokeType::Ice);

    DamageResult result = calculate_damage(attacker, defender, move);
    REQUIRE(result.type_effectiveness == 4.0f);
  }
}
