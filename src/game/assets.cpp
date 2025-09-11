#include "game/assets.hpp"

namespace assets
{ 
    std::vector<std::string> frames = {
       "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};

    std::unordered_map<char, std::string> pieces = {
        {'p', "♟"}, {'r', "♜"}, {'b', "♝"}, {'n', "♞"},
        {'k', "♚"}, {'q', "♛"}, {'P', "♙"}, {'R', "♖"},
        {'B', "♗"}, {'N', "♘"}, {'K', "♔"}, {'Q', "♕"},
        {'s', " "}, {'h', "•"}
    };
}
