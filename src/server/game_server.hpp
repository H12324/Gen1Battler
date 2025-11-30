#pragma once
#include "../network/socket.hpp"
#include <string>
#include <vector>

// Represents a connected client
struct ClientConnection {
  Socket *socket;
  std::string player_name;
  int player_id;
  bool ready;

  ClientConnection(Socket *s, int id)
      : socket(s), player_id(id), ready(false) {}

  ~ClientConnection() {
    if (socket) {
      delete socket;
    }
  }
};

// Main game server
class GameServer {
private:
  Socket listen_socket_;
  std::vector<ClientConnection *> clients_;
  int port_;
  int next_player_id_;

public:
  GameServer(int port);
  ~GameServer();

  // Server lifecycle
  bool start();
  void stop();

  // Client management
  ClientConnection *accept_client();
  void wait_for_clients(int count);
  void disconnect_client(ClientConnection *client);

  // Accessors
  const std::vector<ClientConnection *> &get_clients() const {
    return clients_;
  }
  int client_count() const { return clients_.size(); }
};
