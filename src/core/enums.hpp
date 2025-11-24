#pragma once

enum class PokeType {
  None,
  Normal,
  Fire,
  Water,
  Grass,
  Electric,
  Ice,
  Fighting,
  Poison,
  Ground,
  Flying,
  Psychic,
  Bug,
  Rock,
  Ghost,
  Dragon
};

enum class PokeStatus {
  None,
  Burn,
  Freeze,
  Paralysis,
  Poison,
  Sleep,
  Toxic, // Badly poisoned (increasing damage)
  Fainted
};

// Volatile status conditions (reset when switched out)
enum class VolatileStatus {
  None,
  Confusion,
  Flinch,
  Trapped,     // Bind, Wrap, Fire Spin, Clamp
  Charging,    // Fly, Dig, Solar Beam, Skull Bash, Razor Wind, Sky Attack
  Recharging,  // Hyper Beam recharge turn
  Seeded,      // Leech Seed
  Disabled,    // Disable move
  Mist,        // Protected from stat drops
  FocusEnergy, // Focus Energy (bugged in Gen 1)
  Substitute   // Substitute active
};

enum class PokeStat { HP, Attack, Defense, Speed, Special };
