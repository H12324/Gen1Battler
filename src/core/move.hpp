#pragma once
#include <string>

struct Move {
    std::string name;
    int power = 0;
    int accuracy = 100; // percent
};
