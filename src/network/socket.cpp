#include "socket.hpp"
#include <cstring>
#include <iostream>

bool Socket::winsock_initialized_ = false;

void Socket::initialize_winsock() {
#ifdef _WIN32
  if (!winsock_initialized_) {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
      std::cerr << "WSAStartup failed\n";
      return;
    }
    winsock_initialized_ = true;
  }
#endif
}

void Socket::cleanup_winsock() {
#ifdef _WIN32
  if (winsock_initialized_) {
    WSACleanup();
    winsock_initialized_ = false;
  }
#endif
}

Socket::Socket()
    : socket_fd_(INVALID_SOCKET), is_connected_(false), is_listening_(false) {
  initialize_winsock();
}

Socket::Socket(socket_t fd)
    : socket_fd_(fd), is_connected_(true), is_listening_(false) {
  initialize_winsock();
}

Socket::~Socket() { close(); }

bool Socket::listen(int port, int backlog) {
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ == INVALID_SOCKET) {
    std::cerr << "Failed to create socket\n";
    return false;
  }

  // Allow address reuse
  int opt = 1;
#ifdef _WIN32
  setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt,
             sizeof(opt));
#else
  setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(socket_fd_, (sockaddr *)&server_addr, sizeof(server_addr)) ==
      SOCKET_ERROR) {
    std::cerr << "Bind failed\n";
    close();
    return false;
  }

  if (::listen(socket_fd_, backlog) == SOCKET_ERROR) {
    std::cerr << "Listen failed\n";
    close();
    return false;
  }

  is_listening_ = true;
  return true;
}

Socket *Socket::accept() {
  if (!is_listening_) {
    return nullptr;
  }

  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);

#ifdef _WIN32
  socket_t client_fd =
      ::accept(socket_fd_, (sockaddr *)&client_addr, &client_len);
#else
  socket_t client_fd =
      ::accept(socket_fd_, (sockaddr *)&client_addr, &client_len);
#endif

  if (client_fd == INVALID_SOCKET) {
    return nullptr;
  }

  return new Socket(client_fd);
}

bool Socket::connect(const std::string &host, int port) {
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ == INVALID_SOCKET) {
    std::cerr << "Failed to create socket\n";
    return false;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

#ifdef _WIN32
  server_addr.sin_addr.s_addr = inet_addr(host.c_str());
  if (server_addr.sin_addr.s_addr == INADDR_NONE) {
    // Try to resolve hostname
    std::cerr << "Invalid address or hostname resolution not implemented\n";
    close();
    return false;
  }
#else
  if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address\n";
    close();
    return false;
  }
#endif

  if (::connect(socket_fd_, (sockaddr *)&server_addr, sizeof(server_addr)) ==
      SOCKET_ERROR) {
    std::cerr << "Connection failed\n";
    close();
    return false;
  }

  is_connected_ = true;
  return true;
}

bool Socket::send(const std::vector<uint8_t> &data) {
  if (!is_connected_) {
    return false;
  }

  size_t total_sent = 0;
  while (total_sent < data.size()) {
#ifdef _WIN32
    int sent = ::send(socket_fd_, (const char *)(data.data() + total_sent),
                      data.size() - total_sent, 0);
#else
    ssize_t sent = ::send(socket_fd_, data.data() + total_sent,
                          data.size() - total_sent, 0);
#endif

    if (sent == SOCKET_ERROR) {
      std::cerr << "Send failed\n";
      is_connected_ = false;
      return false;
    }

    total_sent += sent;
  }

  return true;
}

std::vector<uint8_t> Socket::receive(size_t max_size) {
  if (!is_connected_) {
    return {};
  }

  std::vector<uint8_t> buffer(max_size);

#ifdef _WIN32
  int received = ::recv(socket_fd_, (char *)buffer.data(), max_size, 0);
#else
  ssize_t received = ::recv(socket_fd_, buffer.data(), max_size, 0);
#endif

  if (received == SOCKET_ERROR || received == 0) {
    is_connected_ = false;
    return {};
  }

  buffer.resize(received);
  return buffer;
}

bool Socket::receive_exact(std::vector<uint8_t> &buffer, size_t size) {
  if (!is_connected_) {
    return false;
  }

  buffer.resize(size);
  size_t total_received = 0;

  while (total_received < size) {
#ifdef _WIN32
    int received = ::recv(socket_fd_, (char *)(buffer.data() + total_received),
                          size - total_received, 0);
#else
    ssize_t received = ::recv(socket_fd_, buffer.data() + total_received,
                              size - total_received, 0);
#endif

    if (received == SOCKET_ERROR || received == 0) {
      is_connected_ = false;
      return false;
    }

    total_received += received;
  }

  return true;
}

void Socket::close() {
  if (socket_fd_ != INVALID_SOCKET) {
#ifdef _WIN32
    closesocket(socket_fd_);
#else
    ::close(socket_fd_);
#endif
    socket_fd_ = INVALID_SOCKET;
  }
  is_connected_ = false;
  is_listening_ = false;
}
