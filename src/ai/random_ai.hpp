#pragma once
#include "ai_interface.hpp"

// Simple random AI that picks moves randomly
class RandomAI : public BattleAI {
public:
  RandomAI() = default;
  ~RandomAI() override = default;

  int choose_move(const Pokemon &ai_pokemon,
                  const Pokemon &player_pokemon) const override;
};
