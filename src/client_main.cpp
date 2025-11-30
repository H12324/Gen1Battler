#include "client/game_client.hpp"
#include <iostream>

int main(int argc, char **argv) {
  std::string host = "127.0.0.1"; // Default to localhost
  int port = 8888;                // Default port

  if (argc > 1)
    host = argv[1];
  if (argc > 2)
    port = std::atoi(argv[2]);

  std::cout << "=== Pokemon Gen 1 Battler - Client ===\n\n";

  // Get player name
  std::string name;
  std::cout << "Enter your name: ";
  std::getline(std::cin, name);

  if (name.empty()) {
    name = "Player";
  }

  // Create and connect client
  GameClient client(name);

  if (!client.connect(host, port)) {
    std::cerr << "Failed to connect to server\n";
    return 1;
  }

  // Run client main loop
  client.run();

  return 0;
}
