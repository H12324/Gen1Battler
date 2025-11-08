#include "ui/cli.hpp"
#include "core/battle.hpp"
#include <iostream>

using namespace std;

void run_cli(Battle &b) {
    int turn = 1;

    while (!b.over) {
        std::cout << "Turn " << turn++ << ".\n";
		print_mon(b.active1);
        b.run();
    }
}

void print_mon(Pokemon mon) {
	cout << mon.name() << " HP: " << mon.hp() << "\n";
}