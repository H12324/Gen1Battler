#pragma once
#include "../core/pokemon.hpp"
#include <vector>

// Generate a random team of Pokemon
std::vector<Pokemon> generate_random_team(int team_size = 6, int level = 50);
