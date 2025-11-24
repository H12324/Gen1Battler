#pragma once
#include <memory>
#include <string>


#include "enums.hpp"
#include "move_effect.hpp"

enum class MoveCategory { Physical, Special, Status };

struct MoveData {
  std::string name;
  PokeType type;
  MoveCategory category;
  int power;
  int accuracy;
  int max_pp;

  // Effect data
  MoveEffect primary_effect;
  std::unique_ptr<SecondaryEffect> secondary_effect;

  MoveData() : power(0), accuracy(100), max_pp(0), secondary_effect(nullptr) {}
};

struct Move {
  const MoveData *data;
  int current_pp;
  int pp_ups;

  Move() : data(nullptr), current_pp(0), pp_ups(0) {}
  Move(const MoveData *d) : data(d), current_pp(d ? d->max_pp : 0), pp_ups(0) {}
};
