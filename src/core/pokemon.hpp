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

  // Turn-specific data (reset each turn)
  struct TurnData {
    int damage_taken = 0;
    bool moved_first = false;
    const MoveData *last_move_hit_by = nullptr;
  };
  TurnData turn_data_;

  // Move locking (for Rage, Thrash, etc.)
  bool locked_into_move_ = false;
  const MoveData *locked_move_ = nullptr;
  int lock_turns_remaining_ = 0;

  // Disabled move tracking
  int disabled_move_index_ = -1;
  int disable_turns_remaining_ = 0;

  // Bide tracking
  bool bide_active_ = false;
  int bide_turns_remaining_ = 0;
  int bide_damage_stored_ = 0;

  // Reflect/Light Screen tracking
  bool reflect_active_ = false;
  int reflect_turns_remaining_ = 0;
  bool light_screen_active_ = false;
  int light_screen_turns_remaining_ = 0;

  // Moves
  std::array<Move, 4> moves_;
  int move_count_;

public:
  // Turn data access
  void reset_turn_data();
  void set_moved_first(bool first);
  void record_damage_taken(int damage, const MoveData *move);
  const TurnData &get_turn_data() const;

  // Move locking
  void lock_into_move(const MoveData *move, int turns);
  void unlock_move();
  bool is_locked() const;
  const MoveData *get_locked_move() const;

  // Disable
  void disable_move(int move_index, int turns);
  void update_disable();
  bool is_move_disabled(int move_index) const;

  // Bide
  void start_bide(int turns);
  void store_bide_damage(int damage);
  int release_bide();
  bool is_bide_active() const;
  void update_bide();

  // Reflect/Light Screen
  void activate_reflect(int turns);
  void activate_light_screen(int turns);
  bool has_reflect() const;
  bool has_light_screen() const;
  void update_screens();
};
