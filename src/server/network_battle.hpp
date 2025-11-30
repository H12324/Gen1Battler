#pragma once
#include "../core/battle.hpp"
#include "../core/pokemon.hpp"
#include "../network/protocol.hpp"
#include "game_server.hpp"
#include <string>
#include <vector>

// Battle that synchronizes state over network
class NetworkBattle : public Battle {
private:
  ClientConnection *player1_conn_;
  ClientConnection *player2_conn_;
  std::vector<std::string> battle_log_;

  // Network communication
  void send_to_player(ClientConnection *client, const Message &msg);
  int request_move_from_player(ClientConnection *client,
                               const Pokemon &their_pokemon,
                               const Pokemon &opponent_pokemon);
  int request_switch_from_player(ClientConnection *client);

  // State synchronization
  void send_team_data(ClientConnection *client,
                      const std::vector<Pokemon> &team, bool is_own_team);
  void broadcast_battle_state();
  void send_battle_log(const std::string &message);
  void flush_battle_log();

public:
  NetworkBattle(const std::vector<Pokemon> &team1,
                const std::vector<Pokemon> &team2, ClientConnection *p1,
                ClientConnection *p2);

  void run();

  // Override log to send to network instead of stdout
  void log(const std::string &message) override;
};
