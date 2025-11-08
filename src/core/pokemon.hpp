#pragma once
#include <string>
#include <vector>

class Pokemon {
public:
    Pokemon(std::string name, int hp, int atk, int def, int spd, int spe, int level);
    const std::string& name() const;
    int hp() const;
    void take_damage(int dmg);
private:
    std::string name_;
    int hp_;
    int level;
	std::vector<int> stats; // HP, Attack, Defense, Speed, Special
};
