#include "engine/damage.hpp"

int calculate_damage(int power, int attack, int defense) {
    // Extremely simplified damage formula for skeleton project
    int base = std::max(1, power + attack - defense);
    return base;
}
