#include "core/pokemon.hpp"

Pokemon::Pokemon(std::string name, int hp)
    : name_(std::move(name)), hp_(hp) {}

const std::string& Pokemon::name() const { return name_; }
int Pokemon::hp() const { return hp_; }
void Pokemon::take_damage(int dmg) { hp_ -= dmg; if (hp_ < 0) hp_ = 0; }
