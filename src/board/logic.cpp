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
    // use signed ints for arithmetic to avoid unsigned underflow
    int r = static_cast<int>(from.first);
    int c = static_cast<int>(from.second);

    const Piece &p = gBoard[from.first][from.second]; // read-only reference

    switch (p.identity)
    {
    case PAWN:
    {
        // direction: white pawns move -1 (up), black pawns +1 (down)
        int dir = p.isWhite ? -1 : 1;

        int one = r + dir;
        if (one >= 0 && one < consts::ROWS)
        {
            if (gBoard[one][c].identity == SQUARE)
                eval[one][c] = 1;

            int two = r + 2*dir;
            if (!p.moved && two >= 0 && two < consts::ROWS && gBoard[two][c].identity == SQUARE
                && gBoard[one][c].identity == SQUARE)
            {
                eval[two][c] = 1;
            }
        }

        // captures: use the piece's deltas (which should be oriented for piece color)
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
                // TODO: en passant capture detection can also mark an attack square here
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
