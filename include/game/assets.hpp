#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "utils/tui/colors.hpp"
#include "state.hpp"

namespace assets
{
    extern std::string cursor;
    extern std::vector<std::string> frames;
    extern std::vector<std::string> splash;
    extern std::vector<std::string> header;
    extern std::unordered_map<char, std::string> pieces;
    extern std::unordered_map<gst, std::vector<std::string>> logs;
}
