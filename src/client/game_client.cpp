#include "game_client.hpp"
#include <iostream>

GameClient::GameClient(const std::string &name)
    : player_name_(name), connected_(false), in_game_(false) {}

GameClient::~GameClient() { disconnect(); }

bool GameClient::connect(const std::string &host, int port) {
  std::cout << "[Client] Connecting to " << host << ":" << port << "...\n";

  if (!socket_.connect(host, port)) {
    std::cerr << "[Client] Failed to connect\n";
    return false;
  }

  connected_ = true;
  std::cout << "[Client] Connected!\n";

  // Send connection request with player name
  Message connect_msg(MessageType::CONNECT_REQUEST, player_name_);
  socket_.send(connect_msg.serialize());

  // Wait for response
  std::vector<uint8_t> data = socket_.receive();
  if (!data.empty()) {
    Message response = Message::deserialize(data);
    if (response.type == MessageType::CONNECT_RESPONSE) {
      std::cout << "[Server] " << response.get_payload_string() << "\n";
    }
  }

  return true;
}

void GameClient::disconnect() {
  if (connected_) {
    Message disc_msg(MessageType::DISCONNECT);
    socket_.send(disc_msg.serialize());
    socket_.close();
    connected_ = false;
    std::cout << "[Client] Disconnected\n";
  }
}

void GameClient::display_message(const std::string &msg) {
  std::cout << msg << "\n";
}

void GameClient::handle_message(const Message &msg) {
  switch (msg.type) {
  case MessageType::GAME_START:
    std::cout << "\n=== GAME STARTING ===\n";
    in_game_ = true;
    break;

  case MessageType::TEAM_DATA:
    std::cout << "[Team Info] " << msg.get_payload_string() << "\n";
    break;

  case MessageType::BATTLE_UPDATE:
    std::cout << "\n" << msg.get_payload_string() << "\n";
    break;

  case MessageType::BATTLE_LOG:
    std::cout << msg.get_payload_string();
    break;

  case MessageType::MOVE_REQUEST: {
    // Just display the move request, don't prompt yet
    // The prompt will happen in the run() loop after all messages are processed
    std::cout << "\n" << msg.get_payload_string() << "\n";
    break;
  }

  case MessageType::SWITCH_REQUEST: {
    // Display the switch request message from server (includes Pokemon list)
    std::cout << msg.get_payload_string();
    int switch_choice = prompt_switch_choice();

    Message response(MessageType::SWITCH_RESPONSE);
    response.set_payload_int(switch_choice);
    socket_.send(response.serialize());
    std::cout << "[Client] Switch response sent: " << switch_choice << "\n";
    break;
  }

  case MessageType::WINNER_DECLARED:
    std::cout << "\n=== GAME OVER ===\n";
    std::cout << "Winner: " << msg.get_payload_string() << "\n";
    in_game_ = false;
    break;

  case MessageType::ERROR_MSG:
    std::cerr << "[Error] " << msg.get_payload_string() << "\n";
    break;

  default:
    std::cout << "[Unknown message type]\n";
    break;
  }
}

int GameClient::prompt_move_choice() {
  int choice;
  std::cout << "Your choice: ";
  std::cin >> choice;
  return choice;
}

int GameClient::prompt_switch_choice() {
  int choice;
  std::cout << "Your choice: ";
  std::cin >> choice;
  return choice;
}

void GameClient::run() {
  std::cout << "[Client] Waiting for game to start...\n";

  bool waiting_for_move = false;

  while (connected_) {
    // Receive message from server
    std::vector<uint8_t> data = socket_.receive();

    if (data.empty()) {
      std::cout << "[Client] Connection lost\n";
      connected_ = false;
      break;
    }

    Message msg = Message::deserialize(data);

    // Check if this is a move request
    if (msg.type == MessageType::MOVE_REQUEST) {
      waiting_for_move = true;
    }

    handle_message(msg);

    // If we received a move request, prompt for input now
    if (waiting_for_move) {
      waiting_for_move = false;

      int move_choice = prompt_move_choice();
      Message response(MessageType::MOVE_RESPONSE);
      response.set_payload_int(move_choice);
      socket_.send(response.serialize());
    }

    // Exit if game is over
    if (!in_game_ && msg.type == MessageType::WINNER_DECLARED) {
      break;
    }
  }

  std::cout << "[Client] Exiting...\n";
}
