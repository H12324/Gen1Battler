#include "pokemon.hpp"
#include <cmath>
#include <iostream>

Pokemon::Pokemon(const std::string &species_name, int level)
    : level_(level), status_(PokeStatus::None),
      volatile_status_(VolatileStatus::None), confusion_turns_(0),
      sleep_turns_(0), toxic_counter_(0), move_count_(0) {

  species_ = GameData::getInstance().getSpecies(species_name);
  if (!species_) {
    // Fallback or error handling
    std::cerr << "Error: Species " << species_name << " not found!\n";
    // Create a dummy species to prevent crash
    static SpeciesData dummy = {
        "MissingNo", 33, 136, 0, 29, 6, PokeType::Normal, PokeType::Normal};
    species_ = &dummy;
  }

  nickname_ = species_->name;

  // Initialize IVs (random 0-15 in Gen 1, simplified here to max)
  ivs_.fill(15);
  // Initialize EVs (stat exp, 0-65535, simplified to 0)
  evs_.fill(0);

  // Initialize Stat Stages
  stat_stages_.fill(0);

  calculate_stats();
  current_hp_ = current_stats_[static_cast<int>(PokeStat::HP)];
}

void Pokemon::calculate_stats() {
  // Gen 1 Stat Formula:
  // HP: (((Base + IV) * 2 + sqrt(EV)/4) * Level) / 100 + Level + 10
  // Other: (((Base + IV) * 2 + sqrt(EV)/4) * Level) / 100 + 5

  int base_hp = species_->hp;
  int iv_hp = ivs_[static_cast<int>(PokeStat::HP)];
  int ev_hp = evs_[static_cast<int>(PokeStat::HP)];

  current_stats_[static_cast<int>(PokeStat::HP)] =
      (((base_hp + iv_hp) * 2 + std::sqrt(static_cast<double>(ev_hp)) / 4) *
       level_) /
          100 +
      level_ + 10;

  // Helper lambda for other stats
  auto calc_stat = [&](int base, int iv, int ev) {
    return (((base + iv) * 2 + std::sqrt(static_cast<double>(ev)) / 4) *
            level_) /
               100 +
           5;
  };

  current_stats_[static_cast<int>(PokeStat::Attack)] =
      calc_stat(species_->attack, ivs_[static_cast<int>(PokeStat::Attack)],
                evs_[static_cast<int>(PokeStat::Attack)]);
  current_stats_[static_cast<int>(PokeStat::Defense)] =
      calc_stat(species_->defense, ivs_[static_cast<int>(PokeStat::Defense)],
                evs_[static_cast<int>(PokeStat::Defense)]);
  current_stats_[static_cast<int>(PokeStat::Speed)] =
      calc_stat(species_->speed, ivs_[static_cast<int>(PokeStat::Speed)],
                evs_[static_cast<int>(PokeStat::Speed)]);
  current_stats_[static_cast<int>(PokeStat::Special)] =
      calc_stat(species_->special, ivs_[static_cast<int>(PokeStat::Special)],
                evs_[static_cast<int>(PokeStat::Special)]);
}

const std::string &Pokemon::name() const { return nickname_; }
int Pokemon::hp() const { return current_hp_; }
int Pokemon::max_hp() const {
  return current_stats_[static_cast<int>(PokeStat::HP)];
}
int Pokemon::stat(PokeStat s) const {
  return current_stats_[static_cast<int>(s)];
}

int Pokemon::get_modified_stat(PokeStat stat) const {
  if (stat == PokeStat::HP) {
    return current_stats_[static_cast<int>(stat)];
  }

  int base_stat = current_stats_[static_cast<int>(stat)];
  int stage = stat_stages_[static_cast<int>(stat)];

  // Gen 1 stat stage multipliers
  // Stages: -6 to +6
  // Multiplier = (2 + max(stage, 0)) / (2 + max(-stage, 0))
  double multiplier;
  if (stage >= 0) {
    multiplier = (2.0 + stage) / 2.0;
  } else {
    multiplier = 2.0 / (2.0 - stage);
  }

  int modified = static_cast<int>(base_stat * multiplier);

  // Apply burn attack reduction (only for physical moves, but we apply it to
  // Attack stat)
  if (stat == PokeStat::Attack && status_ == PokeStatus::Burn) {
    modified /= 2;
  }

  // Apply paralysis speed reduction
  if (stat == PokeStat::Speed && status_ == PokeStatus::Paralysis) {
    modified /= 4;
  }

  return modified > 0 ? modified : 1;
}

int Pokemon::level() const { return level_; }
PokeType Pokemon::type1() const { return species_->type1; }
PokeType Pokemon::type2() const { return species_->type2; }
PokeStatus Pokemon::status() const { return status_; }
VolatileStatus Pokemon::volatile_status() const { return volatile_status_; }
int Pokemon::stat_stage(PokeStat stat) const {
  return stat_stages_[static_cast<int>(stat)];
}

void Pokemon::add_move(const Move &move) {
  if (move_count_ < 4) {
    moves_[move_count_++] = move;
  }
}

const Move &Pokemon::get_move(int index) const { return moves_[index]; }

int Pokemon::move_count() const { return move_count_; }

void Pokemon::take_damage(int dmg) {
  current_hp_ -= dmg;
  if (current_hp_ < 0)
    current_hp_ = 0;
  if (current_hp_ == 0)
    status_ = PokeStatus::Fainted;
}

void Pokemon::heal(int amount) {
  current_hp_ += amount;
  if (current_hp_ > max_hp())
    current_hp_ = max_hp();
}

bool Pokemon::apply_status(PokeStatus new_status) {
  // Can't apply status if already has one (except None)
  if (status_ != PokeStatus::None && status_ != PokeStatus::Fainted) {
    return false;
  }

  // Can't apply status to fainted Pokemon
  if (status_ == PokeStatus::Fainted) {
    return false;
  }

  // Type immunities (Gen 1)
  if (new_status == PokeStatus::Poison || new_status == PokeStatus::Toxic) {
    // Poison-types are immune to poison
    if (species_->type1 == PokeType::Poison ||
        species_->type2 == PokeType::Poison) {
      return false;
    }
  }

  if (new_status == PokeStatus::Burn) {
    // Fire-types are immune to burn
    if (species_->type1 == PokeType::Fire ||
        species_->type2 == PokeType::Fire) {
      return false;
    }
  }

  status_ = new_status;

  // Initialize status-specific counters
  if (new_status == PokeStatus::Sleep) {
    sleep_turns_ = 1 + (rand() % 7); // 1-7 turns in Gen 1
  } else if (new_status == PokeStatus::Toxic) {
    toxic_counter_ = 1;
  }

  return true;
}

void Pokemon::apply_volatile_status(VolatileStatus vstatus) {
  volatile_status_ = vstatus;

  if (vstatus == VolatileStatus::Confusion) {
    confusion_turns_ = 2 + (rand() % 4); // 2-5 turns
  }
}

void Pokemon::clear_volatile_status() {
  volatile_status_ = VolatileStatus::None;
  confusion_turns_ = 0;
}

void Pokemon::modify_stat_stage(PokeStat stat, int stages) {
  if (stat == PokeStat::HP)
    return; // Can't modify HP stages

  int &current_stage = stat_stages_[static_cast<int>(stat)];
  current_stage += stages;

  // Clamp to -6 to +6
  if (current_stage > 6)
    current_stage = 6;
  if (current_stage < -6)
    current_stage = -6;
}

void Pokemon::reset_stat_stages() { stat_stages_.fill(0); }

// Turn data methods
void Pokemon::reset_turn_data() { turn_data_ = TurnData(); }

void Pokemon::set_moved_first(bool first) { turn_data_.moved_first = first; }

void Pokemon::record_damage_taken(int damage, const MoveData *move) {
  turn_data_.damage_taken += damage;
  turn_data_.last_move_hit_by = move;
}

const Pokemon::TurnData &Pokemon::get_turn_data() const { return turn_data_; }

// Move locking methods
void Pokemon::lock_into_move(const MoveData *move, int turns) {
  locked_into_move_ = true;
  locked_move_ = move;
  lock_turns_remaining_ = turns;
}

void Pokemon::unlock_move() {
  locked_into_move_ = false;
  locked_move_ = nullptr;
  lock_turns_remaining_ = 0;
}

bool Pokemon::is_locked() const {
  return locked_into_move_ && lock_turns_remaining_ > 0;
}

const MoveData *Pokemon::get_locked_move() const { return locked_move_; }

// Disable methods
void Pokemon::disable_move(int move_index, int turns) {
  disabled_move_index_ = move_index;
  disable_turns_remaining_ = turns;
}

void Pokemon::update_disable() {
  if (disable_turns_remaining_ > 0) {
    disable_turns_remaining_--;
    if (disable_turns_remaining_ == 0) {
      disabled_move_index_ = -1;
    }
  }
}

bool Pokemon::is_move_disabled(int move_index) const {
  return disabled_move_index_ == move_index && disable_turns_remaining_ > 0;
}
