#include "ai/gen1_ai.hpp"
#include "core/battle.hpp"
#include "data/loader.hpp"
#include "ui/cli.hpp"
#include <algorithm>
#include <iostream>
#include <random>

int main() {
  std::cout << "Pokemon Gen 1 Battler\n";

  load_species("src/data/species.json");
  load_moves("src/data/moves.json");
  load_type_chart("src/data/type_chart.json");

  // Get all available species and moves
  auto all_species = GameData::getInstance().getAllSpeciesNames();
  auto all_moves = GameData::getInstance().getAllMoveNames();

  // Setup random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> species_dist(0, all_species.size() - 1);
  std::uniform_int_distribution<> move_dist(0, all_moves.size() - 1);

  // Create team 1 (6 random Pokemon)
  std::vector<Pokemon> team1;
  std::cout << "\n=== Team 1 ===\n";
  for (int i = 0; i < 6; i++) {
    std::string species_name = all_species[species_dist(gen)];
    Pokemon pokemon(species_name, 50);

    std::cout << (i + 1) << ". " << species_name << " (Lv. 50)\n   Moves: ";

    // Add 4 random moves
    for (int j = 0; j < 4; j++) {
      std::string move_name = all_moves[move_dist(gen)];
      const MoveData *move_data = GameData::getInstance().getMove(move_name);
      if (move_data) {
        pokemon.add_move(Move(move_data));
        std::cout << move_name;
        if (j < 3)
          std::cout << ", ";
      }
    }
    std::cout << "\n";

    team1.push_back(pokemon);
  }

  // Create team 2 (6 random Pokemon)
  std::vector<Pokemon> team2;
  std::cout << "\n=== Team 2 ===\n";
  for (int i = 0; i < 6; i++) {
    std::string species_name = all_species[species_dist(gen)];
    Pokemon pokemon(species_name, 50);

    std::cout << (i + 1) << ". " << species_name << " (Lv. 50)\n   Moves: ";

    // Add 4 random moves
    for (int j = 0; j < 4; j++) {
      std::string move_name = all_moves[move_dist(gen)];
      const MoveData *move_data = GameData::getInstance().getMove(move_name);
      if (move_data) {
        pokemon.add_move(Move(move_data));
        std::cout << move_name;
        if (j < 3)
          std::cout << ", ";
      }
    }
    std::cout << "\n";

    team2.push_back(pokemon);
  }

  // Create battle with the two teams
  Battle battle(team1, team2);

  std::cout << "\n=== Battle Start ===\n";
  std::cout << team1[0].name() << " vs " << team2[0].name() << "!\n\n";

  // Create Gen1 AI for the opponent
  Gen1AI opponent_ai;

  // Run the CLI battle with Gen1 AI
  run_cli(battle, opponent_ai);

  return 0;
}
