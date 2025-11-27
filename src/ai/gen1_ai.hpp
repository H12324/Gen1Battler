#pragma once
#include "../core/move.hpp"
#include "ai_interface.hpp"
#include <vector>


// Gen 1 AI implementation with move scoring system
class Gen1AI : public BattleAI {
public:
  Gen1AI() = default;
  ~Gen1AI() override = default;

  int choose_move(const Pokemon &ai_pokemon,
                  const Pokemon &player_pokemon) const override;

private:
  // Move scoring data
  struct MoveScore {
    int move_index;
    int score;
  };

  // Step 1: Assign base scores to moves
  int get_base_score(const MoveData *move) const;

  // Step 2: Check if move is usable
  bool is_move_usable(const Move &move, const MoveData *move_data,
                      const Pokemon &ai_pokemon,
                      const Pokemon &player_pokemon) const;

  // Step 3: Apply special rules and modifiers
  int apply_special_rules(const MoveData *move, int base_score,
                          const Pokemon &ai_pokemon,
                          const Pokemon &player_pokemon) const;

  // Step 4: Weighted random selection
  int weighted_random_select(const std::vector<MoveScore> &scored_moves) const;

  // Helper: Calculate type effectiveness
  float calculate_effectiveness(PokeType move_type,
                                const Pokemon &defender) const;

  // Helper: Check if move is OHKO
  bool is_ohko_move(const MoveData *move) const;

  // Helper: Check if move is stat-boosting
  bool is_stat_boost_move(const MoveData *move) const;

  // Helper: Check if stat is already maxed
  bool is_stat_maxed(const Pokemon &pokemon, PokeStat stat) const;
};
