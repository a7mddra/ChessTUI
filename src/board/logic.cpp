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

const Piece& Board::cell(int r, int c) const {
    static const Piece empty = Square;
    if (r < 0 || r >= consts::ROWS || c < 0 || c >= consts::COLS) return empty;
    return gBoard[r][c];
}


void Board::markValid(std::pair<size_t, size_t> from)
{
    int r = static_cast<int>(from.first);
    int c = static_cast<int>(from.second);
    auto &[ai_r, ai_c] = ai;

    const Piece &p   = gBoard[r][c];
    const Piece &aiP = gBoard[ai_r][ai_c];

    switch (p.identity)
    {
    case PAWN:
    {
        int dir = p.isWhite ? -1 : 1;

        int st1 = r + dir;
        if (st1 >= 0 && st1 < consts::ROWS)
        {
            if (gBoard[st1][c].identity == SQUARE)
                eval[st1][c] = 1;

            int st2 = r + 2*dir;
            if (!p.moved && st2 >= 0 && st2 < consts::ROWS
                && gBoard[st2][c].identity == SQUARE
                && gBoard[st1][c].identity == SQUARE)
            {
                eval[st2][c] = 1;
            }
        }

        for (auto [dr, dc] : p.deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < consts::ROWS && nc >= 0 && nc < consts::COLS)
            {
                const Piece &tgt = gBoard[nr][nc];
                if (!tgt.isme && tgt.identity != SQUARE)
                {
                    eval[nr][nc] = -1;
                }
                if (enpAI and ai_r == r and ai_c == nc)
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
