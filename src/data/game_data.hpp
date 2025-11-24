#pragma once
#include "../core/enums.hpp"
#include "../core/move.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


struct SpeciesData {
  std::string name;
  int hp;
  int attack;
  int defense;
  int speed;
  int special;
  PokeType type1;
  PokeType type2;
  // In a full game, we'd have learnsets, evolution data, etc.
};

class GameData {
public:
  static GameData &getInstance() {
    static GameData instance;
    return instance;
  }

  void addSpecies(const std::string &name, const SpeciesData &data) {
    species_map[name] = data;
  }

  const SpeciesData *getSpecies(const std::string &name) const {
    auto it = species_map.find(name);
    if (it != species_map.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void addMove(const std::string &name, std::unique_ptr<MoveData> data) {
    move_map[name] = std::move(data);
  }

  const MoveData *getMove(const std::string &name) const {
    auto it = move_map.find(name);
    if (it != move_map.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  void setTypeEffectiveness(PokeType attack, PokeType defend,
                            float effectiveness) {
    type_chart[attack][defend] = effectiveness;
  }

  float getEffectiveness(PokeType attack, PokeType defend) const {
    if (attack == PokeType::None || defend == PokeType::None)
      return 1.0f;
    if (type_chart.count(attack) && type_chart.at(attack).count(defend)) {
      return type_chart.at(attack).at(defend);
    }
    return 1.0f;
  }

  std::vector<std::string> getAllSpeciesNames() const {
    std::vector<std::string> names;
    for (const auto &pair : species_map) {
      names.push_back(pair.first);
    }
    return names;
  }

  std::vector<std::string> getAllMoveNames() const {
    std::vector<std::string> names;
    for (const auto &pair : move_map) {
      names.push_back(pair.first);
    }
    return names;
  }

private:
  GameData() {}
  std::unordered_map<std::string, SpeciesData> species_map;
  std::unordered_map<std::string, std::unique_ptr<MoveData>> move_map;
  std::unordered_map<PokeType, std::unordered_map<PokeType, float>> type_chart;
};
