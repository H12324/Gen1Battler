#pragma once
#include <iostream>
#include <vector>

#include "pokemon.hpp"

class Battle {
public:
    Battle() : active1("Pikachu", 5), active2("Bulbasaur", 5) {
        team1.push_back(active1);
        team2.push_back(active2);
    }
    void run(); 
    bool over = false;

    Pokemon active1;
    Pokemon active2;
private:
    std::vector<Pokemon> team1;
    std::vector<Pokemon> team2;

    int turn = 0;
};
