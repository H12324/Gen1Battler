#pragma once

#include "core/move_effect.hpp"
#include <nlohmann/json.hpp>
#include <string>

// Helper functions to parse move effects from JSON
PokeStat parseStat(const std::string &statStr);
PokeStatus parseStatus(const std::string &statusStr);
EffectTarget parseEffectTarget(const std::string &targetStr);
void parseMoveEffect(MoveEffect &effect, const nlohmann::json &effectData);
