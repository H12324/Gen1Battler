#include <iostream>
#include "data/loader.hpp"
#include "data/game_data.hpp"

int main() {
    std::cout << "Testing Pokemon Data Loading\n";
    std::cout << "============================\n\n";
    
    load_species("src/data/species.json");
    load_type_chart("src/data/type_chart.json");
    
    // Test a few Pokemon
    const char* testPokemon[] = {"Bulbasaur", "Charizard", "Pikachu", "Mewtwo", "Mew", "Dragonite"};
    
    for (const char* name : testPokemon) {
        const SpeciesData* species = GameData::getInstance().getSpecies(name);
        if (species) {
            std::cout << name << ":\n";
            std::cout << "  HP: " << species->hp << "\n";
            std::cout << "  Attack: " << species->attack << "\n";
            std::cout << "  Defense: " << species->defense << "\n";
            std::cout << "  Speed: " << species->speed << "\n";
            std::cout << "  Special: " << species->special << "\n\n";
        } else {
            std::cout << name << ": NOT FOUND\n\n";
        }
    }
    
    return 0;
}
