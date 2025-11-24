#pragma once
#include <string>
#include <vector>

#include "../data/game_data.hpp"
#include "enums.hpp"
#include "move.hpp"
#include <array>

class Pokemon {
public:
  Pokemon(const std::string &species_name, int level);

  const std::string &name() const;
  int hp() const;
  int max_hp() const;
  int stat(PokeStat s) const;

  void take_damage(int dmg);
  void heal(int amount);

  // Getters
  int level() const;
  PokeType type1() const;
  PokeType type2() const;
  PokeStatus status() const;
  VolatileStatus volatile_status() const;
  int stat_stage(PokeStat stat) const;

  // Status and stat modification
  bool apply_status(PokeStatus new_status);
  void apply_volatile_status(VolatileStatus vstatus);
  void clear_volatile_status();
  void modify_stat_stage(PokeStat stat, int stages);
  void reset_stat_stages();
  int get_modified_stat(PokeStat stat) const;

  // Move management
  void add_move(const Move &move);
  const Move &get_move(int index) const;
  int move_count() const;

private:
  void calculate_stats();

  const SpeciesData *species_;
  std::string nickname_;
  int level_;

  // Stats
  std::array<int, 5> base_stats_; // HP, Atk, Def, Spd, Spe
  std::array<int, 5> ivs_;
  std::array<int, 5> evs_;
  std::array<int, 5> current_stats_;

  int current_hp_;

  // Battle specific
  std::array<int, 5> stat_stages_; // -6 to +6
  PokeStatus status_;
  VolatileStatus volatile_status_;
  int confusion_turns_;
  int sleep_turns_;
  int toxic_counter_; // For Toxic damage calculation

  // Moves
  std::array<Move, 4> moves_;
  int move_count_;
};
