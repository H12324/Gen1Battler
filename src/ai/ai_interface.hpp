#pragma once
#include "../core/pokemon.hpp"

// Abstract base class for all AI implementations
class BattleAI {
public:
  virtual ~BattleAI() = default;

  // Choose a move for the AI's Pokemon
  // Returns the index of the move to use (0-3)
  // Parameters:
  //   ai_pokemon: The AI's active Pokemon
  //   player_pokemon: The player's active Pokemon
  virtual int choose_move(const Pokemon &ai_pokemon,
                          const Pokemon &player_pokemon) const = 0;
};
