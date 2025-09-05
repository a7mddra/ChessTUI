#include "game/board.hpp"
#include "game/consts.hpp"

const bool Board::isEmpty(Pos p) const
{
    return pMap.find(p) == pMap.end();
}

const bool Board::inRange(Pos p) const
{
    auto r = static_cast<int>(p.first);
    auto c = static_cast<int>(p.second);
    bool row = r >= 0 && r < consts::ROWS;
    bool col = c >= 0 && c < consts::COLS;
    return row && col;
}

void Board::syncPos()
{
    for (int r = 0; r < consts::ROWS; ++r)
    {
        for (int c = 0; c < consts::COLS; ++c)
        {
            auto it = pMap.find({r, c});
            if (it != pMap.end())
            {
                gBoard[r][c] = *(it->second);
            }
            else
            {
                gBoard[r][c] = sq;
            }
        }
    }
}

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

void Board::setEval(Pos t, int v)
{
    auto pc = pMap[from];
    auto cap = pMap.count(t) ? pMap[t] : nullptr;

    makeMove(from, t);

    if (isSafe(KK))
    {
        eval[t.first][t.second] = v;
    }

    makeMove(t, from);
    if (cap)
    {
        pMap[t] = cap;
    }
}

bool Board::isSafe(const Piece &p)
{
    int r = static_cast<int>(p.pos.first);
    int c = static_cast<int>(p.pos.second);
    for (auto [dr, dc] : Knight.deltas)
    {
        int nr = r + dr;
        int nc = c + dc;
        if (!inRange({nr, nc}))
        {
            continue;
        }
        if (!pMap[{nr, nc}]->isMe &&
            pMap[{nr, nc}]->identity == KNIGHT)
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

            if (!inRange({nr, nc}))
            {
                break;
            }

            if (pMap[{nr, nc}]->isMe or isEmpty({nr, nc}))
            {
                break;
            }

            if (!pMap[{nr, nc}]->isMe)
            {
                bool isDiag = (dr != 0 && dc != 0);

                if (pMap[{nr, nc}]->identity == QUEEN)
                {
                    return false;
                }

                if (inc == 1 && pMap[{nr, nc}]->identity == KING)
                {
                    return false;
                }

                if (isDiag)
                {
                    if (inc == 1 && pMap[{nr, nc}]->identity == PAWN)
                    {
                        if (dr == -1)
                        {
                            return false;
                        }
                    }
                    if (pMap[{nr, nc}]->identity == BISHOP)
                    {
                        return false;
                    }
                }
                else
                {
                    if (pMap[{nr, nc}]->identity == ROOK)
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
    auto id = pMap[from]->identity;

    switch (id)
    {
    case PAWN:
    {
        int st1 = r - 1;
        if (inRange({st1, c}))
        {
            if (isEmpty({st1, c}))
            {
                setEval({st1, c}, 1);
                int st2 = st1 - 1;
                if (!pMap[from]->isMoved &&
                    isEmpty({st2, c}))
                {
                    setEval({st2, c}, 1);
                }
            }
        }
        goto def;
    }

    case KING:
    {
        if (isSafe(*pMap[from]))
        {
            Pos k, ksc, rkc, qsc, rqc;
            ksc = {consts::CTL_ROW, consts::KSC_COL};
            rkc = {consts::CTL_ROW, consts::RKC_TO};
            qsc = {consts::CTL_ROW, consts::QSC_COL};
            rqc = {consts::CTL_ROW, consts::RQC_TO};
            auto chk = [&](Pos pos) -> bool
            {
                Piece p;
                p.setPos(pos);
                return isEmpty(pos) && isSafe(p);
            };
            if (chk(ksc) and chk(rkc))
            {
                setEval({consts::CTL_ROW, consts::KSC_COL}, 1);
            }
            if (chk(qsc) and chk(rqc))
            {
                setEval({consts::CTL_ROW, consts::QSC_COL}, 1);
            }
        }
    }

    default:
    def:
    {
        for (auto [dr, dc] : pMap[from]->deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            if (inRange({nr, nc}))
            {
                if (!pMap[{nr, nc}]->isMe && !isEmpty({nr, nc}))
                {
                    setEval({nr, nc}, -1);
                }
                if (id == PAWN)
                {
                    if (enpAI.first == nr &&
                        enpAI.second == nc)
                    {
                        setEval({nr, nc}, 1);
                        setEval({r, nc}, -1);
                    }
                    continue;
                }
                if (isEmpty({nr, nc}))
                {
                    setEval({nr, nc}, 1);
                }
                if (pMap[from]->isSliding)
                {
                    int sr = nr + dr;
                    int sc = nc + dc;
                    while (inRange({sr, sc}))
                    {
                        if (isEmpty({sr, sc}))
                        {
                            setEval({sr, sc}, 1);
                        }
                        else
                        {
                            if (!pMap[{nr, nc}]->isMe)
                            {
                                setEval({sr, sc}, -1);
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
