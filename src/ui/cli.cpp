#include "ui/cli.hpp"
#include "core/battle.hpp"
#include <iostream>

void run_cli(Battle &b) {
    std::cout << "CLI: launching simple demo.\n";
    b.run();
}
