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
    std::cout << "Loading type chart (hardcoded Gen 1) from: " << path << "\n";
    auto& gd = GameData::getInstance();

    // Helper macro to make it readable
    #define SET_EFF(atk, def, val) gd.setTypeEffectiveness(PokeType::atk, PokeType::def, val)

    // Normal
    SET_EFF(Normal, Rock, 0.5f);
    SET_EFF(Normal, Ghost, 0.0f);

    // Fire
    SET_EFF(Fire, Fire, 0.5f);
    SET_EFF(Fire, Water, 0.5f);
    SET_EFF(Fire, Grass, 2.0f);
    SET_EFF(Fire, Ice, 2.0f);
    SET_EFF(Fire, Bug, 2.0f);
    SET_EFF(Fire, Rock, 0.5f);
    SET_EFF(Fire, Dragon, 0.5f);

    // Water
    SET_EFF(Water, Fire, 2.0f);
    SET_EFF(Water, Water, 0.5f);
    SET_EFF(Water, Grass, 0.5f);
    SET_EFF(Water, Ground, 2.0f);
    SET_EFF(Water, Rock, 2.0f);
    SET_EFF(Water, Dragon, 0.5f);

    // Electric
    SET_EFF(Electric, Water, 2.0f);
    SET_EFF(Electric, Electric, 0.5f);
    SET_EFF(Electric, Grass, 0.5f);
    SET_EFF(Electric, Ground, 0.0f);
    SET_EFF(Electric, Flying, 2.0f);
    SET_EFF(Electric, Dragon, 0.5f);

    // Grass
    SET_EFF(Grass, Fire, 0.5f);
    SET_EFF(Grass, Water, 2.0f);
    SET_EFF(Grass, Grass, 0.5f);
    SET_EFF(Grass, Poison, 0.5f);
    SET_EFF(Grass, Ground, 2.0f);
    SET_EFF(Grass, Flying, 0.5f);
    SET_EFF(Grass, Bug, 0.5f);
    SET_EFF(Grass, Rock, 2.0f);
    SET_EFF(Grass, Dragon, 0.5f);

    // Ice
    SET_EFF(Ice, Water, 0.5f);
    SET_EFF(Ice, Grass, 2.0f);
    SET_EFF(Ice, Ice, 0.5f);
    SET_EFF(Ice, Ground, 2.0f);
    SET_EFF(Ice, Flying, 2.0f);
    SET_EFF(Ice, Dragon, 2.0f);

    // Fighting
    SET_EFF(Fighting, Normal, 2.0f);
    SET_EFF(Fighting, Ice, 2.0f);
    SET_EFF(Fighting, Poison, 0.5f);
    SET_EFF(Fighting, Flying, 0.5f);
    SET_EFF(Fighting, Psychic, 0.5f);
    SET_EFF(Fighting, Bug, 0.5f);
    SET_EFF(Fighting, Rock, 2.0f);
    SET_EFF(Fighting, Ghost, 0.0f);

    // Poison
    SET_EFF(Poison, Grass, 2.0f);
    SET_EFF(Poison, Poison, 0.5f);
    SET_EFF(Poison, Ground, 0.5f);
    SET_EFF(Poison, Bug, 2.0f); // Gen 1 bug: Poison SE against Bug
    SET_EFF(Poison, Rock, 0.5f);
    SET_EFF(Poison, Ghost, 0.5f);

    // Ground
    SET_EFF(Ground, Fire, 2.0f);
    SET_EFF(Ground, Electric, 2.0f);
    SET_EFF(Ground, Grass, 0.5f);
    SET_EFF(Ground, Poison, 2.0f);
    SET_EFF(Ground, Flying, 0.0f);
    SET_EFF(Ground, Bug, 0.5f);
    SET_EFF(Ground, Rock, 2.0f);

    // Flying
    SET_EFF(Flying, Electric, 0.5f);
    SET_EFF(Flying, Grass, 2.0f);
    SET_EFF(Flying, Fighting, 2.0f);
    SET_EFF(Flying, Bug, 2.0f);
    SET_EFF(Flying, Rock, 0.5f);

    // Psychic
    SET_EFF(Psychic, Fighting, 2.0f);
    SET_EFF(Psychic, Poison, 2.0f);
    SET_EFF(Psychic, Psychic, 0.5f);

    // Bug
    SET_EFF(Bug, Fire, 0.5f);
    SET_EFF(Bug, Grass, 2.0f);
    SET_EFF(Bug, Fighting, 0.5f);
    SET_EFF(Bug, Poison, 2.0f); // Gen 1: Bug SE against Poison
    SET_EFF(Bug, Flying, 0.5f);
    SET_EFF(Bug, Psychic, 2.0f);
    SET_EFF(Bug, Ghost, 0.5f);

    // Rock
    SET_EFF(Rock, Fire, 2.0f);
    SET_EFF(Rock, Ice, 2.0f);
    SET_EFF(Rock, Fighting, 0.5f);
    SET_EFF(Rock, Ground, 0.5f);
    SET_EFF(Rock, Flying, 2.0f);
    SET_EFF(Rock, Bug, 2.0f);

    // Ghost
    SET_EFF(Ghost, Normal, 0.0f);
    SET_EFF(Ghost, Psychic, 0.0f); // Gen 1 bug: Ghost ineffective against Psychic
    SET_EFF(Ghost, Ghost, 2.0f);

    // Dragon
    SET_EFF(Dragon, Dragon, 2.0f);
}
