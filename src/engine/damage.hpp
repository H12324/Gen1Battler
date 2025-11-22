#pragma once
#include <algorithm>

#include "core/pokemon.hpp"
#include "core/move.hpp"

struct DamageResult {
    int damage;
    bool critical;
    float type_effectiveness;
};

DamageResult calculate_damage(const Pokemon& attacker, const Pokemon& defender, const Move& move);
