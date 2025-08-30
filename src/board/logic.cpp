#include "board.hpp"
#include "assets.hpp"
#include "constants.hpp"

void Board::reState()
{
    // check for check, checkmate, stalemate, draw
    setState(gst::INPUT);
}

void Board::refBoard()
{
    for (int r = 0; r < consts::ROWS; ++r)
        for (int c = 0; c < consts::COLS; ++c)
            gBoard[r][c].setEval(eval[r][c]);
}

void Board::markValid(std::pair<size_t, size_t> from)
{
    pending = true;
    // marking logic will update the eval values 1 -> valid move, -1 -> can attack, 0 -> other
    // mark valid moves that are in bounds of a piece limits && wont lead to a self-check
    eval[4][0] = 1;
    refBoard();
}

void Board::umarkValid()
{
    pending = false;
    eval = std::vector<std::vector<int>>(consts::ROWS, std::vector<int>(consts::COLS, 0));
    refBoard();
}
