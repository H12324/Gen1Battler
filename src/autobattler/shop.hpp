#pragma once
#include "../core/pokemon.hpp"
#include "../data/game_data.hpp"
#include "player_state.hpp"
#include "rarity.hpp"
#include <random>
#include <vector>

// Represents a slot in the shop
struct ShopSlot {
  const SpeciesData *species;
  int cost;
  Rarity rarity;
  bool locked; // Locked slots persist through refresh

  ShopSlot()
      : species(nullptr), cost(0), rarity(Rarity::Common), locked(false) {}
};

// The shop where players buy Pokemon
class Shop {
private:
  std::vector<ShopSlot> slots_;
  int refresh_cost_;
  int tier_;
  std::mt19937 rng_;

  // Get number of shop slots based on tier
  int get_slot_count() const {
    return 3 + tier_; // Tier 1: 4 slots, Tier 2: 5 slots, etc.
  }

  // Select a random species based on rarity weights
  const SpeciesData *select_random_species(Rarity target_rarity) {
    std::vector<const SpeciesData *> candidates;

    // Get all species from GameData
    auto &game_data = GameData::getInstance();
    auto all_names = game_data.getAllSpeciesNames();

    // Iterate through all species and filter by rarity
    // For now, assign rarity based on base stats total
    for (const auto &name : all_names) {
      const SpeciesData *species = game_data.getSpecies(name);
      if (!species)
        continue;

      int stat_total = species->hp + species->attack + species->defense +
                       species->special + species->speed;

      Rarity species_rarity;
      if (stat_total >= 600)
        species_rarity = Rarity::Legendary;
      else if (stat_total >= 500)
        species_rarity = Rarity::Epic;
      else if (stat_total >= 450)
        species_rarity = Rarity::Rare;
      else if (stat_total >= 400)
        species_rarity = Rarity::Uncommon;
      else
        species_rarity = Rarity::Common;

      if (species_rarity == target_rarity) {
        candidates.push_back(species);
      }
    }

    if (candidates.empty())
      return nullptr;

    std::uniform_int_distribution<> dist(0, candidates.size() - 1);
    return candidates[dist(rng_)];
  }

  // Roll for a rarity based on weights
  Rarity roll_rarity() {
    std::uniform_int_distribution<> dist(1, 100);
    int roll = dist(rng_);

    if (roll <= 50)
      return Rarity::Common;
    if (roll <= 80)
      return Rarity::Uncommon;
    if (roll <= 95)
      return Rarity::Rare;
    if (roll <= 99)
      return Rarity::Epic;
    return Rarity::Legendary;
  }

public:
  Shop(int tier = 1)
      : refresh_cost_(1), tier_(tier), rng_(std::random_device{}()) {
    slots_.resize(get_slot_count());
    refresh();
  }

  // Refresh the shop (generate new Pokemon)
  void refresh() {
    for (auto &slot : slots_) {
      if (slot.locked)
        continue; // Skip locked slots

      Rarity rarity = roll_rarity();
      const SpeciesData *species = select_random_species(rarity);

      if (species) {
        slot.species = species;
        slot.rarity = rarity;
        slot.cost = get_base_cost(rarity);
        slot.locked = false;
      }
    }
  }

  // Purchase a Pokemon from a slot
  bool purchase(size_t slot_index, PlayerState &player) {
    if (slot_index >= slots_.size())
      return false;

    ShopSlot &slot = slots_[slot_index];
    if (!slot.species)
      return false;

    if (!player.spend_gold(slot.cost))
      return false;

    // Create Pokemon from species (using species name)
    Pokemon mon(slot.species->name, 50); // All Pokemon are level 50

    // Assign 4 random moves
    auto all_moves = GameData::getInstance().getAllMoveNames();
    std::uniform_int_distribution<> move_dist(0, all_moves.size() - 1);
    for (int j = 0; j < 4; j++) {
      std::string move_name = all_moves[move_dist(rng_)];
      const MoveData *move_data = GameData::getInstance().getMove(move_name);
      if (move_data) {
        mon.add_move(Move(move_data));
      }
    }

    // Try to add to team, otherwise add to bench
    if (!player.add_to_team(mon)) {
      player.add_to_bench(mon);
    }

    // Clear the slot
    slot.species = nullptr;
    slot.locked = false;

    return true;
  }

  // Sell a Pokemon for gold
  int sell_pokemon(const Pokemon &mon) {
    // Sell for half the base cost
    // TODO: Get rarity from Pokemon
    return 1; // Placeholder
  }

  // Lock/unlock a slot
  void toggle_lock(size_t slot_index) {
    if (slot_index < slots_.size()) {
      slots_[slot_index].locked = !slots_[slot_index].locked;
    }
  }

  // Refresh shop (costs gold)
  bool refresh_shop(PlayerState &player) {
    if (!player.spend_gold(refresh_cost_))
      return false;
    refresh();
    return true;
  }

  // Upgrade shop tier
  void set_tier(int tier) {
    tier_ = tier;
    int new_slot_count = get_slot_count();
    if (new_slot_count > static_cast<int>(slots_.size())) {
      slots_.resize(new_slot_count);
    }
    refresh();
  }

  // Getters
  const std::vector<ShopSlot> &slots() const { return slots_; }
  int refresh_cost() const { return refresh_cost_; }
  int tier() const { return tier_; }
};
