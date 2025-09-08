#include "game/assets.hpp"

namespace assets
{
    std::string cursor = "$ ";

    std::unordered_map<gst, std::vector<std::string>> logs = {
        {gst::LOADING,   {"Loading"},},
        {gst::EXITING,   {"Exiting"},},
        {gst::THINKING,  {"Thinking"},},
        {gst::DRAW,      {"Draw! Game over", " "},},
        {gst::CHECKMATE, {"Checkmate! Game over", " "},},
        {gst::STALEMATE, {"Stalemate! Game over", " "},},
        {gst::ERRAI,     {"Stockfish engine error", " "},},
        {gst::ERRMOVE,   {"Invalid move. Try again", " "},},
        {gst::PENDING,   {"Type a valid destination", " "},},
        {gst::PROMOTION, {"Type a piece for promotion",
                          "(Q, R, B, N)"},},
        {gst::INPUT,     {"Use algebraic notation:",
                          "e2 → e4 or e2e4"},},
        {gst::ERRINPUT,  {"Type a valid move, 'quit' to exit,",
                          "'new' for a new game,",
                          "or 'flip' to flip the board"}}};

    std::vector<std::string> frames = {
        "⢀⠀", "⡀⠀", "⠄⠀", "⢂⠀", "⡂⠀", "⠅⠀",
        "⢃⠀", "⡃⠀", "⠍⠀", "⢋⠀", "⡋⠀", "⠍⠁",
        "⢋⠁", "⡋⠁", "⠍⠉", "⠋⠉", "⠋⠉", "⠉⠙",
        "⠉⠙", "⠉⠩", "⠈⢙", "⠈⡙", "⢈⠩", "⡀⢙",
        "⠄⡙", "⢂⠩", "⡂⢘", "⠅⡘", "⢃⠨", "⡃⢐",
        "⠍⡐", "⢋⠠", "⡋⢀", "⠍⡁", "⢋⠁", "⡋⠁",
        "⠍⠉", "⠋⠉", "⠋⠉", "⠉⠙", "⠉⠙", "⠉⠩",
        "⠈⢙", "⠈⡙", "⠈⠩", "⠀⢙", "⠀⡙", "⠀⠩",
        "⠀⢘", "⠀⡘", "⠀⠨", "⠀⢐", "⠀⡐", "⠀⠠",
        "⠀⢀", "⠀⡀"};

    std::vector<std::string> splash = {
        "       ", "      ", "      ", "      ",
        "   ▄▄▄   ██░  ██  ███▄ ▄███▓▓█████▄  ",
        "  ▒████▄  ▓██░ ██▒▓██▒▀█▀ ██▒▒██▀ ██▌",
        " ▒██  ▀█▄ ▒██▀▀██░▓██    ▓██░░██   █▌",
        "░██▄▄▄▄██ ░▓█ ░██ ▒██    ▒██ ░▓█▄  █▌",
        " ▓█   ▓██▒░▓█▒░██▓▒██▒   ░██▒░▒████▓ ",
        " ▒▒   ▓▒█░ ▒ ░░▒░▒░ ▒░   ░  ░ ▒▒▓  ▒ ",
        "  ▒   ▒▒ ░ ▒ ░▒░ ░░  ░      ░ ░ ▒  ▒ ",
        "  ░   ▒    ░  ░░ ░░      ░    ░ ░  ░ ",
        "      ░  ░ ░  ░  ░       ░      ░    ",
        "                              ░      ",
        "                                     "};

    std::vector<std::string> header = {
        "  ____ _   _ _____ ____ ____  ",
        " / ___| | | | ____/ ___/ ___| ",
        "| |   | |_| |  _| \\___ \\___ \\ ",
        "| |___|  _  | |___ ___) |__) |",
        " \\____|_| |_|_____|____/____/ "};

    std::unordered_map<char, std::string> pieces = {
        {'P', "♟"}, {'R', "♜"}, {'B', "♝"}, {'N', "♞"},
        {'p', "♙"}, {'r', "♖"}, {'b', "♗"}, {'n', "♘"},
        {'K', "♚"}, {'Q', "♛"}, {'k', "♔"}, {'q', "♕"},
        {'s', "·"}, {'h', color::blue("•")}};
}
