#include "core/battle.hpp"
#include "engine/damage.hpp"
#include <iostream>

void Battle::execute_turn(int player_move_index, int ai_move_index) {
    turn++;
    
    // Determine turn order based on Speed
    bool player_first = active1.stat(PokeStat::Speed) >= active2.stat(PokeStat::Speed);
    
    Pokemon* first = player_first ? &active1 : &active2;
    Pokemon* second = player_first ? &active2 : &active1;
    int first_move_idx = player_first ? player_move_index : ai_move_index;
    int second_move_idx = player_first ? ai_move_index : player_move_index;
    
    // First Pokemon attacks
    if (first->hp() > 0) {
        const Move& move = first->get_move(first_move_idx);
        apply_move(*first, *second, move);
    }
    
    // Second Pokemon attacks (if still alive)
    if (second->hp() > 0 && first->hp() > 0) {
        const Move& move = second->get_move(second_move_idx);
        apply_move(*second, *first, move);
    }
    
    // Check if battle is over
    if (active1.hp() <= 0 || active2.hp() <= 0) {
        over = true;
    }
}

void Battle::apply_move(Pokemon& attacker, Pokemon& defender, const Move& move) {
    if (!move.data) {
        std::cout << attacker.name() << " has no move!\n";
        return;
    }
    
    std::cout << attacker.name() << " used " << move.data->name << "!\n";
    
    DamageResult result = calculate_damage(attacker, defender, move);
    
    if (result.type_effectiveness == 0.0f) {
        std::cout << "It doesn't affect " << defender.name() << "...\n";
        return;
    }
    
    defender.take_damage(result.damage);
    
    std::cout << defender.name() << " took " << result.damage << " damage!\n";
    
    if (result.critical) {
        std::cout << "Critical hit!\n";
    }
    
    if (result.type_effectiveness > 1.0f) {
        std::cout << "It's super effective!\n";
    } else if (result.type_effectiveness < 1.0f && result.type_effectiveness > 0.0f) {
        std::cout << "It's not very effective...\n";
    }
    
    if (defender.hp() <= 0) {
        std::cout << defender.name() << " fainted!\n";
    }
}
