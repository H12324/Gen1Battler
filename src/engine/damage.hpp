#pragma once
#include <algorithm>

#include "../core/move.hpp"
#include "../core/pokemon.hpp"


struct DamageResult {
  int damage;
  bool critical;
  float type_effectiveness;
};

DamageResult calculate_damage(const Pokemon &attacker, const Pokemon &defender,
                              const Move &move);
