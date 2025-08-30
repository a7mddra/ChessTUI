#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "colors.hpp"

enum class gst
{
    INPUT, ERRAI, ERRMOVE, ERRINPUT, DRAW, CHECK, CHECKMATE,
    STALEMATE, PROMOTION, PENDING, LOADING, EXITING, THINKING
};

namespace assets
{
    extern std::vector<std::string> frames;
    extern std::vector<std::string> splash;
    extern std::vector<std::string> header;
    extern std::unordered_map<char, std::string> pieces;
    extern std::unordered_map<gst, std::vector<std::string>> logs;
}
