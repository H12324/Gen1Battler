#include "protocol.hpp"
#include <cstring>

Message::Message(MessageType t, const std::string &str) : type(t) {
  set_payload(str);
}

void Message::set_payload(const std::string &str) {
  payload.resize(str.size());
  std::memcpy(payload.data(), str.data(), str.size());
}

std::string Message::get_payload_string() const {
  return std::string(payload.begin(), payload.end());
}

void Message::set_payload_int(int value) {
  payload.resize(sizeof(int));
  std::memcpy(payload.data(), &value, sizeof(int));
}

int Message::get_payload_int() const {
  if (payload.size() < sizeof(int)) {
    return 0;
  }
  int value;
  std::memcpy(&value, payload.data(), sizeof(int));
  return value;
}

std::vector<uint8_t> Message::serialize() const {
  // Format: [4 bytes: payload length][1 byte: message type][N bytes: payload]
  std::vector<uint8_t> result;

  uint32_t payload_len = static_cast<uint32_t>(payload.size());

  // Add payload length (4 bytes, network byte order)
  result.push_back((payload_len >> 24) & 0xFF);
  result.push_back((payload_len >> 16) & 0xFF);
  result.push_back((payload_len >> 8) & 0xFF);
  result.push_back(payload_len & 0xFF);

  // Add message type (1 byte)
  result.push_back(static_cast<uint8_t>(type));

  // Add payload
  result.insert(result.end(), payload.begin(), payload.end());

  return result;
}

Message Message::deserialize(const std::vector<uint8_t> &data) {
  if (data.size() < 5) {
    // Invalid message
    return Message(MessageType::ERROR_MSG);
  }

  // Read payload length (4 bytes, network byte order)
  uint32_t payload_len = (static_cast<uint32_t>(data[0]) << 24) |
                         (static_cast<uint32_t>(data[1]) << 16) |
                         (static_cast<uint32_t>(data[2]) << 8) |
                         static_cast<uint32_t>(data[3]);

  // Read message type (1 byte)
  MessageType msg_type = static_cast<MessageType>(data[4]);

  // Read payload
  std::vector<uint8_t> payload;
  if (payload_len > 0 && data.size() >= 5 + payload_len) {
    payload.assign(data.begin() + 5, data.begin() + 5 + payload_len);
  }

  return Message(msg_type, payload);
}
