#pragma once
#include <string>

#include "enums.hpp"

enum class MoveCategory {
    Physical,
    Special,
    Status
};

struct MoveData {
    std::string name;
    PokeType type;
    MoveCategory category;
    int power;
    int accuracy;
    int max_pp;
};

struct Move {
    const MoveData* data;
    int current_pp;
    int pp_ups;

    Move() : data(nullptr), current_pp(0), pp_ups(0) {}
    Move(const MoveData* d) : data(d), current_pp(d ? d->max_pp : 0), pp_ups(0) {}
};
