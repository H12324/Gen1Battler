#pragma once
#include <string>

class Pokemon {
public:
    Pokemon(std::string name, int hp);
    const std::string& name() const;
    int hp() const;
    void take_damage(int dmg);
private:
    std::string name_;
    int hp_;
};
