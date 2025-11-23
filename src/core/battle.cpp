#include "battle.hpp"
#include "../engine/damage.hpp"
#include <iostream>

void Battle::execute_turn(int player_move_index, int ai_move_index) {
  turn++;

  // Determine turn order based on Speed
  bool player_first =
      active1.stat(PokeStat::Speed) >= active2.stat(PokeStat::Speed);

  Pokemon *first = player_first ? &active1 : &active2;
  Pokemon *second = player_first ? &active2 : &active1;
  int first_move_idx = player_first ? player_move_index : ai_move_index;
  int second_move_idx = player_first ? ai_move_index : player_move_index;

  // First Pokemon attacks
  if (first->hp() > 0) {
    const Move &move = first->get_move(first_move_idx);
    apply_move(*first, *second, move);
  }

  // Second Pokemon attacks (if still alive)
  if (second->hp() > 0 && first->hp() > 0) {
    const Move &move = second->get_move(second_move_idx);
    apply_move(*second, *first, move);
  }

  // Sync active Pokemon back to teams
  sync_active_to_team();

  // Check if battle is over (entire team defeated)
  if (is_team_defeated(1) || is_team_defeated(2)) {
    over = true;
  }
}

void Battle::apply_move(Pokemon &attacker, Pokemon &defender,
                        const Move &move) {
  if (!move.data) {
    std::cout << attacker.name() << " has no move!\n";
    return;
  }

  std::cout << attacker.name() << " used " << move.data->name << "!\n";

  DamageResult result = calculate_damage(attacker, defender, move);

  if (result.type_effectiveness == 0.0f) {
    std::cout << "It doesn't affect " << defender.name() << "...\n";
    return;
  }

  defender.take_damage(result.damage);

  std::cout << defender.name() << " took " << result.damage << " damage!\n";

  if (result.critical) {
    std::cout << "Critical hit!\n";
  }

  if (result.type_effectiveness > 1.0f) {
    std::cout << "It's super effective!\n";
  } else if (result.type_effectiveness < 1.0f &&
             result.type_effectiveness > 0.0f) {
    std::cout << "It's not very effective...\n";
  }

  if (defender.hp() <= 0) {
    std::cout << defender.name() << " fainted!\n";
  }
}

bool Battle::is_team_defeated(int team_num) const {
  const std::vector<Pokemon> &team = (team_num == 1) ? team1 : team2;

  for (const auto &pokemon : team) {
    if (pokemon.hp() > 0) {
      return false;
    }
  }
  return true;
}

int Battle::get_remaining_pokemon(int team_num) const {
  const std::vector<Pokemon> &team = (team_num == 1) ? team1 : team2;
  int count = 0;

  for (const auto &pokemon : team) {
    if (pokemon.hp() > 0) {
      count++;
    }
  }
  return count;
}

std::vector<int> Battle::get_available_pokemon(int team_num) const {
  const std::vector<Pokemon> &team = (team_num == 1) ? team1 : team2;
  int active_index = (team_num == 1) ? active1_index : active2_index;
  std::vector<int> available;

  for (size_t i = 0; i < team.size(); i++) {
    if (team[i].hp() > 0 && static_cast<int>(i) != active_index) {
      available.push_back(i);
    }
  }
  return available;
}

void Battle::switch_pokemon(int team_num, int new_index) {
  if (team_num == 1) {
    // Sync current active back to team
    team1[active1_index] = active1;
    // Switch to new Pokemon
    active1_index = new_index;
    active1 = team1[new_index];
    std::cout << "Go, " << active1.name() << "!\n";
  } else {
    // Sync current active back to team
    team2[active2_index] = active2;
    // Switch to new Pokemon
    active2_index = new_index;
    active2 = team2[new_index];
    std::cout << "Opponent sent out " << active2.name() << "!\n";
  }
}

int Battle::get_next_available_pokemon(int team_num) const {
  const std::vector<Pokemon> &team = (team_num == 1) ? team1 : team2;

  for (size_t i = 0; i < team.size(); i++) {
    if (team[i].hp() > 0) {
      return i;
    }
  }
  return -1; // No Pokemon available
}

void Battle::sync_active_to_team() {
  team1[active1_index] = active1;
  team2[active2_index] = active2;
}

const Pokemon &Battle::get_team_pokemon(int team_num, int index) const {
  const std::vector<Pokemon> &team = (team_num == 1) ? team1 : team2;
  return team[index];
}
