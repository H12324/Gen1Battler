#include "data/loader.hpp"
#include "data/game_data.hpp"
#include <iostream>

// In a real scenario, we would parse JSON here.
// For this task, we will hardcode some data to verify the structure works.

void load_species(const std::string &path) {
    std::cout << "Loading species from: " << path << "\n";
    auto& gd = GameData::getInstance();
    
    // Pikachu
    gd.addSpecies("Pikachu", {
        "Pikachu", 35, 55, 40, 90, 50, PokeType::Electric, PokeType::None
    });
    
    // Bulbasaur
    gd.addSpecies("Bulbasaur", {
        "Bulbasaur", 45, 49, 49, 45, 65, PokeType::Grass, PokeType::Poison
    });
    
    // Charmander
    gd.addSpecies("Charmander", {
        "Charmander", 39, 52, 43, 65, 50, PokeType::Fire, PokeType::None
    });
    
    // Squirtle
    gd.addSpecies("Squirtle", {
        "Squirtle", 44, 48, 65, 43, 50, PokeType::Water, PokeType::None
    });
}

void load_moves(const std::string &path) {
    std::cout << "Loading moves from: " << path << "\n";
    auto& gd = GameData::getInstance();
    
    gd.addMove("Tackle", {
        "Tackle", PokeType::Normal, MoveCategory::Physical, 35, 95, 35
    });
    
    gd.addMove("ThunderShock", {
        "ThunderShock", PokeType::Electric, MoveCategory::Special, 40, 100, 30
    });
}

void load_type_chart(const std::string &path) {
    std::cout << "(stub) load_type_chart from: " << path << "\n";
}
