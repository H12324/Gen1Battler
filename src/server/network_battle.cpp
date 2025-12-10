#include "network_battle.hpp"
#include "../engine/move_effects.hpp"
#include "../network/protocol.hpp"
#include <iostream>
#include <sstream>

NetworkBattle::NetworkBattle(const std::vector<Pokemon> &team1,
                             const std::vector<Pokemon> &team2,
                             ClientConnection *p1, ClientConnection *p2)
    : Battle(team1, team2), player1_conn_(p1), player2_conn_(p2) {}

void NetworkBattle::send_to_player(ClientConnection *client,
                                   const Message &msg) {
  client->socket->send(msg.serialize());
}

void NetworkBattle::send_team_data(ClientConnection *client,
                                   const std::vector<Pokemon> &team,
                                   bool is_own_team) {
  std::stringstream ss;
  ss << (is_own_team ? "Your team:\n" : "Opponent team:\n");
  for (size_t i = 0; i < team.size(); i++) {
    ss << (i + 1) << ". " << team[i].name() << " (Lv. " << team[i].level()
       << ") HP: " << team[i].hp() << "/" << team[i].max_hp() << "\n";
  }

  Message msg(MessageType::TEAM_DATA, ss.str());
  send_to_player(client, msg);
}

void NetworkBattle::send_battle_log(const std::string &message) {
  battle_log_.push_back(message);
}

void NetworkBattle::flush_battle_log() {
  if (battle_log_.empty())
    return;

  std::stringstream ss;
  for (const auto &msg : battle_log_) {
    ss << msg << "\n";
  }

  Message log_msg(MessageType::BATTLE_LOG, ss.str());
  send_to_player(player1_conn_, log_msg);
  send_to_player(player2_conn_, log_msg);

  battle_log_.clear();
}

void NetworkBattle::broadcast_battle_state() {
  std::stringstream p1_state;
  p1_state << "Your Pokemon: " << active1.name() << " HP: " << active1.hp()
           << "/" << active1.max_hp();
  p1_state << " | Opponent: " << active2.name() << " HP: " << active2.hp()
           << "/" << active2.max_hp();

  std::stringstream p2_state;
  p2_state << "Your Pokemon: " << active2.name() << " HP: " << active2.hp()
           << "/" << active2.max_hp();
  p2_state << " | Opponent: " << active1.name() << " HP: " << active1.hp()
           << "/" << active1.max_hp();

  Message p1_update(MessageType::BATTLE_UPDATE, p1_state.str());
  Message p2_update(MessageType::BATTLE_UPDATE, p2_state.str());

  send_to_player(player1_conn_, p1_update);
  send_to_player(player2_conn_, p2_update);
}

int NetworkBattle::request_switch_from_player(ClientConnection *client) {
  // Get available Pokemon for the appropriate team
  int team_num = (client == player1_conn_) ? 1 : 2;
  std::vector<int> available = get_available_pokemon(team_num);

  std::cout << "[Server] Requesting switch from player " << client->player_name
            << " (team " << team_num << ")\n";
  std::cout << "[Server] Available Pokemon: " << available.size() << "\n";

  if (available.empty()) {
    std::cout << "[Server] No Pokemon available for switch!\n";
    return -1; // No Pokemon available
  }

  // Build switch request message
  std::stringstream ss;
  ss << "\nYour Pokemon fainted! Choose a Pokemon to switch to:\n";

  const std::vector<Pokemon> &team = (team_num == 1) ? team1 : team2;
  for (size_t i = 0; i < available.size(); i++) {
    int idx = available[i];
    const Pokemon &p = team[idx];
    ss << (i + 1) << ". " << p.name() << " (Lv. " << p.level()
       << ") HP: " << p.hp() << "/" << p.max_hp() << "\n";
  }
  ss << "Your choice (1-" << available.size() << "): ";

  Message request(MessageType::SWITCH_REQUEST, ss.str());
  send_to_player(client, request);
  std::cout << "[Server] Switch request sent, waiting for response...\n";

  // Wait for response
  std::vector<uint8_t> data = client->socket->receive();

  if (data.empty()) {
    std::cout << "[Server] No response received (empty data), defaulting to "
                 "first available\n";
    return available[0]; // Default to first available if no response
  }

  Message response = Message::deserialize(data);
  std::cout << "[Server] Received message type: "
            << static_cast<int>(response.type) << "\n";

  if (response.type == MessageType::SWITCH_RESPONSE) {
    int choice = response.get_payload_int();
    std::cout << "[Server] Player chose: " << choice << "\n";

    // Validate choice (1-indexed from user, convert to 0-indexed)
    choice = choice - 1;
    if (choice >= 0 && choice < static_cast<int>(available.size())) {
      std::cout << "[Server] Valid choice, switching to index: "
                << available[choice] << "\n";
      return available[choice]; // Return the actual team index
    }
  }

  std::cout << "[Server] Invalid response, defaulting to first available\n";
  return available[0]; // Default to first available
}

void NetworkBattle::log(const std::string &message) {
  send_battle_log(message);
}

void NetworkBattle::run() {
  std::cout << "[Server] Starting network battle between "
            << player1_conn_->player_name << " and "
            << player2_conn_->player_name << "\n";

  send_team_data(player1_conn_, team1, true);
  send_team_data(player1_conn_, team2, false);
  send_team_data(player2_conn_, team2, true);
  send_team_data(player2_conn_, team1, false);

  Message start_msg(MessageType::GAME_START, "Battle starting!");
  send_to_player(player1_conn_, start_msg);
  send_to_player(player2_conn_, start_msg);

  send_battle_log("=== BATTLE START ===");
  send_battle_log(active1.name() + " vs " + active2.name() + "!");
  send_battle_log("\n========== Turn 1 ==========");
  flush_battle_log();
  broadcast_battle_state();

  std::cout << "[Server] Entering battle loop\n";
  while (!over) {
    turn++;
    std::cout << "[Server] Starting turn " << turn << "\n";

    if (turn > 1) {
      send_battle_log("\n========== Turn " + std::to_string(turn) +
                      " ==========");
      flush_battle_log();
    }

    active1.reset_turn_data();
    active2.reset_turn_data();

    // ========== PHASE 1: MOVE SELECTION ==========
    // Send move requests to BOTH players simultaneously
    std::stringstream p1_request;
    p1_request << "Choose your move:\n";
    for (int i = 0; i < active1.move_count(); i++) {
      const Move &move = active1.get_move(i);
      p1_request << (i + 1) << ". " << move.data->name
                 << " (PP: " << move.current_pp << "/" << move.max_pp()
                 << ")\n";
    }
    p1_request << "Your choice (1-" << active1.move_count() << "): ";

    std::stringstream p2_request;
    p2_request << "Choose your move:\n";
    for (int i = 0; i < active2.move_count(); i++) {
      const Move &move = active2.get_move(i);
      p2_request << (i + 1) << ". " << move.data->name
                 << " (PP: " << move.current_pp << "/" << move.max_pp()
                 << ")\n";
    }
    p2_request << "Your choice (1-" << active2.move_count() << "): ";

    Message p1_req_msg(MessageType::MOVE_REQUEST, p1_request.str());
    Message p2_req_msg(MessageType::MOVE_REQUEST, p2_request.str());
    std::cout << "[Server] Sending move requests to both players\n";
    send_to_player(player1_conn_, p1_req_msg);
    send_to_player(player2_conn_, p2_req_msg);

    // Wait for both responses
    std::cout << "[Server] Waiting for move responses...\n";
    std::vector<uint8_t> p1_data = player1_conn_->socket->receive();
    std::cout << "[Server] Received p1 data: " << p1_data.size() << " bytes\n";
    std::vector<uint8_t> p2_data = player2_conn_->socket->receive();
    std::cout << "[Server] Received p2 data: " << p2_data.size() << " bytes\n";

    int p1_move = 0;
    int p2_move = 0;

    if (!p1_data.empty()) {
      Message p1_response = Message::deserialize(p1_data);
      if (p1_response.type == MessageType::MOVE_RESPONSE) {
        p1_move = p1_response.get_payload_int() -
                  1; // Convert from 1-indexed to 0-indexed
        if (p1_move < 0 || p1_move >= active1.move_count()) {
          p1_move = 0;
        }
      }
    }

    if (!p2_data.empty()) {
      Message p2_response = Message::deserialize(p2_data);
      if (p2_response.type == MessageType::MOVE_RESPONSE) {
        p2_move = p2_response.get_payload_int() -
                  1; // Convert from 1-indexed to 0-indexed
        if (p2_move < 0 || p2_move >= active2.move_count()) {
          p2_move = 0;
        }
      }
    }

    // ========== PHASE 2: EXECUTE MOVES ==========
    // Determine turn order
    bool player1_first = active1.get_modified_stat(PokeStat::Speed) >=
                         active2.get_modified_stat(PokeStat::Speed);

    Pokemon *first = player1_first ? &active1 : &active2;
    Pokemon *second = player1_first ? &active2 : &active1;
    int first_move_idx = player1_first ? p1_move : p2_move;
    int second_move_idx = player1_first ? p2_move : p1_move;

    first->set_moved_first(true);
    second->set_moved_first(false);

    // Execute moves in order
    std::cout << "[Server] Executing moves\n";
    if (first->hp() > 0) {
      execute_pokemon_move(*first, *second, first_move_idx);
    }

    if (second->hp() > 0 && first->hp() > 0) {
      execute_pokemon_move(*second, *first, second_move_idx);
    }

    std::cout << "[Server] Flushing battle log\n";
    flush_battle_log();
    sync_active_to_team();

    // ========== PHASE 3: SWITCH PHASE (if Pokemon fainted) ==========
    std::cout << "[Server] Checking for faints\n";
    bool p1_fainted = (active1.hp() == 0);
    bool p2_fainted = (active2.hp() == 0);

    // Handle player 1's fainted Pokemon
    if (p1_fainted && !is_team_defeated(1)) {
      send_battle_log(active1.name() + " fainted!");
      flush_battle_log();

      int switch_choice = request_switch_from_player(player1_conn_);
      if (switch_choice != -1) {
        // Manually switch without using base class method (to avoid duplicate
        // logging)
        team1[active1_index] = active1;
        active1_index = switch_choice;
        active1 = team1[switch_choice];
        send_battle_log("Go, " + active1.name() + "!");
        flush_battle_log();
      }
    }

    // Handle player 2's fainted Pokemon
    if (p2_fainted && !is_team_defeated(2)) {
      send_battle_log(active2.name() + " fainted!");
      flush_battle_log();

      int switch_choice = request_switch_from_player(player2_conn_);
      if (switch_choice != -1) {
        // Manually switch without using base class method (to avoid duplicate
        // logging)
        team2[active2_index] = active2;
        active2_index = switch_choice;
        active2 = team2[switch_choice];
        send_battle_log("Go, " + active2.name() + "!");
        flush_battle_log();
      }
    }

    // ========== PHASE 4: CHECK BATTLE END ==========
    std::cout << "[Server] Checking battle end\n";
    if (is_team_defeated(1) || is_team_defeated(2)) {
      std::cout << "[Server] Battle is over!\n";
      over = true;
      broadcast_battle_state();

      if (is_team_defeated(1)) {
        send_battle_log("\n" + player2_conn_->player_name + " wins!");
        Message winner(MessageType::WINNER_DECLARED,
                       player2_conn_->player_name);
        send_to_player(player1_conn_, winner);
        send_to_player(player2_conn_, winner);
      } else {
        send_battle_log("\n" + player1_conn_->player_name + " wins!");
        Message winner(MessageType::WINNER_DECLARED,
                       player1_conn_->player_name);
        send_to_player(player1_conn_, winner);
        send_to_player(player2_conn_, winner);
      }

      flush_battle_log();
      continue;
    }

    // ========== PHASE 5: END OF TURN EFFECTS ==========
    std::cout << "[Server] Applying end of turn effects\n";
    active1.update_disable();
    active2.update_disable();
    active1.update_bide();
    active2.update_bide();
    active1.update_screens();
    active2.update_screens();

    if (active1.hp() > 0) {
      apply_end_of_turn_status_damage(active1);
    }
    if (active2.hp() > 0) {
      apply_end_of_turn_status_damage(active2);
    }

    sync_active_to_team();
    flush_battle_log();
    broadcast_battle_state();
    std::cout << "[Server] Turn " << turn << " complete\n";
  }

  std::cout << "[Server] Exited battle loop\n";

  std::cout << "[Server] Battle complete!\n";
}
