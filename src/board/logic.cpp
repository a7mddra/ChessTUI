#include "board.hpp"
#include "assets.hpp"
#include "constants.hpp"

void Board::syncEval()
{
    for (int r = 0; r < consts::ROWS; ++r)
    {
        for (int c = 0; c < consts::COLS; ++c)
        {
            gBoard[r][c].setEval(eval[r][c]);
        }
    }
}

void Board::setEval(int r, int c, int v)
{
    auto cpy = gBoard;
    applyMove({r, c}, from);
    if (isSafe(isWhite ? KK : kk))
    {
        eval[r][c] = v;
    }
    gBoard = cpy;
}

bool Board::isSafe(const Piece &p)
{
    int r = static_cast<int>(p.pos.first);
    int c = static_cast<int>(p.pos.second);

    for (auto [dr, dc] : Knight.deltas)
    {
        int nr = r + dr;
        int nc = c + dc;
        if (!inBounds(nr, nc))
        {
            continue;
        }
        if (!cell(nr, nc).isMe &&
            cell(nr, nc).identity == KNIGHT)
        {
            return false;
        }
    }

    for (auto [dr, dc] : King.deltas)
    {
        int inc = 1;
        while (inc < consts::ROWS)
        {
            int nr = r + dr * inc;
            int nc = c + dc * inc;

            if (!inBounds(nr, nc))
            {
                break;
            }

            const Piece &tar = cell(nr, nc);
            if (tar.isMe)
            {
                break;
            }

            if (!tar.isMe && !tar.isEmt)
            {
                bool isDiag = (dr != 0 && dc != 0);

                if (tar.identity == QUEEN)
                {
                    return false;
                }

                if (inc == 1 && tar.identity == KING)
                {
                    return false;
                }

                if (isDiag)
                {
                    if (inc == 1 && tar.identity == PAWN)
                    {
                        bool dn = !tar.isWhite && dr == 1;
                        bool up = tar.isWhite && dr == -1;

                        if (dn || up)
                        {
                            return false;
                        }
                    }
                    if (tar.identity == BISHOP)
                    {
                        return false;
                    }
                }
                else
                {
                    if (tar.identity == ROOK)
                    {
                        return false;
                    }
                }
                break;
            }
            inc++;
        }
    }

    return true;
}

void Board::markValid()
{
    int r = static_cast<int>(from.first);
    int c = static_cast<int>(from.second);
    int ar = static_cast<int>(ai.first);
    int ac = static_cast<int>(ai.second);

    auto id = cell(r, c).identity;
    int dir = cell(r, c).isWhite ? -1 : 1;

    switch (id)
    {
    case PAWN:
    {
        int st1 = r + dir;
        if (inBounds(st1, c))
        {
            if (cell(st1, c).isEmt)
            {
                setEval(st1, c, 1);
            }
            int st2 = r + 2 * dir;
            if (!cell(r, c).isMoved && inBounds(st2, c) &&
                cell(st2, c).isEmt && cell(st1, c).isEmt)
            {
                setEval(st2, c, 1);
            }
        }
    }

    case KING:
    {
        if (isSafe(cell(r, c)))
        {
            auto ksc = cell(consts::CTL_ROW, consts::KSC_COL);
            auto rkc = cell(consts::CTL_ROW, consts::RKC_TO);
            auto qsc = cell(consts::CTL_ROW, consts::QSC_COL);
            auto rqc = cell(consts::CTL_ROW, consts::RQC_TO);
            auto chk = [&](Piece &p) -> bool
            {
                return p.isEmt && isSafe(p);
            };
            if (chk(ksc) and chk(rkc))
            {
                setEval(consts::CTL_ROW, consts::KSC_COL, 1);
            }
            if (chk(qsc) and chk(rqc))
            {
                setEval(consts::CTL_ROW, consts::QSC_COL, 1);
            }
        }
    }

    default:
    {
        for (auto [dr, dc] : cell(r, c).deltas)
        {
            dr *= dir;
            dc *= dir;
            int nr = r + dr;
            int nc = c + dc;
            if (inBounds(nr, nc))
            {
                if (!cell(nr, nc).isMe && !cell(nr, nc).isEmt)
                {
                    setEval(nr, nc, -1);
                }
                if (id == PAWN)
                {
                    if (enpAI and ar == r and ac == nc)
                    {
                        setEval(nr, nc, 1);
                        setEval(r, nc, -1);
                    }
                    continue;
                }
                if (cell(nr, nc).isEmt)
                {
                    setEval(nr, nc, 1);
                }
                if (cell(r, c).isSliding)
                {
                    int sr = nr + dr;
                    int sc = nc + dc;
                    while (inBounds(sr, sc))
                    {
                        if (cell(sr, sc).isEmt)
                        {
                            setEval(sr, sc, 1);
                        }
                        else
                        {
                            if (!cell(sr, sc).isMe)
                            {
                                setEval(sr, sc, -1);
                            }
                        }
                        sr += dr;
                        sc += dc;
                    }
                }
            }
        }
        break;
    }
    }
    syncEval();
}

void Board::umarkValid()
{
    eval.assign(consts::ROWS,
                std::vector<int>(consts::COLS, 0));
    syncEval();
}
