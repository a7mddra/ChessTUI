#include "board.hpp"
#include "assets.hpp"
#include "constants.hpp"

void Board::reState()
{
    // check for check, checkmate, stalemate, draw
    setState(gst::INPUT);

}

void Board::syncEval()
{
    for (int r = 0; r < consts::ROWS; ++r)
        for (int c = 0; c < consts::COLS; ++c)
            gBoard[r][c].setEval(eval[r][c]);
}

bool Board::inBounds(int r, int c) const {
    return r >= 0 && r < consts::ROWS
        && c >= 0 && c < consts::COLS;
}

const Piece& Board::cell(int r, int c) const {
    if (inBounds(r,c))
    {
        return gBoard[r][c];
    }
    return sq;
}

void Board::markValid(std::pair<size_t, size_t> from)
{
    int r  = static_cast<int>(from.first);
    int c  = static_cast<int>(from.second);
    int ar = static_cast<int>(ai.first);
    int ac = static_cast<int>(ai.second);

    switch (cell(r, c).identity)
    {
    case PAWN:
    {
        int dir = cell(r, c).isWhite ? -1 : 1;

        int st1 = r + dir;
        if (inBounds(st1, c))
        {
            if (gBoard[st1][c].identity == SQUARE)
                eval[st1][c] = 1;

            int st2 = r + 2*dir;
            if (!cell(r, c).moved && inBounds(st2, c)
                && gBoard[st2][c].identity == SQUARE
                && gBoard[st1][c].identity == SQUARE)
            {
                eval[st2][c] = 1;
            }
        }

        for (auto [dr, dc] : cell(r, c).deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            if (inBounds(nr, nc))
            {
                if (!cell(nr, nc).isme &&
                    cell(nr, nc).identity != SQUARE)
                {
                    eval[nr][nc] = -1;
                }
                if (enpAI and ar == r and ac == nc)
                {
                    eval[nr][nc] = 1;
                    eval[r][nc] = -1;
                }
            }
        }
        break;
    }

    default:
        break;
    }

    syncEval();
}

void Board::umarkValid()
{
    eval.assign(consts::ROWS,
        std::vector<int>(consts::COLS, 0));
    syncEval();
}
