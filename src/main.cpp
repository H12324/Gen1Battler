#include <iostream>
#include "core/battle.hpp"
#include "data/loader.hpp"
#include "ui/cli.hpp"

int main() {
    std::cout << "Pokemon Gen 1 Battler\n";
    
    load_species("data/species.json");
    load_moves("data/moves.json");
    load_type_chart("data/type_chart.json");

    Battle battle;
    
    // Give Pokemon some moves
    const MoveData* tackle = GameData::getInstance().getMove("Tackle");
    const MoveData* thundershock = GameData::getInstance().getMove("ThunderShock");
    
    if (tackle) {
        battle.active1.add_move(Move(tackle));
        battle.active2.add_move(Move(tackle));
    }
    if (thundershock) {
        battle.active1.add_move(Move(thundershock));
    }
    
    // Run the CLI battle
    run_cli(battle);
    
    return 0;
}
