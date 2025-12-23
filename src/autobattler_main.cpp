#include "autobattler/auto_battle.hpp"
#include "autobattler/evolution.hpp"
#include "autobattler/player_state.hpp"
#include "autobattler/shop.hpp"
#include "data/loader.hpp"
#include "server/team_generator.hpp"
#include <iostream>
#include <string>

void display_team(const PlayerState &player) {
  std::cout << "\n=== YOUR TEAM ===\n";
  const auto &team = player.team();
  if (team.empty()) {
    std::cout << "(empty)\n";
  } else {
    for (size_t i = 0; i < team.size(); i++) {
      std::cout << (i + 1) << ". " << team[i].name() << " (Lv. "
                << team[i].level() << ") "
                << "HP: " << team[i].hp() << "/" << team[i].max_hp() << "\n";
    }
  }
  std::cout << "Slots: " << team.size() << "/" << player.max_team_size()
            << "\n";
}

void display_shop(const Shop &shop) {
  std::cout << "\n=== SHOP ===\n";
  const auto &slots = shop.slots();
  for (size_t i = 0; i < slots.size(); i++) {
    std::cout << (i + 1) << ". ";
    if (slots[i].species) {
      std::cout << slots[i].species->name << " ["
                << rarity_to_string(slots[i].rarity) << "] "
                << "- " << slots[i].cost << " gold";
      if (slots[i].locked)
        std::cout << " [LOCKED]";
    } else {
      std::cout << "(sold)";
    }
    std::cout << "\n";
  }
  std::cout << "\nRefresh cost: " << shop.refresh_cost() << " gold\n";
}

void shop_phase(PlayerState &player, Shop &shop) {
  while (true) {
    std::cout << "\n========================================\n";
    std::cout << "ROUND " << player.round() << " | TIER " << player.tier()
              << "\n";
    std::cout << "Gold: " << player.gold() << " | Hearts: " << player.hearts()
              << "/5\n";
    std::cout << "========================================\n";

    display_team(player);
    display_shop(shop);

    std::cout << "\nActions:\n";
    std::cout << "  1-" << shop.slots().size() << ": Buy Pokemon\n";
    std::cout << "  r: Refresh shop\n";
    std::cout << "  l [slot]: Lock/unlock slot\n";
    std::cout << "  s [team slot]: Sell Pokemon\n";
    std::cout << "  c [slot1] [slot2]: Combine Pokemon\n";
    std::cout << "  b: Battle!\n";
    std::cout << "  q: Quit\n";
    std::cout << "\nChoice: ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty())
      continue;

    char action = input[0];

    if (action == 'b') {
      break; // Start battle
    } else if (action == 'q') {
      std::cout << "Thanks for playing!\n";
      exit(0);
    } else if (action == 'r') {
      if (shop.refresh_shop(player)) {
        std::cout << "Shop refreshed!\n";
      } else {
        std::cout << "Not enough gold!\n";
      }
    } else if (action >= '1' && action <= '9') {
      int slot = action - '1';
      if (shop.purchase(slot, player)) {
        std::cout << "Purchased!\n";
      } else {
        std::cout << "Cannot purchase (not enough gold or invalid slot)\n";
      }
    } else if (action == 'l') {
      // Lock/unlock
      if (input.length() >= 3) {
        int slot = input[2] - '1';
        shop.toggle_lock(slot);
        std::cout << "Toggled lock on slot " << (slot + 1) << "\n";
      }
    } else if (action == 's') {
      // Sell Pokemon
      if (input.length() >= 3) {
        int slot = input[2] - '1';
        if (slot >= 0 && slot < static_cast<int>(player.team().size())) {
          player.add_gold(1); // Sell for 1 gold
          player.team().erase(player.team().begin() + slot);
          std::cout << "Sold for 1 gold!\n";
        }
      }
    } else if (action == 'c') {
      // Combine Pokemon
      if (input.length() >= 5) {
        int slot1 = input[2] - '1';
        int slot2 = input[4] - '1';

        if (slot1 >= 0 && slot1 < static_cast<int>(player.team().size()) &&
            slot2 >= 0 && slot2 < static_cast<int>(player.team().size()) &&
            slot1 != slot2) {

          const Pokemon &mon1 = player.team()[slot1];
          const Pokemon &mon2 = player.team()[slot2];

          if (EvolutionSystem::can_combine(mon1, mon2)) {
            Pokemon evolved = EvolutionSystem::combine_and_evolve(mon1, mon2);

            // Remove both Pokemon and add evolved one
            if (slot1 > slot2) {
              player.team().erase(player.team().begin() + slot1);
              player.team().erase(player.team().begin() + slot2);
            } else {
              player.team().erase(player.team().begin() + slot2);
              player.team().erase(player.team().begin() + slot1);
            }

            player.team().push_back(evolved);
            std::cout << "Evolved into " << evolved.name() << "!\n";
          } else {
            std::cout << "Cannot combine these Pokemon!\n";
          }
        }
      }
    }
  }
}

void battle_phase(PlayerState &player) {
  // Generate opponent team based on round
  int opponent_level = 50;
  int team_size = std::min(3 + player.round() / 3, 6);

  auto opponent_team = generate_random_team(team_size, opponent_level);

  std::cout << "\n=== BATTLE PHASE ===\n";
  std::cout << "Opponent has " << opponent_team.size() << " Pokemon!\n";

  AutoBattle auto_battle;
  BattleResult result = auto_battle.run(player.team(), opponent_team, true);

  // Handle result
  if (result == BattleResult::Win) {
    player.record_win();
    int gold_reward = 3 + player.round() / 2;
    player.add_gold(gold_reward);
    std::cout << "You won! Earned " << gold_reward << " gold.\n";

    // Unlock tier every 3 rounds
    if (player.round() % 3 == 0) {
      player.unlock_tier();
      std::cout << "TIER UP! Now at tier " << player.tier() << "\n";
      std::cout << "Max team size increased to " << player.max_team_size()
                << "\n";
    }
  } else if (result == BattleResult::Loss) {
    player.record_loss();
    player.lose_heart();
    std::cout << "You lost! Lost 1 heart.\n";
  } else {
    std::cout << "Draw! No rewards or penalties.\n";
  }

  player.next_round();
}

int main() {
  std::cout << "=== Pokemon Auto-Battler ===\n\n";

  // Load game data
  std::cout << "Loading game data...\n";
  load_species("src/data/species.json");
  load_moves("src/data/moves.json");
  load_type_chart("src/data/type_chart.json");
  std::cout << "Game data loaded!\n";

  // Get player name
  std::string name;
  std::cout << "\nEnter your name: ";
  std::getline(std::cin, name);
  if (name.empty())
    name = "Player";

  PlayerState player(name);
  Shop shop(player.tier());

  std::cout << "\nWelcome, " << name << "!\n";
  std::cout << "You start with " << player.gold() << " gold and "
            << player.hearts() << " hearts.\n";
  std::cout << "Lose all hearts and it's game over!\n";

  // Main game loop
  while (!player.is_game_over()) {
    shop.set_tier(player.tier());
    shop_phase(player, shop);

    if (player.team().empty()) {
      std::cout << "\nYou need at least 1 Pokemon to battle!\n";
      continue;
    }

    battle_phase(player);

    if (player.is_game_over()) {
      std::cout << "\n=== GAME OVER ===\n";
      std::cout << "You survived " << player.round() - 1 << " rounds!\n";
      std::cout << "Wins: " << player.wins() << " | Losses: " << player.losses()
                << "\n";
      break;
    }
  }

  return 0;
}
