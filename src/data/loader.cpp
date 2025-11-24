#include "data/loader.hpp"
#include "data/game_data.hpp"
#include "data/move_parser.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Helper function to parse type string to enum
PokeType parseType(const std::string &typeStr) {
  if (typeStr == "Normal")
    return PokeType::Normal;
  if (typeStr == "Fire")
    return PokeType::Fire;
  if (typeStr == "Water")
    return PokeType::Water;
  if (typeStr == "Grass")
    return PokeType::Grass;
  if (typeStr == "Electric")
    return PokeType::Electric;
  if (typeStr == "Ice")
    return PokeType::Ice;
  if (typeStr == "Fighting")
    return PokeType::Fighting;
  if (typeStr == "Poison")
    return PokeType::Poison;
  if (typeStr == "Ground")
    return PokeType::Ground;
  if (typeStr == "Flying")
    return PokeType::Flying;
  if (typeStr == "Psychic")
    return PokeType::Psychic;
  if (typeStr == "Bug")
    return PokeType::Bug;
  if (typeStr == "Rock")
    return PokeType::Rock;
  if (typeStr == "Ghost")
    return PokeType::Ghost;
  if (typeStr == "Dragon")
    return PokeType::Dragon;
  return PokeType::None;
}

// Helper to parse move category
MoveCategory parseCategory(const std::string &catStr) {
  if (catStr == "Physical")
    return MoveCategory::Physical;
  if (catStr == "Special")
    return MoveCategory::Special;
  return MoveCategory::Status;
}

void load_species(const std::string &path) {
  std::cout << "Loading species from: " << path << "\n";
  auto &gd = GameData::getInstance();

  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Warning: Could not open " << path
              << ", using fallback data\n";
    // Fallback to hardcoded data
    gd.addSpecies("Pikachu", {"Pikachu", 35, 55, 30, 90, 50, PokeType::Electric,
                              PokeType::None});
    gd.addSpecies("Bulbasaur", {"Bulbasaur", 45, 49, 49, 45, 65,
                                PokeType::Grass, PokeType::Poison});
    return;
  }

  try {
    json j;
    file >> j;

    int count = 0;
    for (auto &[name, data] : j.items()) {
      SpeciesData species;
      species.name = name;
      species.hp = data["hp"];
      species.attack = data["attack"];
      species.defense = data["defense"];
      species.speed = data["speed"];
      species.special = data["special"];
      species.type1 = parseType(data["type1"]);
      species.type2 = parseType(data["type2"]);

      gd.addSpecies(name, species);
      count++;
    }

    std::cout << "Loaded " << count << " species successfully\n";
  } catch (const std::exception &e) {
    std::cerr << "Error parsing JSON: " << e.what() << "\n";
    std::cerr << "Using fallback data\n";
    gd.addSpecies("Pikachu", {"Pikachu", 35, 55, 30, 90, 50, PokeType::Electric,
                              PokeType::None});
    gd.addSpecies("Bulbasaur", {"Bulbasaur", 45, 49, 49, 45, 65,
                                PokeType::Grass, PokeType::Poison});
  }

  file.close();
}

void load_moves(const std::string &path) {
  std::cout << "Loading moves from: " << path << "\n";
  auto &gd = GameData::getInstance();

  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Warning: Could not open " << path
              << ", using fallback data\n";
    // Fallback to hardcoded data
    auto tackle = std::make_unique<MoveData>();
    tackle->name = "Tackle";
    tackle->type = PokeType::Normal;
    tackle->category = MoveCategory::Physical;
    tackle->power = 35;
    tackle->accuracy = 95;
    tackle->max_pp = 35;
    tackle->primary_effect.type = MoveEffectType::Damage;
    gd.addMove("Tackle", std::move(tackle));

    auto thunder_shock = std::make_unique<MoveData>();
    thunder_shock->name = "Thunder Shock";
    thunder_shock->type = PokeType::Electric;
    thunder_shock->category = MoveCategory::Special;
    thunder_shock->power = 40;
    thunder_shock->accuracy = 100;
    thunder_shock->max_pp = 30;
    thunder_shock->primary_effect.type = MoveEffectType::Damage;
    gd.addMove("Thunder Shock", std::move(thunder_shock));
    return;
  }

  try {
    json j;
    file >> j;

    int count = 0;
    for (auto &[name, data] : j.items()) {
      auto move = std::make_unique<MoveData>();
      move->name = name;
      move->type = parseType(data["type"]);
      move->category = parseCategory(data["category"]);
      move->power = data["power"];
      move->accuracy = data["accuracy"];
      move->max_pp = data["pp"];

      // Parse primary effect if present
      if (data.contains("effect")) {
        parseMoveEffect(move->primary_effect, data["effect"]);
      } else {
        // Initialize default effect based on category
        if (move->category == MoveCategory::Status) {
          move->primary_effect.type = MoveEffectType::None;
        } else {
          move->primary_effect.type = MoveEffectType::Damage;
        }
      }

      // Parse secondary effect if present
      if (data.contains("secondary_effect")) {
        auto secondary = std::make_unique<SecondaryEffect>();
        secondary->chance = data["secondary_effect"].value("chance", 30);
        parseMoveEffect(secondary->effect, data["secondary_effect"]);
        move->secondary_effect = std::move(secondary);
      }

      gd.addMove(name, std::move(move));
      count++;
    }

    std::cout << "Loaded " << count << " moves successfully\n";
  } catch (const std::exception &e) {
    std::cerr << "Error parsing JSON: " << e.what() << "\n";
    std::cerr << "Using fallback data\n";

    auto tackle = std::make_unique<MoveData>();
    tackle->name = "Tackle";
    tackle->type = PokeType::Normal;
    tackle->category = MoveCategory::Physical;
    tackle->power = 35;
    tackle->accuracy = 95;
    tackle->max_pp = 35;
    tackle->primary_effect.type = MoveEffectType::Damage;
    gd.addMove("Tackle", std::move(tackle));

    auto thunder_shock = std::make_unique<MoveData>();
    thunder_shock->name = "Thunder Shock";
    thunder_shock->type = PokeType::Electric;
    thunder_shock->category = MoveCategory::Special;
    thunder_shock->power = 40;
    thunder_shock->accuracy = 100;
    thunder_shock->max_pp = 30;
    thunder_shock->primary_effect.type = MoveEffectType::Damage;
    gd.addMove("Thunder Shock", std::move(thunder_shock));
  }

  file.close();
}

void load_type_chart(const std::string &path) {
  std::cout << "Loading type chart (hardcoded Gen 1) from: " << path << "\n";
  auto &gd = GameData::getInstance();

// Helper macro to make it readable
#define SET_EFF(atk, def, val)                                                 \
  gd.setTypeEffectiveness(PokeType::atk, PokeType::def, val)

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
