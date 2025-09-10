#include "game/assets.hpp"

namespace assets
{
    std::unordered_map<gst, std::vector<std::string>> logs = {
        {gst::DRAW,      {"Draw! Game over", " "},},
        {gst::CHECKMATE, {"Checkmate! Game over", " "},},
        {gst::STALEMATE, {"Stalemate! Game over", " "},},
        {gst::ERRAI,     {"Stockfish engine error", " "},},
        {gst::ERRMOVE,   {"Invalid move. Try again", " "},},
        {gst::PENDING,   {"Type a valid destination", " "},},
        {gst::PROMOTION, {"Type a piece for promotion",
                          "(Q, R, B, N)"},},
        {gst::ERRINPUT,  {"Type a valid move, 'quit' to exit,",
                          "'new' for a new game,",
                          "or 'flip' to flip the board"}}};

    std::vector<std::string> frames = {
       "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};

    std::unordered_map<char, std::string> pieces = {
        {'p', "♟"}, {'r', "♜"}, {'b', "♝"}, {'n', "♞"},
        {'k', "♚"}, {'q', "♛"}, {'P', "♙"}, {'R', "♖"},
        {'B', "♗"}, {'N', "♘"}, {'K', "♔"}, {'Q', "♕"},
        {'s', " "}, {'h', "•"}
    };
}
