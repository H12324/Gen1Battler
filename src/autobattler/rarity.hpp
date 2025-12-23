#pragma once
#include <string>

// Rarity tiers for Pokemon in auto-battler mode
enum class Rarity {
  Common,   // 50% spawn rate, 2-3 gold
  Uncommon, // 30% spawn rate, 4-5 gold
  Rare,     // 15% spawn rate, 6-8 gold
  Epic,     // 4% spawn rate, 10-12 gold
  Legendary // 1% spawn rate, 15-20 gold
};

// Convert rarity to string for display/serialization
inline const char *rarity_to_string(Rarity rarity) {
  switch (rarity) {
  case Rarity::Common:
    return "common";
  case Rarity::Uncommon:
    return "uncommon";
  case Rarity::Rare:
    return "rare";
  case Rarity::Epic:
    return "epic";
  case Rarity::Legendary:
    return "legendary";
  default:
    return "common";
  }
}

// Convert string to rarity for deserialization
inline Rarity string_to_rarity(const std::string &str) {
  if (str == "uncommon")
    return Rarity::Uncommon;
  if (str == "rare")
    return Rarity::Rare;
  if (str == "epic")
    return Rarity::Epic;
  if (str == "legendary")
    return Rarity::Legendary;
  return Rarity::Common;
}

// Get base cost for a rarity tier
inline int get_base_cost(Rarity rarity) {
  switch (rarity) {
  case Rarity::Common:
    return 2;
  case Rarity::Uncommon:
    return 4;
  case Rarity::Rare:
    return 6;
  case Rarity::Epic:
    return 10;
  case Rarity::Legendary:
    return 15;
  default:
    return 2;
  }
}

// Get spawn weight for rarity (used in shop generation)
inline int get_spawn_weight(Rarity rarity) {
  switch (rarity) {
  case Rarity::Common:
    return 50;
  case Rarity::Uncommon:
    return 30;
  case Rarity::Rare:
    return 15;
  case Rarity::Epic:
    return 4;
  case Rarity::Legendary:
    return 1;
  default:
    return 50;
  }
}
