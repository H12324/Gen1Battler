#pragma once
#include <iostream>
#include <vector>

#include "pokemon.hpp"

class Battle {
public:
  Battle(std::vector<Pokemon> t1, std::vector<Pokemon> t2)
      : team1(t1), team2(t2), active1(t1[0]), active2(t2[0]), active1_index(0),
        active2_index(0) {}

  // Execute a turn with both Pokemon's moves
  void execute_turn(int player_move_index, int ai_move_index);

  // Team management
  bool is_team_defeated(int team_num) const;
  int get_remaining_pokemon(int team_num) const;
  std::vector<int> get_available_pokemon(int team_num) const;
  void switch_pokemon(int team_num, int new_index);
  const Pokemon &get_team_pokemon(int team_num, int index) const;

  bool over = false;

  Pokemon active1;
  Pokemon active2;

private:
  void execute_pokemon_move(Pokemon &attacker, Pokemon &defender,
                            int move_index);
  void apply_move(Pokemon &attacker, Pokemon &defender, const Move &move,
                  Battle *battle);
  int get_next_available_pokemon(int team_num) const;
  void sync_active_to_team();

  std::vector<Pokemon> team1;
  std::vector<Pokemon> team2;

  int active1_index;
  int active2_index;
  int turn = 0;
};
