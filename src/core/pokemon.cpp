#include "core/pokemon.hpp"

#include "core/pokemon.hpp"
#include <cmath>
#include <iostream>

Pokemon::Pokemon(const std::string& species_name, int level) 
    : level_(level), status_(PokeStatus::None) {
    
    species_ = GameData::getInstance().getSpecies(species_name);
    if (!species_) {
        // Fallback or error handling
        std::cerr << "Error: Species " << species_name << " not found!\n";
        // Create a dummy species to prevent crash
        static SpeciesData dummy = {"MissingNo", 33, 136, 0, 29, 6, PokeType::Normal, PokeType::Normal};
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
        (((base_hp + iv_hp) * 2 + std::sqrt(static_cast<double>(ev_hp)) / 4) * level_) / 100 + level_ + 10;

    // Helper lambda for other stats
    auto calc_stat = [&](int base, int iv, int ev) {
        return (((base + iv) * 2 + std::sqrt(static_cast<double>(ev)) / 4) * level_) / 100 + 5;
    };

    current_stats_[static_cast<int>(PokeStat::Attack)] = calc_stat(species_->attack, ivs_[static_cast<int>(PokeStat::Attack)], evs_[static_cast<int>(PokeStat::Attack)]);
    current_stats_[static_cast<int>(PokeStat::Defense)] = calc_stat(species_->defense, ivs_[static_cast<int>(PokeStat::Defense)], evs_[static_cast<int>(PokeStat::Defense)]);
    current_stats_[static_cast<int>(PokeStat::Speed)] = calc_stat(species_->speed, ivs_[static_cast<int>(PokeStat::Speed)], evs_[static_cast<int>(PokeStat::Speed)]);
    current_stats_[static_cast<int>(PokeStat::Special)] = calc_stat(species_->special, ivs_[static_cast<int>(PokeStat::Special)], evs_[static_cast<int>(PokeStat::Special)]);
}

const std::string& Pokemon::name() const { return nickname_; }
int Pokemon::hp() const { return current_hp_; }
int Pokemon::max_hp() const { return current_stats_[static_cast<int>(PokeStat::HP)]; }
int Pokemon::stat(PokeStat s) const { return current_stats_[static_cast<int>(s)]; }

int Pokemon::level() const { return level_; }
PokeType Pokemon::type1() const { return species_->type1; }
PokeType Pokemon::type2() const { return species_->type2; }
PokeStatus Pokemon::status() const { return status_; }

void Pokemon::take_damage(int dmg) { 
    current_hp_ -= dmg; 
    if (current_hp_ < 0) current_hp_ = 0; 
}
