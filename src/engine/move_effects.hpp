#pragma once
#include "../core/move.hpp"
#include "../core/pokemon.hpp"
#include <string>

// Forward declaration
class Battle;

// Result of applying a move effect
struct EffectResult {
  bool success;
  int damage;
  int hits;
  int recoil_damage;
  int drain_amount;
  bool ohko;
  std::string message;

  EffectResult()
      : success(false), damage(0), hits(1), recoil_damage(0), drain_amount(0),
        ohko(false), message("") {}
};

// Apply a move's primary effect
EffectResult apply_move_effect(Pokemon &attacker, Pokemon &defender,
                               const MoveData *move_data, Battle *battle);

// Apply secondary effects (chance-based)
void apply_secondary_effect(Pokemon &attacker, Pokemon &defender,
                            const SecondaryEffect &effect);

// Specific effect handlers
bool apply_status_effect(Pokemon &target, PokeStatus status);
bool apply_volatile_effect(Pokemon &target, VolatileStatus vstatus);
void apply_stat_change(Pokemon &target, const StatChange &change);
int calculate_multi_hit_count(int min_hits, int max_hits);
int calculate_recoil_damage(int damage_dealt, int recoil_percent);
int calculate_drain_amount(int damage_dealt, int drain_percent);
bool check_ohko(const Pokemon &attacker, const Pokemon &defender);
int calculate_fixed_damage(const Pokemon &attacker,
                           const FixedDamageData &data);

// Status condition checks
bool can_move_with_status(Pokemon &pokemon, std::string &message);
void apply_end_of_turn_status_damage(Pokemon &pokemon);
