#include "battle.hpp"
#include "../engine/damage.hpp"
#include "../engine/move_effects.hpp"
#include <iostream>

void Battle::execute_turn(int player_move_index, int ai_move_index) {
  turn++;

  // Reset turn data
  active1.reset_turn_data();
  active2.reset_turn_data();

  // Determine turn order based on Speed
  bool player_first = active1.get_modified_stat(PokeStat::Speed) >=
                      active2.get_modified_stat(PokeStat::Speed);

  Pokemon *first = player_first ? &active1 : &active2;
  Pokemon *second = player_first ? &active2 : &active1;
  int first_move_idx = player_first ? player_move_index : ai_move_index;
  int second_move_idx = player_first ? ai_move_index : player_move_index;

  // Set move order flags
  first->set_moved_first(true);
  second->set_moved_first(false);

  // First Pokemon attacks
  if (first->hp() > 0) {
    execute_pokemon_move(*first, *second, first_move_idx);
  }

  // Second Pokemon attacks (if both alive)
  if (second->hp() > 0 && first->hp() > 0) {
    std::cout << "\n"; // Add spacing between Pokemon moves
    execute_pokemon_move(*second, *first, second_move_idx);
  }

  // End of turn updates
  active1.update_disable();
  active2.update_disable();
  active1.update_bide();
  active2.update_bide();
  active1.update_screens();
  active2.update_screens();

  // Apply end-of-turn status damage
  if (active1.hp() > 0) {
    apply_end_of_turn_status_damage(active1);
  }
  if (active2.hp() > 0) {
    apply_end_of_turn_status_damage(active2);
  }

  // Sync active Pokemon back to teams
  sync_active_to_team();

  // Check if battle is over
  if (is_team_defeated(1) || is_team_defeated(2)) {
    over = true;
  }
}

void Battle::execute_pokemon_move(Pokemon &attacker, Pokemon &defender,
                                  int move_index) {
  Move &move = const_cast<Move &>(attacker.get_move(move_index));

  // Check if move is disabled
  if (attacker.is_move_disabled(move_index)) {
    std::cout << attacker.name() << "'s move is disabled!\n";
    return;
  }

  // Check PP
  if (!move.has_pp()) {
    std::cout << attacker.name() << " has no PP left for " << move.data->name
              << "!\n";
    return;
  }

  // Check status conditions
  std::string status_message;
  if (!can_move_with_status(attacker, status_message)) {
    std::cout << status_message << "\n";
    return;
  }

  // Deduct PP
  move.deduct_pp();

  // Apply move with battle context
  apply_move(attacker, defender, move, this);
}

void Battle::apply_move(Pokemon &attacker, Pokemon &defender, const Move &move,
                        Battle *battle) {
  if (!move.data) {
    std::cout << attacker.name() << " has no move!\n";
    return;
  }

  std::cout << attacker.name() << " used " << move.data->name << "!\n";

  // Get the move effect type
  const MoveEffect &effect = move.data->primary_effect;

  // Handle based on effect type
  if (effect.type == MoveEffectType::Damage ||
      effect.type == MoveEffectType::None) {
    // Standard damage move
    DamageResult result = calculate_damage(attacker, defender, move);

    if (result.type_effectiveness == 0.0f) {
      std::cout << "It doesn't affect " << defender.name() << "...\n";
      return;
    }

    defender.take_damage(result.damage);
    std::cout << defender.name() << " took " << result.damage << " damage!\n";

    // Record damage for Counter mechanic
    defender.record_damage_taken(result.damage, move.data);

    // Store damage for Bide if active
    defender.store_bide_damage(result.damage);

    if (result.critical) {
      std::cout << "Critical hit!\n";
    }

    if (result.type_effectiveness > 1.0f) {
      std::cout << "It's super effective!\n";
    } else if (result.type_effectiveness < 1.0f &&
               result.type_effectiveness > 0.0f) {
      std::cout << "It's not very effective...\n";
    }
  } else if (effect.type == MoveEffectType::StatChange) {
    // Stat-changing move
    Pokemon &target =
        effect.stat_change.target == EffectTarget::Self ? attacker : defender;
    apply_stat_change(target, effect.stat_change);
  } else if (effect.type == MoveEffectType::StatusInflict) {
    // Status-inflicting move
    Pokemon &target = effect.status_inflict.target == EffectTarget::Self
                          ? attacker
                          : defender;
    apply_status_effect(target, effect.status_inflict.status);
  } else {
    // For other effects, use the effects engine
    EffectResult eff_result =
        apply_move_effect(attacker, defender, move.data, battle);

    // Apply damage from effect
    if (eff_result.damage > 0) {
      defender.take_damage(eff_result.damage);
      std::cout << defender.name() << " took " << eff_result.damage
                << " damage!\n";

      // Record damage for Counter mechanic
      defender.record_damage_taken(eff_result.damage, move.data);
    }

    // Apply recoil damage to attacker
    if (eff_result.recoil_damage > 0) {
      attacker.take_damage(eff_result.recoil_damage);
      std::cout << attacker.name() << " is hit with recoil!\n";
      if (attacker.hp() <= 0) {
        std::cout << attacker.name() << " fainted from recoil!\n";
      }
    }

    // Apply drain healing to attacker
    if (eff_result.drain_amount > 0) {
      attacker.heal(eff_result.drain_amount);
      std::cout << attacker.name() << " drained HP!\n";
    }

    if (!eff_result.message.empty()) {
      std::cout << eff_result.message << "\n";
    }
  }

  // Apply secondary effects if they exist
  if (move.data->secondary_effect) {
    apply_secondary_effect(attacker, defender, *move.data->secondary_effect);
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
