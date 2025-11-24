#include "move_parser.hpp"
#include "core/enums.hpp"

using json = nlohmann::json;

// Helper to parse PokeStat from string
PokeStat parseStat(const std::string &statStr) {
  if (statStr == "Attack")
    return PokeStat::Attack;
  if (statStr == "Defense")
    return PokeStat::Defense;
  if (statStr == "Speed")
    return PokeStat::Speed;
  if (statStr == "Special")
    return PokeStat::Special;
  return PokeStat::HP;
}

// Helper to parse PokeStatus from string
PokeStatus parseStatus(const std::string &statusStr) {
  if (statusStr == "Burn")
    return PokeStatus::Burn;
  if (statusStr == "Freeze")
    return PokeStatus::Freeze;
  if (statusStr == "Paralysis")
    return PokeStatus::Paralysis;
  if (statusStr == "Poison")
    return PokeStatus::Poison;
  if (statusStr == "Sleep")
    return PokeStatus::Sleep;
  if (statusStr == "Toxic")
    return PokeStatus::Toxic;
  return PokeStatus::None;
}

// Helper to parse effect target
EffectTarget parseEffectTarget(const std::string &targetStr) {
  if (targetStr == "self")
    return EffectTarget::Self;
  if (targetStr == "opponent")
    return EffectTarget::Opponent;
  if (targetStr == "both")
    return EffectTarget::Both;
  return EffectTarget::Opponent;
}

// Parse move effect from JSON
void parseMoveEffect(MoveEffect &effect, const json &effectData) {
  if (!effectData.contains("type"))
    return;

  std::string effectType = effectData["type"];

  if (effectType == "damage") {
    effect.type = MoveEffectType::Damage;
  } else if (effectType == "stat_change") {
    effect.type = MoveEffectType::StatChange;
    effect.stat_change.stat = parseStat(effectData["stat"]);
    effect.stat_change.stages = effectData["stages"];
    effect.stat_change.target =
        parseEffectTarget(effectData.value("target", "self"));
    effect.stat_change.chance = effectData.value("chance", 100);
  } else if (effectType == "status") {
    effect.type = MoveEffectType::StatusInflict;
    effect.status_inflict.status = parseStatus(effectData["status"]);
    effect.status_inflict.chance = effectData.value("chance", 100);
    effect.status_inflict.target =
        parseEffectTarget(effectData.value("target", "opponent"));
  } else if (effectType == "recoil") {
    effect.type = MoveEffectType::Recoil;
    effect.recoil_percent = effectData.value("percent", 25);
  } else if (effectType == "drain") {
    effect.type = MoveEffectType::Drain;
    effect.drain_percent = effectData.value("percent", 50);
  } else if (effectType == "multi_hit") {
    effect.type = MoveEffectType::MultiHit;
    effect.min_hits = effectData.value("min", 2);
    effect.max_hits = effectData.value("max", 5);
  } else if (effectType == "two_hit") {
    effect.type = MoveEffectType::TwoHit;
  } else if (effectType == "ohko") {
    effect.type = MoveEffectType::OHKO;
  } else if (effectType == "fixed_damage") {
    effect.type = MoveEffectType::FixedDamage;
    std::string damageType = effectData.value("damage_type", "level");
    if (damageType == "level") {
      effect.fixed_damage.type = FixedDamageData::Type::Level;
    } else if (damageType == "constant") {
      effect.fixed_damage.type = FixedDamageData::Type::Constant;
      effect.fixed_damage.value = effectData.value("value", 40);
    }
  } else if (effectType == "confusion") {
    effect.type = MoveEffectType::Confusion;
  } else if (effectType == "haze") {
    effect.type = MoveEffectType::Haze;
  } else if (effectType == "high_crit") {
    effect.type = MoveEffectType::HighCritRatio;
  }
}
