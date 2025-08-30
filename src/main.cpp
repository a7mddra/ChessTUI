#include "board.hpp"

int main()
{
    auto board = std::make_shared<Board>();
    board->init(true);
    board->run();
    return 0;
}
