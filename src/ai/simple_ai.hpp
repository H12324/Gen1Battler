#pragma once
#include "../core/pokemon.hpp"

class SimpleAI {
public:
  SimpleAI() = default;
  int choose_move() const; // pick move index
};
