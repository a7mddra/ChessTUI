#include "board.hpp"

int main()
{
    auto board = std::make_shared<Board>();
    board->run();
    return 0;
}
