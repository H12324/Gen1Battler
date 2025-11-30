#include "network/protocol.hpp"
#include "network/socket.hpp"
#include <iostream>
#include <thread>

void server_test() {
  Socket server;
  std::cout << "[Server] Starting on port 8888...\n";

  if (!server.listen(8888)) {
    std::cerr << "[Server] Failed to start\n";
    return;
  }

  std::cout << "[Server] Waiting for client...\n";
  Socket *client = server.accept();

  if (!client) {
    std::cerr << "[Server] Failed to accept client\n";
    return;
  }

  std::cout << "[Server] Client connected!\n";

  // Receive message
  std::vector<uint8_t> data = client->receive();
  if (!data.empty()) {
    Message msg = Message::deserialize(data);
    std::cout << "[Server] Received: " << msg.get_payload_string() << "\n";

    // Send response
    Message response(MessageType::CONNECT_RESPONSE, "Hello from server!");
    client->send(response.serialize());
    std::cout << "[Server] Sent response\n";
  }

  delete client;
  std::cout << "[Server] Test complete\n";
}

void client_test() {
  // Give server time to start
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  Socket client;
  std::cout << "[Client] Connecting to localhost:8888...\n";

  if (!client.connect("127.0.0.1", 8888)) {
    std::cerr << "[Client] Connection failed\n";
    return;
  }

  std::cout << "[Client] Connected!\n";

  // Send message
  Message msg(MessageType::CONNECT_REQUEST, "Hello from client!");
  client.send(msg.serialize());
  std::cout << "[Client] Sent message\n";

  // Receive response
  std::vector<uint8_t> data = client.receive();
  if (!data.empty()) {
    Message response = Message::deserialize(data);
    std::cout << "[Client] Received: " << response.get_payload_string() << "\n";
  }

  std::cout << "[Client] Test complete\n";
}

int main() {
  std::cout << "=== Network Test ===\n\n";

  // Run server and client in separate threads
  std::thread server_thread(server_test);
  std::thread client_thread(client_test);

  server_thread.join();
  client_thread.join();

  std::cout << "\n=== Test Complete ===\n";
  return 0;
}
