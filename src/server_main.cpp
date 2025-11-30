#include "data/loader.hpp"
#include "server/game_server.hpp"
#include "server/network_battle.hpp"
#include "server/team_generator.hpp"
#include <iostream>

int main(int argc, char **argv) {
  int port = 8888; // Default port
  if (argc > 1) {
    port = std::atoi(argv[1]);
  }

  std::cout << "=== Pokemon Gen 1 Battler - Server ===\n\n";

  // Load game data
  std::cout << "Loading game data...\n";
  load_species("src/data/species.json");
  load_moves("src/data/moves.json");
  load_type_chart("src/data/type_chart.json");
  std::cout << "Game data loaded!\n\n";

  // Create and start server
  GameServer server(port);
  if (!server.start()) {
    std::cerr << "Failed to start server\n";
    return 1;
  }

  // Menu: 1v1 or Tournament
  std::cout << "\nSelect mode:\n";
  std::cout << "1. 1v1 Battle (2 players)\n";
  std::cout << "2. Tournament (8 players) [NOT YET IMPLEMENTED]\n";
  std::cout << "Choice: ";

  int mode;
  std::cin >> mode;

  if (mode == 1) {
    // 1v1 Battle Mode
    std::cout << "\n=== 1v1 Battle Mode ===\n";
    server.wait_for_clients(2);

    auto clients = server.get_clients();
    if (clients.size() < 2) {
      std::cerr << "Not enough clients connected\n";
      return 1;
    }

    // Generate random teams
    std::cout << "\nGenerating random teams...\n";
    auto team1 = generate_random_team(6, 50);
    auto team2 = generate_random_team(6, 50);
    std::cout << "Teams generated!\n\n";

    // Create and run battle
    NetworkBattle battle(team1, team2, clients[0], clients[1]);
    battle.run();

    std::cout << "\nBattle complete! Server shutting down...\n";
  } else if (mode == 2) {
    std::cout << "\nTournament mode not yet implemented!\n";
  } else {
    std::cout << "\nInvalid choice\n";
  }

  server.stop();
  return 0;
}
