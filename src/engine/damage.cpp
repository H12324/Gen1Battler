#include "damage.hpp"
#include "../core/rng.hpp"
#include "../data/game_data.hpp"
#include <cmath>
#include <iostream>

DamageResult calculate_damage(const Pokemon &attacker, const Pokemon &defender,
                              const Move &move) {
  DamageResult result = {0, false, 1.0f};

  if (!move.data)
    return result;
  if (move.data->category == MoveCategory::Status)
    return result;

  // 1. Critical Hit Check
  // Gen 1 Crit Rate: BaseSpeed / 512
  int threshold = attacker.stat(PokeStat::Speed) / 2;
  if (threshold > 255)
    threshold = 255;
  // High crit moves logic omitted for simplicity

  int roll = rng_int(0, 255);
  bool is_crit = roll < threshold;
  result.critical = is_crit;

  // 2. Level
  int level = attacker.level();
  int crit_factor = is_crit ? 2 : 1;

  // 3. Attack and Defense
  int atk = 0;
  int def = 0;

  if (move.data->category == MoveCategory::Physical) {
    atk = attacker.get_modified_stat(PokeStat::Attack);
    def = defender.get_modified_stat(PokeStat::Defense);
  } else {
    atk = attacker.get_modified_stat(PokeStat::Special);
    def = defender.get_modified_stat(PokeStat::Special);
  }

  // Gen 1 Crit ignores stat modifiers (not implemented yet, but good to note)
  // Also capping logic
  if (atk > 255 || def > 255) {
    atk /= 4;
    def /= 4;
  }
  if (def == 0)
    def = 1; // Prevent divide by zero (Gen 1 softlock prevention)

  // 4. Base Damage Calculation
  // ((2 * Level * Crit / 5 + 2) * Power * A / D) / 50 + 2
  // CRITICAL: Must do multiplication before division to avoid precision loss
  int damage =
      ((2 * level * crit_factor / 5 + 2) * move.data->power * atk / def) / 50 +
      2;

  // 5. STAB
  if (move.data->type == attacker.type1() ||
      move.data->type == attacker.type2()) {
    damage += damage / 2;
  }

  // 6. Type Effectiveness
  float type1_eff = GameData::getInstance().getEffectiveness(move.data->type,
                                                             defender.type1());
  float type2_eff = 1.0f;
  if (defender.type2() != PokeType::None) {
    type2_eff = GameData::getInstance().getEffectiveness(move.data->type,
                                                         defender.type2());
  }

  float total_eff = type1_eff * type2_eff;
  result.type_effectiveness = total_eff;

  damage = static_cast<int>(damage * total_eff);

  // 7. Random Factor
  // Random integer between 217 and 255
  if (damage > 1) { // If damage is 1, random is skipped (always 1)
    int random_val = rng_int(217, 255);
    damage = (damage * random_val) / 255;
  }

  result.damage = damage;
  return result;
}
