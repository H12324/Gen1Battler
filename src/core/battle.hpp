#pragma once
#include <iostream>
#include <vector>

#include "pokemon.hpp"

class Battle {
public:
  Battle(std::vector<Pokemon> t1, std::vector<Pokemon> t2)
      : team1(t1), team2(t2), active1(t1[0]), active2(t2[0]) {}

  // Execute a turn with both Pokemon's moves
  void execute_turn(int player_move_index, int ai_move_index);

  bool over = false;

  Pokemon active1;
  Pokemon active2;

private:
  void apply_move(Pokemon &attacker, Pokemon &defender, const Move &move);

  std::vector<Pokemon> team1;
  std::vector<Pokemon> team2;

  int turn = 0;
};
