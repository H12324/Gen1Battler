#include <iostream>
#include "core/battle.hpp"
#include "core/pokemon.hpp"
#include "ui/cli.hpp"

int main() {
    std::cout << "Gen1Battler skeleton app. Run tests with CMake/CTest.\n";
    Battle b;
    run_cli(b);
    return 0;
}
