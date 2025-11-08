#include "core/pokemon.hpp"

Pokemon::Pokemon(std::string name, int hp, int atk, int def, int spd, int spe, int level) {
	name_ = std::move(name);
	hp_ = hp;
	level = level;
	stats.push_back(hp);
	stats.push_back(atk);
	stats.push_back(def);
	stats.push_back(spd);
	stats.push_back(spe);
}

const std::string& Pokemon::name() const { return name_; }
int Pokemon::hp() const { return hp_; }
void Pokemon::take_damage(int dmg) { hp_ -= dmg; if (hp_ < 0) hp_ = 0; }
