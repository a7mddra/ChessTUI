#include "game/board.hpp"

int main()
{
    auto board = std::make_shared<Board>();
    board->isWhite = true;
    board->myTurn = true;
    board->init();
    board->run();
    return 0;
}
