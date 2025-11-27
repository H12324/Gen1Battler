#include "cli.hpp"
#include "../ai/ai_interface.hpp"
#include "../core/battle.hpp"
#include "../core/rng.hpp"
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

void print_mon(const Pokemon &mon);

void run_cli(Battle &b, const BattleAI &ai) {
  int turn = 1;

  while (!b.over) {
    std::cout << "\n========== Turn " << turn++ << " ==========\n";

    // Display team status
    std::cout << "\nYour team: " << b.get_remaining_pokemon(1)
              << " Pokemon remaining\n";
    std::cout << "Opponent's team: " << b.get_remaining_pokemon(2)
              << " Pokemon remaining\n";

    // Display both Pokemon status
    std::cout << "\nYour Pokemon:\n";
    print_mon(b.active1);
    std::cout << "\nOpponent's Pokemon:\n";
    print_mon(b.active2);

    // Player chooses move
    std::cout << "\nYour moves:\n";
    for (int i = 0; i < b.active1.move_count(); i++) {
      const Move &move = b.active1.get_move(i);
      std::cout << (i + 1) << ". " << move.data->name
                << " (PP: " << move.current_pp << "/" << move.data->max_pp
                << ")\n";
    }

    int player_choice = 0;
    do {
      std::cout << "\nChoose a move (1-" << b.active1.move_count() << "): ";
      std::cin >> player_choice;

      // Clear error state if input fails
      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        player_choice = 0;
      }
    } while (player_choice < 1 || player_choice > b.active1.move_count());

    // AI chooses move using the AI system
    int ai_choice = ai.choose_move(b.active2, b.active1);

    std::cout << "\n";
    b.execute_turn(player_choice - 1, ai_choice);

    // Check if player's Pokemon fainted and handle switching
    if (b.active1.hp() <= 0 && !b.is_team_defeated(1)) {
      std::cout << "\nYour active Pokemon fainted! Choose your next Pokemon:\n";
      auto available = b.get_available_pokemon(1);

      for (size_t i = 0; i < available.size(); i++) {
        const Pokemon &pokemon = b.get_team_pokemon(1, available[i]);
        std::cout << (i + 1) << ". " << pokemon.name()
                  << " (HP: " << pokemon.hp() << "/" << pokemon.max_hp()
                  << ")\n";
      }

      int switch_choice = 0;
      do {
        std::cout << "\nChoose Pokemon (1-" << available.size() << "): ";
        std::cin >> switch_choice;

        if (std::cin.fail()) {
          std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          switch_choice = 0;
        }
      } while (switch_choice < 1 ||
               switch_choice > static_cast<int>(available.size()));

      b.switch_pokemon(1, available[switch_choice - 1]);
    }

    // Check if AI's Pokemon fainted and handle switching
    if (b.active2.hp() <= 0 && !b.is_team_defeated(2)) {
      // AI randomly chooses next Pokemon
      auto available = b.get_available_pokemon(2);
      if (!available.empty()) {
        int random_choice = rng_int(0, available.size() - 1);
        b.switch_pokemon(2, available[random_choice]);
      }
    }
  }

  // Display winner
  std::cout << "\n========== Battle Over ==========\n";
  if (b.is_team_defeated(2)) {
    std::cout << "You won! All opponent's Pokemon have fainted!\n";
  } else {
    std::cout << "You lost! All your Pokemon have fainted!\n";
  }
}

void print_mon(const Pokemon &mon) {
  cout << mon.name() << " - HP: " << mon.hp() << "/" << mon.max_hp() << "\n";
}