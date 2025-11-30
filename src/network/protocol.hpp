#pragma once
#include <cstdint>
#include <string>
#include <vector>

// Message types for client-server communication
enum class MessageType : uint8_t {
  // Connection
  CONNECT_REQUEST = 0,
  CONNECT_RESPONSE = 1,

  // Game Setup
  GAME_START = 10,
  TEAM_DATA = 11,

  // Battle
  MOVE_REQUEST = 20,
  MOVE_RESPONSE = 21,
  SWITCH_REQUEST = 22,
  SWITCH_RESPONSE = 23,
  BATTLE_UPDATE = 24,
  BATTLE_LOG = 25,

  // Tournament
  TOURNAMENT_START = 30,
  MATCH_START = 31,
  ROUND_COMPLETE = 32,
  BRACKET_UPDATE = 33,

  // Results
  GAME_OVER = 40,
  WINNER_DECLARED = 41,

  // General
  ERROR_MSG = 50,
  DISCONNECT = 51,
  PING = 52,
  PONG = 53
};

// Message structure
struct Message {
  MessageType type;
  std::vector<uint8_t> payload;

  Message() : type(MessageType::PING) {}
  Message(MessageType t) : type(t) {}
  Message(MessageType t, const std::vector<uint8_t> &p) : type(t), payload(p) {}
  Message(MessageType t, const std::string &str);

  // Serialization
  std::vector<uint8_t> serialize() const;
  static Message deserialize(const std::vector<uint8_t> &data);

  // Convenience methods for payload
  void set_payload(const std::string &str);
  std::string get_payload_string() const;

  void set_payload_int(int value);
  int get_payload_int() const;
};
