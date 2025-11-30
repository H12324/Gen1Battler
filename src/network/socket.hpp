#pragma once
#include <cstdint>
#include <string>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

class Socket {
private:
  socket_t socket_fd_;
  bool is_connected_;
  bool is_listening_;

  static bool winsock_initialized_;
  static void initialize_winsock();
  static void cleanup_winsock();

public:
  Socket();
  Socket(socket_t fd); // For accepted connections
  ~Socket();

  // Server methods
  bool listen(int port, int backlog = 5);
  Socket *accept();

  // Client methods
  bool connect(const std::string &host, int port);

  // Communication
  bool send(const std::vector<uint8_t> &data);
  std::vector<uint8_t> receive(size_t max_size = 4096);
  bool receive_exact(std::vector<uint8_t> &buffer, size_t size);

  // State
  bool is_connected() const { return is_connected_; }
  void close();

  // Utility
  socket_t get_fd() const { return socket_fd_; }
};
