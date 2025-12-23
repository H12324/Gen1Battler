#pragma once
#include "../ai/random_ai.hpp"
#include "../core/battle.hpp"
#include <iostream>


// Result of an auto-battle
enum class BattleResult { Win, Loss, Draw };

// Automated battle system for auto-battler mode
class AutoBattle {
private:
  // Select best move using simple AI logic
  int select_move(const Pokemon &attacker, const Pokemon &defender) {
    // Simple AI: pick a random valid move
    int move_count = attacker.move_count();
    if (move_count == 0)
      return 0;

    // Find a move with PP
    for (int i = 0; i < move_count; i++) {
      if (attacker.get_move(i).has_pp()) {
        return i;
      }
    }

    return 0; // Fallback
  }

public:
  // Run an automated battle between two teams
  BattleResult run(std::vector<Pokemon> team1, std::vector<Pokemon> team2,
                   bool verbose = true) {
    Battle battle(team1, team2);

    if (verbose) {
      std::cout << "\n=== AUTO-BATTLE START ===\n";
      std::cout << battle.active1.name() << " vs " << battle.active2.name()
                << "!\n\n";
    }

    int turn = 0;
    while (!battle.over &&
           turn < 100) { // Max 100 turns to prevent infinite loops
      turn++;

      if (verbose) {
        std::cout << "========== Turn " << turn << " ==========\n";
      }

      // Select moves for both Pokemon
      int move1 = select_move(battle.active1, battle.active2);
      int move2 = select_move(battle.active2, battle.active1);

      // Execute turn
      battle.execute_turn(move1, move2);

      if (verbose) {
        std::cout << battle.active1.name() << " HP: " << battle.active1.hp()
                  << "/" << battle.active1.max_hp() << "\n";
        std::cout << battle.active2.name() << " HP: " << battle.active2.hp()
                  << "/" << battle.active2.max_hp() << "\n\n";
      }
    }

    // Determine result
    bool team1_defeated = battle.is_team_defeated(1);
    bool team2_defeated = battle.is_team_defeated(2);

    if (team1_defeated && team2_defeated) {
      if (verbose)
        std::cout << "=== DRAW ===\n\n";
      return BattleResult::Draw;
    } else if (team2_defeated) {
      if (verbose)
        std::cout << "=== VICTORY ===\n\n";
      return BattleResult::Win;
    } else {
      if (verbose)
        std::cout << "=== DEFEAT ===\n\n";
      return BattleResult::Loss;
    }
  }
};
