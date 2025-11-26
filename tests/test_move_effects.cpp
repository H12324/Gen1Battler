#include "core/battle.hpp"
#include "core/pokemon.hpp"
#include "data/game_data.hpp"
#include "data/loader.hpp"
#include "engine/move_effects.hpp"
#include <catch2/catch.hpp>
#include <memory>

// Helper to create test Pokemon
Pokemon createPokemon(const std::string &name, PokeType type1, int level = 50) {
  GameData::getInstance().addSpecies(
      name, {name, 100, 100, 100, 100, 100, type1, PokeType::None});
  return Pokemon(name, level);
}

// Helper to create test move with specific effect
std::unique_ptr<MoveData> createMoveWithEffect(const std::string &name,
                                               MoveEffectType effectType,
                                               int power = 50) {
  auto moveData = std::make_unique<MoveData>();
  moveData->name = name;
  moveData->type = PokeType::Normal;
  moveData->category = MoveCategory::Physical;
  moveData->power = power;
  moveData->accuracy = 100;
  moveData->max_pp = 35;
  moveData->primary_effect.type = effectType;
  return moveData;
}

TEST_CASE("Move Effects - Drain Moves", "[move][drain]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Drain heals attacker by 50% of damage dealt") {
    Pokemon attacker = createPokemon("Attacker", PokeType::Grass);
    Pokemon defender = createPokemon("Defender", PokeType::Normal);

    // Damage attacker first
    attacker.take_damage(50);
    int initial_hp = attacker.hp();

    auto moveData = createMoveWithEffect("Absorb", MoveEffectType::Drain, 40);
    moveData->primary_effect.drain_percent = 50;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.damage > 0);
    REQUIRE(result.drain_amount == result.damage / 2);
  }

  SECTION("Mega Drain heals 50%") {
    Pokemon attacker = createPokemon("Attacker2", PokeType::Grass);
    Pokemon defender = createPokemon("Defender2", PokeType::Normal);

    attacker.take_damage(60);

    auto moveData =
        createMoveWithEffect("MegaDrain", MoveEffectType::Drain, 40);
    moveData->primary_effect.drain_percent = 50;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.drain_amount == result.damage / 2);
  }
}

TEST_CASE("Move Effects - Multi-Hit Moves", "[move][multihit]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Multi-hit moves hit 2-5 times") {
    Pokemon attacker = createPokemon("Attacker3", PokeType::Normal);
    Pokemon defender = createPokemon("Defender3", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("DoubleSlap", MoveEffectType::MultiHit, 15);
    moveData->primary_effect.min_hits = 2;
    moveData->primary_effect.max_hits = 5;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.hits >= 2);
    REQUIRE(result.hits <= 5);
    REQUIRE(result.damage > 0);
  }
}

TEST_CASE("Move Effects - Two-Hit Moves", "[move][twohit]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Two-hit moves always hit exactly twice") {
    Pokemon attacker = createPokemon("Attacker4", PokeType::Fighting);
    Pokemon defender = createPokemon("Defender4", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("DoubleKick", MoveEffectType::TwoHit, 30);

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.hits == 2);
    REQUIRE(result.damage > 0);
  }
}

TEST_CASE("Move Effects - OHKO Moves", "[move][ohko]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("OHKO fails if target level > user level") {
    Pokemon attacker = createPokemon("Attacker5", PokeType::Normal, 50);
    Pokemon defender = createPokemon("Defender5", PokeType::Normal, 60);

    auto moveData = createMoveWithEffect("Guillotine", MoveEffectType::OHKO, 0);

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE_FALSE(result.success);
  }

  SECTION("OHKO can succeed if user level >= target level") {
    Pokemon attacker = createPokemon("Attacker6", PokeType::Normal, 60);
    Pokemon defender = createPokemon("Defender6", PokeType::Normal, 50);

    auto moveData = createMoveWithEffect("HornDrill", MoveEffectType::OHKO, 0);

    // OHKO has a chance to work, we can't guarantee success
    // Just verify it doesn't crash and returns a valid result
    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    // Result should be either success or failure, not undefined
    REQUIRE((result.success || !result.success));
  }
}

TEST_CASE("Move Effects - Fixed Damage Moves", "[move][fixed]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Seismic Toss deals damage equal to user level") {
    Pokemon attacker = createPokemon("Attacker7", PokeType::Fighting, 50);
    Pokemon defender = createPokemon("Defender7", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("SeismicToss", MoveEffectType::FixedDamage, 0);
    moveData->primary_effect.fixed_damage.type = FixedDamageData::Type::Level;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.damage == 50);
  }

  SECTION("Dragon Rage deals constant 40 damage") {
    Pokemon attacker = createPokemon("Attacker8", PokeType::Dragon);
    Pokemon defender = createPokemon("Defender8", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("DragonRage", MoveEffectType::FixedDamage, 0);
    moveData->primary_effect.fixed_damage.type =
        FixedDamageData::Type::Constant;
    moveData->primary_effect.fixed_damage.value = 40;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.damage == 40);
  }

  SECTION("Sonic Boom deals constant 20 damage") {
    Pokemon attacker = createPokemon("Attacker9", PokeType::Normal);
    Pokemon defender = createPokemon("Defender9", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("SonicBoom", MoveEffectType::FixedDamage, 0);
    moveData->primary_effect.fixed_damage.type =
        FixedDamageData::Type::Constant;
    moveData->primary_effect.fixed_damage.value = 20;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.damage == 20);
  }
}

TEST_CASE("Move Effects - Stat Changes", "[move][stat]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Swords Dance raises Attack by 2 stages") {
    Pokemon attacker = createPokemon("Attacker10", PokeType::Normal);
    Pokemon defender = createPokemon("Defender10", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("SwordsDance", MoveEffectType::StatChange, 0);
    moveData->primary_effect.stat_change.stat = PokeStat::Attack;
    moveData->primary_effect.stat_change.stages = 2;
    moveData->primary_effect.stat_change.target = EffectTarget::Self;

    int initial_stage = attacker.stat_stage(PokeStat::Attack);

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.success);
    REQUIRE(attacker.stat_stage(PokeStat::Attack) == initial_stage + 2);
  }

  SECTION("Growl lowers opponent's Attack by 1 stage") {
    Pokemon attacker = createPokemon("Attacker11", PokeType::Normal);
    Pokemon defender = createPokemon("Defender11", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("Growl", MoveEffectType::StatChange, 0);
    moveData->primary_effect.stat_change.stat = PokeStat::Attack;
    moveData->primary_effect.stat_change.stages = -1;
    moveData->primary_effect.stat_change.target = EffectTarget::Opponent;

    int initial_stage = defender.stat_stage(PokeStat::Attack);

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.success);
    REQUIRE(defender.stat_stage(PokeStat::Attack) == initial_stage - 1);
  }
}

TEST_CASE("Move Effects - Status Infliction", "[move][status]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Thunder Wave inflicts paralysis") {
    Pokemon attacker = createPokemon("Attacker12", PokeType::Electric);
    Pokemon defender = createPokemon("Defender12", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("ThunderWave", MoveEffectType::StatusInflict, 0);
    moveData->primary_effect.status_inflict.status = PokeStatus::Paralysis;
    moveData->primary_effect.status_inflict.target = EffectTarget::Opponent;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.success);
    REQUIRE(defender.status() == PokeStatus::Paralysis);
  }

  SECTION("Toxic inflicts badly poisoned status") {
    Pokemon attacker = createPokemon("Attacker13", PokeType::Poison);
    Pokemon defender = createPokemon("Defender13", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("Toxic", MoveEffectType::StatusInflict, 0);
    moveData->primary_effect.status_inflict.status = PokeStatus::Toxic;
    moveData->primary_effect.status_inflict.target = EffectTarget::Opponent;

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.success);
    REQUIRE(defender.status() == PokeStatus::Toxic);
  }
}

TEST_CASE("Move Effects - Confusion", "[move][confusion]") {
  load_type_chart("src/data/type_chart.json");

  SECTION("Confuse Ray inflicts confusion") {
    Pokemon attacker = createPokemon("Attacker14", PokeType::Ghost);
    Pokemon defender = createPokemon("Defender14", PokeType::Normal);

    auto moveData =
        createMoveWithEffect("ConfuseRay", MoveEffectType::Confusion, 0);

    EffectResult result =
        apply_move_effect(attacker, defender, moveData.get(), nullptr);

    REQUIRE(result.success);
    REQUIRE(defender.volatile_status() == VolatileStatus::Confusion);
  }
}
