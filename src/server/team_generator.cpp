#include "team_generator.hpp"
#include "../data/game_data.hpp"
#include <algorithm>
#include <random>

std::vector<Pokemon> generate_random_team(int team_size, int level) {
  std::vector<Pokemon> team;

  // Get all available species and moves
  auto all_species = GameData::getInstance().getAllSpeciesNames();
  auto all_moves = GameData::getInstance().getAllMoveNames();

  // Setup random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> species_dist(0, all_species.size() - 1);
  std::uniform_int_distribution<> move_dist(0, all_moves.size() - 1);

  for (int i = 0; i < team_size; i++) {
    // Random species
    std::string species_name = all_species[species_dist(gen)];
    Pokemon pokemon(species_name, level);

    // Add 4 random moves
    for (int j = 0; j < 4; j++) {
      std::string move_name = all_moves[move_dist(gen)];
      const MoveData *move_data = GameData::getInstance().getMove(move_name);
      if (move_data) {
        pokemon.add_move(Move(move_data));
      }
    }

    team.push_back(pokemon);
  }

  return team;
}
