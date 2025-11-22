#include <iostream>
#include "core/battle.hpp"
#include "data/loader.hpp"

int main() {
    std::cout << "Pokemon Gen 1 Battler\n";
    
    load_species("data/species.json");
    load_moves("data/moves.json");
    load_type_chart("data/type_chart.json");

    Battle battle;
    
    std::cout << "Player 1: " << battle.active1.name() << " (Lvl " << battle.active1.level() << ")\n";
    std::cout << "HP: " << battle.active1.hp() << "/" << battle.active1.max_hp() << "\n";
    std::cout << "Atk: " << battle.active1.stat(PokeStat::Attack) << "\n";
    
    std::cout << "Player 2: " << battle.active2.name() << " (Lvl " << battle.active2.level() << ")\n";
    std::cout << "HP: " << battle.active2.hp() << "/" << battle.active2.max_hp() << "\n";
    
    battle.run();
    
    return 0;
}
