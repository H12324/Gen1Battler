#include "random_ai.hpp"
#include "../core/rng.hpp"
#include <vector>

int RandomAI::choose_move(const Pokemon &ai_pokemon,
                          const Pokemon &player_pokemon) const {
  // Get all moves with PP remaining
  std::vector<int> valid_moves;
  for (int i = 0; i < ai_pokemon.move_count(); i++) {
    if (ai_pokemon.get_move(i).has_pp()) {
      valid_moves.push_back(i);
    }
  }

  // If no valid moves, return first move (Struggle would happen in real Gen 1)
  if (valid_moves.empty()) {
    return 0;
  }

  // Pick a random valid move
  int random_index = rng_int(0, valid_moves.size() - 1);
  return valid_moves[random_index];
}
