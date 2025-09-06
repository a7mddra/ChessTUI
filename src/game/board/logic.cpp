#include "game/board.hpp"
#include "game/consts.hpp"

bool Board::inRange(Pos p) const
{
    auto r = static_cast<int>(p.first);
    auto c = static_cast<int>(p.second);
    bool row = r >= 0 && r < consts::ROWS;
    bool col = c >= 0 && c < consts::COLS;
    return row && col;
}

Piece *Board::cell(Pos p) const
{
    auto it = pMap.find(p);
    return it != pMap.end() ? it->second : nullptr;
}

bool Board::isEmpty(Pos p) const
{
    return cell(p) == nullptr;
}

bool Board::isEnemy(Pos p) const
{
    return cell(p) && !cell(p)->isMe;
}

template <typename F>
void Board::forEachCell(F f)
{
    for (int r = 0; r < consts::ROWS; ++r)
    {
        for (int c = 0; c < consts::COLS; ++c)
        {
            f({r, c});
        }
    }
}

void Board::syncPos()
{
    forEachCell([this](Pos p)
    {
        auto [r, c] = p;
        gBoard[r][c] = cell(p) ? *cell(p) : sq;
    });
}

void Board::syncEval()
{
    forEachCell([this](Pos p)
    {
        auto [r, c] = p;
        gBoard[r][c].setEval(eval[r][c]);
    });
}

void Board::setEval(Pos t, int v)
{
    auto f   = from;
    auto pc  = cell(f);
    auto cap = cell(t);
    
    // simulate
    pMap.erase(f);
    pMap.erase(t);
    pc->setPos(t);
    pMap[t] = pc;
    
    // evaluate
    if (isSafe(KK))
    {
        auto[r, c] = t;
        eval[r][c] = v;
    }

    // undo
    pMap.erase(t);
    pc->setPos(f);
    pMap[f] = pc;
    if (cap)
    {
        pMap[t] = cap;
    }
}

bool Board::isSafe(const Piece &pc)
{
    int r = static_cast<int>(pc.pos.first);
    int c = static_cast<int>(pc.pos.second);
    for (auto [dr, dc] : Knight.deltas)
    {
        int nr = r + dr;
        int nc = c + dc;
        Pos ps = {nr, nc};

        if (!inRange(ps))
        {
            continue;
        }
        if (isEnemy(ps) && cell(ps)->identity == KNIGHT)
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
            Pos ps = {nr, nc};

            if (!inRange(ps))
            {
                break;
            }
            if (isEnemy(ps))
            {
                bool isDiag = (dr != 0 && dc != 0);

                if (cell(ps)->identity == QUEEN)
                {
                    return false;
                }

                if (inc == 1 && cell(ps)->identity == KING)
                {
                    return false;
                }

                if (isDiag)
                {
                    if (inc == 1 && cell(ps)->identity == PAWN)
                    {
                        if (dr == -1)
                        {
                            return false;
                        }
                    }
                    if (cell(ps)->identity == BISHOP)
                    {
                        return false;
                    }
                }
                else
                {
                    if (cell(ps)->identity == ROOK)
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
    auto id = cell(from)->identity;

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
                if (!cell(from)->isMoved && isEmpty({st2, c}))
                {
                    setEval({st2, c}, 1);
                }
            }
        }
        goto def;
    }

    case KING:
    {
        if (isSafe(KK))
        {
            Pos ksc = {consts::CTL_ROW, consts::KSC_COL};
            Pos rkc = {consts::CTL_ROW, consts::RKC_TO};
            Pos qsc = {consts::CTL_ROW, consts::QSC_COL};
            Pos rqc = {consts::CTL_ROW, consts::RQC_TO};

            auto chk = [&](Pos pos) -> bool
            {
                auto [r, c] = pos;
                syncPos();
                Piece pc = gBoard[r][c];
                return isEmpty(pos) && isSafe(pc);
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
        for (auto [dr, dc] : cell(from)->deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            Pos ps = {nr, nc};
            if (inRange(ps))
            {
                if (isEnemy(ps))
                {
                    setEval(ps, -1);
                }
                if (id == PAWN)
                {
                    if (enpAI == ps)
                    {
                        setEval(ps, 1);
                        setEval({r, nc}, -1);
                    }
                    continue;
                }
                if (isEmpty(ps))
                {
                    setEval(ps, 1);
                }
                if (cell(from)->isSliding)
                {
                    ps = {nr + dr, nc + dc};
                    while (inRange(ps))
                    {
                        if (isEmpty(ps))
                        {
                            setEval(ps, 1);
                        }
                        else
                        {
                            if (isEnemy(ps))
                            {
                                setEval(ps, -1);
                            }
                        }
                        ps.first  += dr;
                        ps.second += dc;
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
    using vec = std::vector<int>;
    vec ZEROS = vec(consts::COLS, 0);
    eval.assign(consts::ROWS, ZEROS);
    syncEval();
}
