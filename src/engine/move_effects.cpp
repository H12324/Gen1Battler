#include "move_effects.hpp"
#include "../core/rng.hpp"
#include "damage.hpp"
#include <cmath>
#include <iostream>


EffectResult apply_move_effect(Pokemon &attacker, Pokemon &defender,
                               const MoveData *move_data) {
  EffectResult result;

  if (!move_data) {
    result.message = "No move data!";
    return result;
  }

  const MoveEffect &effect = move_data->primary_effect;

  switch (effect.type) {
  case MoveEffectType::Damage:
  case MoveEffectType::Recoil:
  case MoveEffectType::Drain:
  case MoveEffectType::HighCritRatio: {
    // Calculate damage normally
    Move temp_move(move_data);
    DamageResult dmg_result = calculate_damage(attacker, defender, temp_move);
    result.damage = dmg_result.damage;
    result.success = true;

    // Handle recoil
    if (effect.type == MoveEffectType::Recoil && result.damage > 0) {
      result.recoil_damage =
          calculate_recoil_damage(result.damage, effect.recoil_percent);
    }

    // Handle drain
    if (effect.type == MoveEffectType::Drain && result.damage > 0) {
      result.drain_amount =
          calculate_drain_amount(result.damage, effect.drain_percent);
    }
    break;
  }

  case MoveEffectType::MultiHit: {
    int num_hits = calculate_multi_hit_count(effect.min_hits, effect.max_hits);
    result.hits = num_hits;
    result.success = true;

    // Calculate damage for each hit
    Move temp_move(move_data);
    for (int i = 0; i < num_hits; i++) {
      DamageResult dmg_result = calculate_damage(attacker, defender, temp_move);
      result.damage += dmg_result.damage;
    }
    break;
  }

  case MoveEffectType::TwoHit: {
    result.hits = 2;
    result.success = true;

    Move temp_move(move_data);
    for (int i = 0; i < 2; i++) {
      DamageResult dmg_result = calculate_damage(attacker, defender, temp_move);
      result.damage += dmg_result.damage;
    }
    break;
  }

  case MoveEffectType::OHKO: {
    if (check_ohko(attacker, defender)) {
      result.damage = defender.hp();
      result.ohko = true;
      result.success = true;
      result.message = "It's a one-hit KO!";
    } else {
      result.success = false;
      result.message = "But it failed!";
    }
    break;
  }

  case MoveEffectType::FixedDamage: {
    result.damage = calculate_fixed_damage(attacker, effect.fixed_damage);
    result.success = true;
    break;
  }

  case MoveEffectType::StatChange: {
    apply_stat_change(
        effect.stat_change.target == EffectTarget::Self ? attacker : defender,
        effect.stat_change);
    result.success = true;
    break;
  }

  case MoveEffectType::StatusInflict: {
    Pokemon &target = effect.status_inflict.target == EffectTarget::Self
                          ? attacker
                          : defender;
    result.success = apply_status_effect(target, effect.status_inflict.status);
    break;
  }

  case MoveEffectType::Heal: {
    int heal_amount = (attacker.max_hp() * effect.heal_percent) / 100;
    // Note: Would need to add a heal method to Pokemon
    result.success = true;
    result.message = attacker.name() + " recovered HP!";
    break;
  }

  case MoveEffectType::Confusion: {
    result.success = apply_volatile_effect(defender, VolatileStatus::Confusion);
    if (result.success) {
      result.message = defender.name() + " became confused!";
    }
    break;
  }

  case MoveEffectType::Haze: {
    attacker.reset_stat_stages();
    defender.reset_stat_stages();
    result.success = true;
    result.message = "All stat changes were eliminated!";
    break;
  }

  default:
    result.message = "Effect not yet implemented!";
    break;
  }

  return result;
}

void apply_secondary_effect(Pokemon &attacker, Pokemon &defender,
                            const SecondaryEffect &effect) {
  // Check if secondary effect triggers
  int roll = rng_int(1, 100);
  if (roll <= effect.chance) {
    apply_move_effect(attacker, defender,
                      nullptr); // Would need to pass effect data differently
  }
}

bool apply_status_effect(Pokemon &target, PokeStatus status) {
  bool success = target.apply_status(status);

  if (success) {
    std::string status_name;
    switch (status) {
    case PokeStatus::Burn:
      status_name = "burned";
      break;
    case PokeStatus::Freeze:
      status_name = "frozen";
      break;
    case PokeStatus::Paralysis:
      status_name = "paralyzed";
      break;
    case PokeStatus::Poison:
      status_name = "poisoned";
      break;
    case PokeStatus::Sleep:
      status_name = "fell asleep";
      break;
    case PokeStatus::Toxic:
      status_name = "badly poisoned";
      break;
    default:
      status_name = "affected";
      break;
    }
    std::cout << target.name() << " was " << status_name << "!\n";
  } else {
    std::cout << "But it failed!\n";
  }

  return success;
}

bool apply_volatile_effect(Pokemon &target, VolatileStatus vstatus) {
  target.apply_volatile_status(vstatus);
  return true;
}

void apply_stat_change(Pokemon &target, const StatChange &change) {
  // Check chance
  int roll = rng_int(1, 100);
  if (roll > change.chance) {
    return;
  }

  target.modify_stat_stage(change.stat, change.stages);

  std::string stat_name;
  switch (change.stat) {
  case PokeStat::Attack:
    stat_name = "Attack";
    break;
  case PokeStat::Defense:
    stat_name = "Defense";
    break;
  case PokeStat::Speed:
    stat_name = "Speed";
    break;
  case PokeStat::Special:
    stat_name = "Special";
    break;
  default:
    stat_name = "stat";
    break;
  }

  if (change.stages > 0) {
    std::cout << target.name() << "'s " << stat_name;
    if (change.stages == 1)
      std::cout << " rose!\n";
    else
      std::cout << " rose sharply!\n";
  } else if (change.stages < 0) {
    std::cout << target.name() << "'s " << stat_name;
    if (change.stages == -1)
      std::cout << " fell!\n";
    else
      std::cout << " fell sharply!\n";
  }
}

int calculate_multi_hit_count(int min_hits, int max_hits) {
  // Gen 1 multi-hit distribution: 2-5 hits with specific probabilities
  // 2 hits: 37.5%, 3 hits: 37.5%, 4 hits: 12.5%, 5 hits: 12.5%
  int roll = rng_int(0, 7);
  if (roll < 3)
    return 2;
  if (roll < 6)
    return 3;
  if (roll < 7)
    return 4;
  return 5;
}

int calculate_recoil_damage(int damage_dealt, int recoil_percent) {
  return (damage_dealt * recoil_percent) / 100;
}

int calculate_drain_amount(int damage_dealt, int drain_percent) {
  return (damage_dealt * drain_percent) / 100;
}

bool check_ohko(const Pokemon &attacker, const Pokemon &defender) {
  // OHKO moves fail if target is higher level or same level
  if (defender.level() > attacker.level()) {
    return false;
  }

  // In Gen 1, OHKO accuracy = (user_level - target_level + 30)%
  int accuracy = attacker.level() - defender.level() + 30;
  if (accuracy > 100)
    accuracy = 100;

  int roll = rng_int(1, 100);
  return roll <= accuracy;
}

int calculate_fixed_damage(const Pokemon &attacker,
                           const FixedDamageData &data) {
  switch (data.type) {
  case FixedDamageData::Type::Level:
    return attacker.level();
  case FixedDamageData::Type::Constant:
    return data.value;
  case FixedDamageData::Type::HalfHP:
    // Would need target HP, not implemented here
    return 0;
  default:
    return 0;
  }
}

bool can_move_with_status(Pokemon &pokemon, std::string &message) {
  PokeStatus status = pokemon.status();

  switch (status) {
  case PokeStatus::Sleep:
    // Check if still asleep (would need sleep counter in Pokemon)
    message = pokemon.name() + " is fast asleep!";
    return false;

  case PokeStatus::Freeze:
    // 20% chance to thaw in Gen 1
    if (rng_int(1, 100) <= 20) {
      message = pokemon.name() + " thawed out!";
      pokemon.apply_status(PokeStatus::None);
      return true;
    }
    message = pokemon.name() + " is frozen solid!";
    return false;

  case PokeStatus::Paralysis:
    // 25% chance to be fully paralyzed
    if (rng_int(1, 100) <= 25) {
      message = pokemon.name() + " is fully paralyzed!";
      return false;
    }
    return true;

  default:
    return true;
  }
}

void apply_end_of_turn_status_damage(Pokemon &pokemon) {
  PokeStatus status = pokemon.status();

  switch (status) {
  case PokeStatus::Burn:
  case PokeStatus::Poison: {
    int damage = pokemon.max_hp() / 16;
    if (damage < 1)
      damage = 1;
    pokemon.take_damage(damage);
    std::cout << pokemon.name() << " is hurt by ";
    if (status == PokeStatus::Burn)
      std::cout << "its burn!\n";
    else
      std::cout << "poison!\n";
    break;
  }

  case PokeStatus::Toxic: {
    // Toxic damage increases each turn
    // Would need toxic_counter from Pokemon
    int damage = pokemon.max_hp() / 16;
    if (damage < 1)
      damage = 1;
    pokemon.take_damage(damage);
    std::cout << pokemon.name() << " is hurt by poison!\n";
    break;
  }

  default:
    break;
  }
}
