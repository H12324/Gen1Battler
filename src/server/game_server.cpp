#include "game_server.hpp"
#include "../network/protocol.hpp"
#include <iostream>

GameServer::GameServer(int port) : port_(port), next_player_id_(1) {}

GameServer::~GameServer() {
  stop();
  for (auto *client : clients_) {
    delete client;
  }
  clients_.clear();
}

bool GameServer::start() {
  std::cout << "[Server] Starting on port " << port_ << "...\n";

  if (!listen_socket_.listen(port_)) {
    std::cerr << "[Server] Failed to start listening\n";
    return false;
  }

  std::cout << "[Server] Server started successfully\n";
  return true;
}

void GameServer::stop() {
  std::cout << "[Server] Stopping server...\n";
  listen_socket_.close();
}

ClientConnection *GameServer::accept_client() {
  std::cout << "[Server] Waiting for client connection...\n";

  Socket *client_socket = listen_socket_.accept();
  if (!client_socket) {
    std::cerr << "[Server] Failed to accept client\n";
    return nullptr;
  }

  // Create client connection
  ClientConnection *client =
      new ClientConnection(client_socket, next_player_id_++);

  // Receive connection request with player name
  std::vector<uint8_t> data = client_socket->receive();
  if (!data.empty()) {
    Message msg = Message::deserialize(data);
    if (msg.type == MessageType::CONNECT_REQUEST) {
      client->player_name = msg.get_payload_string();
      std::cout << "[Server] Player '" << client->player_name
                << "' connected (ID: " << client->player_id << ")\n";

      // Send connection response
      Message response(MessageType::CONNECT_RESPONSE, "Welcome to the server!");
      client_socket->send(response.serialize());
    }
  }

  clients_.push_back(client);
  return client;
}

void GameServer::wait_for_clients(int count) {
  std::cout << "[Server] Waiting for " << count << " clients...\n";

  while (clients_.size() < static_cast<size_t>(count)) {
    accept_client();
    std::cout << "[Server] " << clients_.size() << "/" << count
              << " clients connected\n";
  }

  std::cout << "[Server] All clients connected!\n";
}

void GameServer::disconnect_client(ClientConnection *client) {
  auto it = std::find(clients_.begin(), clients_.end(), client);
  if (it != clients_.end()) {
    std::cout << "[Server] Disconnecting player '" << client->player_name
              << "'\n";
    clients_.erase(it);
    delete client;
  }
}
