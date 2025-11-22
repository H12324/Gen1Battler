#pragma once
#include <string>
#include <vector>

#include "enums.hpp"
#include "move.hpp"
#include "data/game_data.hpp"
#include <array>

class Pokemon {
public:
    Pokemon(const std::string& species_name, int level);

    const std::string& name() const;
    int hp() const;
    int max_hp() const;
    int stat(PokeStat s) const;
    
    void take_damage(int dmg);
    
    // Getters
    int level() const;
    PokeType type1() const;
    PokeType type2() const;
    PokeStatus status() const;

private:
    void calculate_stats();

    const SpeciesData* species_;
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
    
    // Moves
    std::array<Move, 4> moves_;
};
