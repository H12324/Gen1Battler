#pragma once
#include <iostream>
#include <sstream>
#include <streambuf>

// Captures std::cout output to a string buffer
class OutputCapture {
private:
  std::stringstream buffer_;
  std::streambuf *old_cout_;
  bool capturing_;

public:
  OutputCapture() : old_cout_(nullptr), capturing_(false) {}

  void start() {
    if (!capturing_) {
      buffer_.str("");
      buffer_.clear();
      old_cout_ = std::cout.rdbuf(buffer_.rdbuf());
      capturing_ = true;
    }
  }

  std::string stop() {
    if (capturing_) {
      std::cout.rdbuf(old_cout_);
      capturing_ = false;
      return buffer_.str();
    }
    return "";
  }

  ~OutputCapture() {
    if (capturing_) {
      std::cout.rdbuf(old_cout_);
    }
  }
};
