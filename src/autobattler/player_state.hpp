#pragma once
#include "../core/pokemon.hpp"
#include "rarity.hpp"
#include <string>
#include <vector>

// Represents the player's state in an auto-battler run
class PlayerState {
private:
  std::string player_name_;
  int gold_;
  int round_;
  int hearts_; // Lives remaining (start with 5)
  int tier_;   // Shop tier (determines available Pokemon)
  int wins_;   // Total wins this run
  int losses_; // Total losses this run

  std::vector<Pokemon> team_;  // Active team (max size based on tier)
  std::vector<Pokemon> bench_; // Bench Pokemon

  int max_team_size_; // Increases with tier unlocks

public:
  PlayerState(const std::string &name = "Player")
      : player_name_(name), gold_(10),          // Start with 10 gold
        round_(1), hearts_(5),                  // Start with 5 hearts
        tier_(1),                               // Start at tier 1
        wins_(0), losses_(0), max_team_size_(3) // Start with 3 team slots
  {}

  // Gold management
  int gold() const { return gold_; }
  void add_gold(int amount) { gold_ += amount; }
  bool spend_gold(int amount) {
    if (gold_ >= amount) {
      gold_ -= amount;
      return true;
    }
    return false;
  }

  // Hearts/lives management
  int hearts() const { return hearts_; }
  void lose_heart() {
    if (hearts_ > 0)
      hearts_--;
  }
  void gain_heart() {
    if (hearts_ < 10)
      hearts_++; // Cap at 10 hearts
  }
  bool is_game_over() const { return hearts_ <= 0; }

  // Round management
  int round() const { return round_; }
  void next_round() { round_++; }

  // Tier management
  int tier() const { return tier_; }
  void unlock_tier() {
    tier_++;
    max_team_size_++; // Gain 1 team slot per tier
  }

  // Win/loss tracking
  int wins() const { return wins_; }
  int losses() const { return losses_; }
  void record_win() { wins_++; }
  void record_loss() { losses_++; }

  // Team management
  const std::vector<Pokemon> &team() const { return team_; }
  std::vector<Pokemon> &team() { return team_; }
  const std::vector<Pokemon> &bench() const { return bench_; }
  std::vector<Pokemon> &bench() { return bench_; }

  int max_team_size() const { return max_team_size_; }
  bool can_add_to_team() const {
    return team_.size() < static_cast<size_t>(max_team_size_);
  }

  bool add_to_team(const Pokemon &mon) {
    if (can_add_to_team()) {
      team_.push_back(mon);
      return true;
    }
    return false;
  }

  bool add_to_bench(const Pokemon &mon) {
    bench_.push_back(mon);
    return true;
  }

  bool remove_from_team(size_t index) {
    if (index < team_.size()) {
      team_.erase(team_.begin() + index);
      return true;
    }
    return false;
  }

  // Player info
  const std::string &name() const { return player_name_; }
};
