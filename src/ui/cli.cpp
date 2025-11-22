#include "ui/cli.hpp"
#include "core/battle.hpp"
#include "core/rng.hpp"
#include <iostream>
#include <limits>

using namespace std;

void run_cli(Battle &b) {
    int turn = 1;

    while (!b.over) {
        std::cout << "\n========== Turn " << turn++ << " ==========\n";
        
        // Display both Pokemon status
        std::cout << "\nYour Pokemon:\n";
        print_mon(b.active1);
        std::cout << "\nOpponent's Pokemon:\n";
        print_mon(b.active2);
        
        // Player chooses move
        std::cout << "\nYour moves:\n";
        for (int i = 0; i < b.active1.move_count(); i++) {
            const Move& move = b.active1.get_move(i);
            std::cout << (i + 1) << ". " << move.data->name 
                     << " (PP: " << move.current_pp << "/" << move.data->max_pp << ")\n";
        }
        
        int player_choice = 0;
        do {
            std::cout << "\nChoose a move (1-" << b.active1.move_count() << "): ";
            std::cin >> player_choice;
            
            // Clear error state if input fails
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                player_choice = 0;
            }
        } while (player_choice < 1 || player_choice > b.active1.move_count());
        
        // AI chooses random move
        int ai_choice = rng_int(0, b.active2.move_count() - 1);
        
        std::cout << "\n";
        b.execute_turn(player_choice - 1, ai_choice);
    }
    
    // Display winner
    std::cout << "\n========== Battle Over ==========\n";
    if (b.active1.hp() > 0) {
        std::cout << "You won!\n";
    } else {
        std::cout << "You lost!\n";
    }
}

void print_mon(const Pokemon& mon) {
	cout << mon.name() << " - HP: " << mon.hp() << "/" << mon.max_hp() << "\n";
}