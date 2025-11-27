#pragma once
#include "../ai/ai_interface.hpp"
#include "../core/battle.hpp"


void run_cli(Battle &b, const BattleAI &ai);
void print_mon(const Pokemon &mon);

void listPossibleMoves();
