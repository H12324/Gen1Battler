#include "data/loader.hpp"
#include "data/game_data.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Helper function to parse type string to enum
PokeType parseType(const std::string& typeStr) {
    if (typeStr == "Normal") return PokeType::Normal;
    if (typeStr == "Fire") return PokeType::Fire;
    if (typeStr == "Water") return PokeType::Water;
    if (typeStr == "Grass") return PokeType::Grass;
    if (typeStr == "Electric") return PokeType::Electric;
    if (typeStr == "Ice") return PokeType::Ice;
    if (typeStr == "Fighting") return PokeType::Fighting;
    if (typeStr == "Poison") return PokeType::Poison;
    if (typeStr == "Ground") return PokeType::Ground;
    if (typeStr == "Flying") return PokeType::Flying;
    if (typeStr == "Psychic") return PokeType::Psychic;
    if (typeStr == "Bug") return PokeType::Bug;
    if (typeStr == "Rock") return PokeType::Rock;
    if (typeStr == "Ghost") return PokeType::Ghost;
    if (typeStr == "Dragon") return PokeType::Dragon;
    return PokeType::None;
}

// Helper to extract value from JSON line
std::string extractValue(const std::string& line, const std::string& key) {
    size_t pos = line.find("\"" + key + "\"");
    if (pos == std::string::npos) return "";
    
    pos = line.find(":", pos);
    if (pos == std::string::npos) return "";
    
    pos = line.find_first_not_of(" \t:", pos + 1);
    if (pos == std::string::npos) return "";
    
    size_t end = line.find_first_of(",}", pos);
    std::string value = line.substr(pos, end - pos);
    
    // Remove quotes if present
    if (!value.empty() && value[0] == '"') {
        value = value.substr(1, value.length() - 2);
    }
    
    return value;
}

void load_species(const std::string &path) {
    std::cout << "Loading species from: " << path << "\n";
    auto& gd = GameData::getInstance();
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << path << ", using fallback data\n";
        // Fallback to hardcoded data
        gd.addSpecies("Pikachu", {"Pikachu", 35, 55, 30, 90, 50, PokeType::Electric, PokeType::None});
        gd.addSpecies("Bulbasaur", {"Bulbasaur", 45, 49, 49, 45, 65, PokeType::Grass, PokeType::Poison});
        return;
    }
    
    std::string line;
    std::string currentPokemon;
    std::string buffer;
    
    while (std::getline(file, line)) {
        // Skip empty lines and braces
        if (line.find('{') != std::string::npos && line.find('}') == std::string::npos) {
            continue;
        }
        if (line.find('}') != std::string::npos && line.find('{') == std::string::npos) {
            continue;
        }
        
        // Find Pokemon name
        size_t nameStart = line.find('"');
        if (nameStart != std::string::npos) {
            size_t nameEnd = line.find('"', nameStart + 1);
            if (nameEnd != std::string::npos) {
                currentPokemon = line.substr(nameStart + 1, nameEnd - nameStart - 1);
                
                // Check if this line has the full data
                if (line.find("hp") != std::string::npos) {
                    // Parse the stats from this line
                    int hp = std::stoi(extractValue(line, "hp"));
                    int attack = std::stoi(extractValue(line, "attack"));
                    int defense = std::stoi(extractValue(line, "defense"));
                    int speed = std::stoi(extractValue(line, "speed"));
                    int special = std::stoi(extractValue(line, "special"));
                    PokeType type1 = parseType(extractValue(line, "type1"));
                    PokeType type2 = parseType(extractValue(line, "type2"));
                    
                    gd.addSpecies(currentPokemon, {
                        currentPokemon, hp, attack, defense, speed, special, type1, type2
                    });
                }
            }
        }
    }
    
    file.close();
    std::cout << "Loaded species data successfully\n";
}


// Helper to parse move category
MoveCategory parseCategory(const std::string& catStr) {
    if (catStr == "Physical") return MoveCategory::Physical;
    if (catStr == "Special") return MoveCategory::Special;
    return MoveCategory::Status;
}

void load_moves(const std::string &path) {
    std::cout << "Loading moves from: " << path << "\n";
    auto& gd = GameData::getInstance();
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << path << ", using fallback data\n";
        // Fallback to hardcoded data
        gd.addMove("Tackle", {"Tackle", PokeType::Normal, MoveCategory::Physical, 35, 95, 35});
        gd.addMove("Thunder Shock", {"Thunder Shock", PokeType::Electric, MoveCategory::Special, 40, 100, 30});
        return;
    }
    
    std::string line;
    std::string currentMove;
    
    while (std::getline(file, line)) {
        // Skip empty lines and braces
        if (line.find('{') != std::string::npos && line.find('}') == std::string::npos) {
            continue;
        }
        if (line.find('}') != std::string::npos && line.find('{') == std::string::npos) {
            continue;
        }
        
        // Find move name
        size_t nameStart = line.find('"');
        if (nameStart != std::string::npos) {
            size_t nameEnd = line.find('"', nameStart + 1);
            if (nameEnd != std::string::npos) {
                currentMove = line.substr(nameStart + 1, nameEnd - nameStart - 1);
                
                // Check if this line has the full data
                if (line.find("type") != std::string::npos) {
                    // Parse the move data from this line
                    PokeType type = parseType(extractValue(line, "type"));
                    MoveCategory category = parseCategory(extractValue(line, "category"));
                    int power = std::stoi(extractValue(line, "power"));
                    int accuracy = std::stoi(extractValue(line, "accuracy"));
                    int pp = std::stoi(extractValue(line, "pp"));
                    
                    gd.addMove(currentMove, {
                        currentMove, type, category, power, accuracy, pp
                    });
                }
            }
        }
    }
    
    file.close();
    std::cout << "Loaded moves data successfully\n";
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
