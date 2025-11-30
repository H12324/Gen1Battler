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
  return 0;
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
  flush_battle_log();
  broadcast_battle_state();

  while (!over) {
    turn++;

    send_battle_log("\n========== Turn " + std::to_string(turn) +
                    " ==========");
    flush_battle_log();

    active1.reset_turn_data();
    active2.reset_turn_data();

    // Send move requests to BOTH players simultaneously
    std::stringstream p1_request;
    p1_request << "Choose your move:\n";
    for (int i = 0; i < active1.move_count(); i++) {
      const Move &move = active1.get_move(i);
      p1_request << i << ". " << move.data->name << " (PP: " << move.current_pp
                 << "/" << move.max_pp() << ")\n";
    }
    p1_request << "Your choice (0-" << (active1.move_count() - 1) << "): ";

    std::stringstream p2_request;
    p2_request << "Choose your move:\n";
    for (int i = 0; i < active2.move_count(); i++) {
      const Move &move = active2.get_move(i);
      p2_request << i << ". " << move.data->name << " (PP: " << move.current_pp
                 << "/" << move.max_pp() << ")\n";
    }
    p2_request << "Your choice (0-" << (active2.move_count() - 1) << "): ";

    Message p1_req_msg(MessageType::MOVE_REQUEST, p1_request.str());
    Message p2_req_msg(MessageType::MOVE_REQUEST, p2_request.str());
    send_to_player(player1_conn_, p1_req_msg);
    send_to_player(player2_conn_, p2_req_msg);

    // Wait for both responses
    std::vector<uint8_t> p1_data = player1_conn_->socket->receive();
    std::vector<uint8_t> p2_data = player2_conn_->socket->receive();

    int p1_move = 0;
    int p2_move = 0;

    if (!p1_data.empty()) {
      Message p1_response = Message::deserialize(p1_data);
      if (p1_response.type == MessageType::MOVE_RESPONSE) {
        p1_move = p1_response.get_payload_int();
        if (p1_move < 0 || p1_move >= active1.move_count()) {
          p1_move = 0;
        }
      }
    }

    if (!p2_data.empty()) {
      Message p2_response = Message::deserialize(p2_data);
      if (p2_response.type == MessageType::MOVE_RESPONSE) {
        p2_move = p2_response.get_payload_int();
        if (p2_move < 0 || p2_move >= active2.move_count()) {
          p2_move = 0;
        }
      }
    }

    // Determine turn order
    bool player1_first = active1.get_modified_stat(PokeStat::Speed) >=
                         active2.get_modified_stat(PokeStat::Speed);

    Pokemon *first = player1_first ? &active1 : &active2;
    Pokemon *second = player1_first ? &active2 : &active1;
    int first_move_idx = player1_first ? p1_move : p2_move;
    int second_move_idx = player1_first ? p2_move : p1_move;

    first->set_moved_first(true);
    second->set_moved_first(false);

    // Execute moves
    if (first->hp() > 0) {
      execute_pokemon_move(*first, *second, first_move_idx);
    }

    if (second->hp() > 0 && first->hp() > 0) {
      execute_pokemon_move(*second, *first, second_move_idx);
    }

    flush_battle_log();
    sync_active_to_team();

    // Check if active Pokemon fainted - switch to next
    if (active1.hp() == 0 && !is_team_defeated(1)) {
      send_battle_log(active1.name() + " fainted!");
      int next = get_next_available_pokemon(1);
      if (next != -1) {
        switch_pokemon(1, next);
        send_battle_log("Go, " + active1.name() + "!");
      }
    }

    if (active2.hp() == 0 && !is_team_defeated(2)) {
      send_battle_log(active2.name() + " fainted!");
      int next = get_next_available_pokemon(2);
      if (next != -1) {
        switch_pokemon(2, next);
        send_battle_log("Go, " + active2.name() + "!");
      }
    }

    // Check if battle is over
    if (is_team_defeated(1) || is_team_defeated(2)) {
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

    // End of turn updates
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
  }

  std::cout << "[Server] Battle complete!\n";
}
