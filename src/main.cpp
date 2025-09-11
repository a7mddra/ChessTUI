#include "game/board.hpp"
#include "utils/tui/gnome.hpp"

int main(int argc, char **argv)
{
    gnome::loadProfile(argc, argv);
    auto board = std::make_shared<Board>();
    board->isWhite = true;
    board->myTurn = true;
    board->init();
    board->run();

    return 0;
}
