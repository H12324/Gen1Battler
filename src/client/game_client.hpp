#pragma once
#include "../network/protocol.hpp"
#include "../network/socket.hpp"
#include <string>

// Client that connects to game server
class GameClient {
private:
  Socket socket_;
  std::string player_name_;
  bool connected_;
  bool in_game_;

  // Message handling
  void handle_message(const Message &msg);
  void display_message(const std::string &msg);

public:
  GameClient(const std::string &name);
  ~GameClient();

  // Connection
  bool connect(const std::string &host, int port);
  void disconnect();

  // Main loop
  void run();

  // Input
  int prompt_move_choice();
  int prompt_switch_choice();

  // State
  bool is_connected() const { return connected_; }
  bool is_in_game() const { return in_game_; }
};
