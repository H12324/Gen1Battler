#pragma once
#include "enums.hpp"
#include <string>

// Types of move effects
enum class MoveEffectType {
  None,
  Damage,        // Standard damage move
  StatChange,    // Modify stats (Swords Dance, Growl, etc.)
  StatusInflict, // Inflict status (Thunder Wave, Toxic, etc.)
  Heal,          // Recover HP
  Recoil,        // Recoil damage (Take Down, Double-Edge)
  Drain,         // Drain HP (Absorb, Mega Drain)
  MultiHit,      // Hit 2-5 times (Double Slap, Fury Attack)
  TwoHit,        // Always hit twice (Double Kick, Bonemerang)
  OHKO,          // One-hit KO (Guillotine, Horn Drill, Fissure)
  FixedDamage,   // Fixed damage (Seismic Toss, Night Shade, Dragon Rage, Sonic
                 // Boom)
  TwoTurn,       // Two-turn move (Fly, Dig, Solar Beam)
  Trapping,      // Trapping move (Bind, Wrap, Fire Spin)
  HighCritRatio, // High critical hit ratio
  Confusion,     // Inflict confusion
  Flinch,        // Cause flinch (only works if faster)
  Disable,       // Disable last used move
  LeechSeed,     // Leech Seed effect
  Substitute,    // Create substitute
  Rage,          // Rage effect (increases attack when hit)
  Mimic,         // Copy opponent's move
  Metronome,     // Use random move
  Mirror,        // Mirror Move
  Haze,          // Reset all stat changes
  Reflect,       // Reduce physical damage
  LightScreen,   // Reduce special damage
  Transform,     // Transform into opponent
  Splash,        // Does nothing
  Teleport,      // Flee from battle
  SelfDestruct,  // Self-destruct/Explosion
  DreamEater,    // Drain sleeping target
  Bide,          // Bide mechanic
  Counter,       // Counter physical damage
  PayDay,        // Scatter coins
  Conversion     // Change type to match move
};

// Target of the effect
enum class EffectTarget { Opponent, Self, Both, Field };

// Stat change data
struct StatChange {
  PokeStat stat;
  int stages; // -6 to +6
  EffectTarget target;
  int chance; // 0-100, 100 means always applies

  StatChange()
      : stat(PokeStat::Attack), stages(0), target(EffectTarget::Self),
        chance(100) {}
  StatChange(PokeStat s, int st, EffectTarget t = EffectTarget::Self,
             int ch = 100)
      : stat(s), stages(st), target(t), chance(ch) {}
};

// Status infliction data
struct StatusInfliction {
  PokeStatus status;
  int chance; // 0-100
  EffectTarget target;

  StatusInfliction()
      : status(PokeStatus::None), chance(100), target(EffectTarget::Opponent) {}
  StatusInfliction(PokeStatus s, int ch = 100,
                   EffectTarget t = EffectTarget::Opponent)
      : status(s), chance(ch), target(t) {}
};

// Volatile status infliction
struct VolatileInfliction {
  VolatileStatus status;
  int chance;
  int duration; // -1 for indefinite, otherwise number of turns

  VolatileInfliction()
      : status(VolatileStatus::None), chance(100), duration(-1) {}
  VolatileInfliction(VolatileStatus s, int ch = 100, int dur = -1)
      : status(s), chance(ch), duration(dur) {}
};

// Fixed damage data
struct FixedDamageData {
  enum class Type {
    Level,    // Damage = user's level (Seismic Toss, Night Shade)
    Constant, // Constant damage (Dragon Rage = 40, Sonic Boom = 20)
    HalfHP    // Half of target's current HP (Super Fang)
  };

  Type type;
  int value; // For Constant type

  FixedDamageData() : type(Type::Level), value(0) {}
  FixedDamageData(Type t, int v = 0) : type(t), value(v) {}
};

// Two-turn move data
struct TwoTurnData {
  bool invulnerable; // Is user invulnerable during charge turn (Fly, Dig)
  std::string charge_message;

  TwoTurnData() : invulnerable(false), charge_message("") {}
  TwoTurnData(bool inv, const std::string &msg)
      : invulnerable(inv), charge_message(msg) {}
};

// Main move effect structure
struct MoveEffect {
  MoveEffectType type;

  // Effect-specific data
  StatChange stat_change;
  StatusInfliction status_inflict;
  VolatileInfliction volatile_inflict;
  FixedDamageData fixed_damage;
  TwoTurnData two_turn;

  // Common effect parameters
  int recoil_percent; // Percentage of damage dealt (25 for Take Down, 33 for
                      // Double-Edge)
  int drain_percent;  // Percentage of damage healed (50 for Absorb/Mega Drain)
  int heal_percent;   // Percentage of max HP healed (50 for Recover/Softboiled)
  int min_hits;       // For multi-hit moves
  int max_hits;       // For multi-hit moves
  int flinch_chance;  // Chance to flinch (0-100)
  bool high_crit;     // High critical hit ratio

  MoveEffect()
      : type(MoveEffectType::Damage), recoil_percent(0), drain_percent(0),
        heal_percent(0), min_hits(1), max_hits(1), flinch_chance(0),
        high_crit(false) {}
};

// Secondary effect (chance-based additional effect)
struct SecondaryEffect {
  int chance; // 0-100
  MoveEffect effect;

  SecondaryEffect() : chance(0) {}
  SecondaryEffect(int ch, const MoveEffect &eff) : chance(ch), effect(eff) {}
};
