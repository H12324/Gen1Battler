#include "gen1_ai.hpp"
#include "../core/rng.hpp"
#include "../data/game_data.hpp"
#include <algorithm>
#include <iostream>

int Gen1AI::choose_move(const Pokemon &ai_pokemon,
                        const Pokemon &player_pokemon) const {
  std::vector<MoveScore> scored_moves;

  // Score each move
  for (int i = 0; i < ai_pokemon.move_count(); i++) {
    const Move &move = ai_pokemon.get_move(i);
    const MoveData *move_data = move.data;

    if (!move_data)
      continue;

    // Step 2: Check if move is usable
    if (!is_move_usable(move, move_data, ai_pokemon, player_pokemon)) {
      continue;
    }

    // Step 1: Get base score
    int score = get_base_score(move_data);

    // Step 3: Apply special rules
    score = apply_special_rules(move_data, score, ai_pokemon, player_pokemon);

    scored_moves.push_back({i, score});
  }

  // If no valid moves, return first move
  if (scored_moves.empty()) {
    return 0;
  }

  // Step 4: Weighted random selection
  return weighted_random_select(scored_moves);
}

int Gen1AI::get_base_score(const MoveData *move) const {
  // High priority moves: Recover, stat-boosting moves
  if (move->category == MoveCategory::Status) {
    if (move->primary_effect.type == MoveEffectType::Heal) {
      return 3; // High priority - Recover
    }
    if (move->primary_effect.type == MoveEffectType::StatChange &&
        move->primary_effect.stat_change.target == EffectTarget::Self &&
        move->primary_effect.stat_change.stages > 0) {
      return 3; // High priority - stat boosting
    }
  }

  // Preferable moves: Strong attacks (power >= 80)
  if (move->power >= 80) {
    return 2;
  }

  // Bad moves: Splash, self-destruct moves
  if (move->primary_effect.type == MoveEffectType::Splash) {
    return -2;
  }
  if (move->primary_effect.type == MoveEffectType::SelfDestruct) {
    return -2; // Could check HP here, but keeping it simple
  }

  // Default score
  return 0;
}

bool Gen1AI::is_move_usable(const Move &move, const MoveData *move_data,
                            const Pokemon &ai_pokemon,
                            const Pokemon &player_pokemon) const {
  // No PP remaining
  if (!move.has_pp()) {
    return false;
  }

  // Check type immunity (Normal/Fighting vs Ghost)
  if (move_data->type == PokeType::Normal ||
      move_data->type == PokeType::Fighting) {
    if (player_pokemon.type1() == PokeType::Ghost ||
        player_pokemon.type2() == PokeType::Ghost) {
      return false; // Immune
    }
  }

  // Ghost moves vs Psychic (Gen 1 bug - Ghost is immune to Psychic but not
  // vice versa)
  if (move_data->type == PokeType::Ghost) {
    if (player_pokemon.type1() == PokeType::Psychic ||
        player_pokemon.type2() == PokeType::Psychic) {
      return false; // Gen 1 bug makes this not work
    }
  }

  // Status moves that would fail
  if (move_data->primary_effect.type == MoveEffectType::StatusInflict) {
    PokeStatus target_status = move_data->primary_effect.status_inflict.status;
    if (player_pokemon.status() != PokeStatus::None &&
        target_status != PokeStatus::None) {
      return false; // Already has a status
    }
  }

  // Counter - only works if hit by physical move this turn
  if (move_data->primary_effect.type == MoveEffectType::Counter) {
    // Simplified: AI won't use Counter (too complex to predict)
    return false;
  }

  // Mirror Move - only works if opponent used a move
  if (move_data->primary_effect.type == MoveEffectType::Mirror) {
    // Simplified: AI won't use Mirror Move
    return false;
  }

  return true;
}

int Gen1AI::apply_special_rules(const MoveData *move, int base_score,
                                const Pokemon &ai_pokemon,
                                const Pokemon &player_pokemon) const {
  int score = base_score;

  // OHKO moves: Only use if AI is faster
  if (is_ohko_move(move)) {
    if (ai_pokemon.stat(PokeStat::Speed) <=
        player_pokemon.stat(PokeStat::Speed)) {
      return -999; // Don't use OHKO if slower
    }
  }

  // Stat-boosting moves: Avoid if stat is already maxed
  if (is_stat_boost_move(move)) {
    PokeStat target_stat = move->primary_effect.stat_change.stat;
    if (is_stat_maxed(ai_pokemon, target_stat)) {
      score -= 3; // Heavily penalize using stat boost at max
    }
  }

  // Type effectiveness bonuses
  if (move->category != MoveCategory::Status) {
    float effectiveness = calculate_effectiveness(move->type, player_pokemon);

    if (effectiveness >= 2.0f) {
      score += 2; // Super effective
    } else if (effectiveness <= 0.5f && effectiveness > 0.0f) {
      score -= 1; // Not very effective
    }
    // Note: 0.0f effectiveness is already filtered in is_move_usable
  }

  return score;
}

int Gen1AI::weighted_random_select(
    const std::vector<MoveScore> &scored_moves) const {
  // Find the minimum score to normalize
  int min_score = scored_moves[0].score;
  for (const auto &ms : scored_moves) {
    if (ms.score < min_score) {
      min_score = ms.score;
    }
  }

  // Normalize scores to be non-negative and convert to weights
  // Score 0 = weight 1, score 1 = weight 2, etc.
  std::vector<int> weights;
  int total_weight = 0;

  for (const auto &ms : scored_moves) {
    int normalized_score = ms.score - min_score;
    int weight = normalized_score + 1; // Minimum weight of 1
    weights.push_back(weight);
    total_weight += weight;
  }

  // Weighted random selection
  int random_value = rng_int(0, total_weight - 1);
  int cumulative = 0;

  for (size_t i = 0; i < scored_moves.size(); i++) {
    cumulative += weights[i];
    if (random_value < cumulative) {
      return scored_moves[i].move_index;
    }
  }

  // Fallback (should never reach here)
  return scored_moves[0].move_index;
}

float Gen1AI::calculate_effectiveness(PokeType move_type,
                                      const Pokemon &defender) const {
  float type1_eff =
      GameData::getInstance().getEffectiveness(move_type, defender.type1());
  float type2_eff = 1.0f;

  if (defender.type2() != PokeType::None) {
    type2_eff =
        GameData::getInstance().getEffectiveness(move_type, defender.type2());
  }

  return type1_eff * type2_eff;
}

bool Gen1AI::is_ohko_move(const MoveData *move) const {
  return move->primary_effect.type == MoveEffectType::OHKO;
}

bool Gen1AI::is_stat_boost_move(const MoveData *move) const {
  return move->primary_effect.type == MoveEffectType::StatChange &&
         move->primary_effect.stat_change.target == EffectTarget::Self &&
         move->primary_effect.stat_change.stages > 0;
}

bool Gen1AI::is_stat_maxed(const Pokemon &pokemon, PokeStat stat) const {
  return pokemon.stat_stage(stat) >= 6;
}
