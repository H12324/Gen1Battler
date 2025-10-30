#pragma once
#include <string>

// Loader functions are placeholders. For a real project you may use nlohmann/json
// or another parser to load the JSON files from disk.
void load_species(const std::string &path);
void load_moves(const std::string &path);
void load_type_chart(const std::string &path);
