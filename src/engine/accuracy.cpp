#include "engine/accuracy.hpp"
#include "core/rng.hpp"

bool check_hit(int accuracy_percent) {
    if (accuracy_percent >= 100) return true;
    if (accuracy_percent <= 0) return false;
    return rng_int(1, 100) <= accuracy_percent;
}
