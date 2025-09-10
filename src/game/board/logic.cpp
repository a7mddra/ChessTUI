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

void Board::kill(Pos p)
{
    if (cell(p))
    {
        lastAtt = cell(p)->baseSym;
    }
}

bool Board::isEmpty(Pos p) const
{
    return cell(p) == nullptr;
}

int Board::cntEmpty()
{
    size_t sz = pMap.size();
    int pcs = static_cast<int>(sz);
    return consts::SIZE - pcs;
}

int Board::revPos(int src)
{
    return consts::ROWS - src - 1;
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
    auto self = shared_from_this();
    forEachCell([this, self](Pos p)
    {
        auto [r, c] = p;
        gBoard[r][c].setEval(self, p);
    });
}

void Board::setEval(Pos t, int v)
{
    auto f = from;
    auto pc = cell(f);
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

        if (!inRange(ps) or !cell(ps))
        {
            continue;
        }
        if (cell(ps)->identity == KNIGHT && !cell(ps)->isMe)
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

            else if (isEmpty(ps))
            {
                inc++;
                continue;
            }

            if (cell(ps)->isMe)
            {
                break;
            }

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
        int st2 = r - 2;
        if (inRange({st1, c}) && isEmpty({st1, c}))
        {
            setEval({st1, c}, 1);
            if (!cell(from)->isMoved &&
                inRange({st2, c}) && isEmpty({st2, c}))
            {
                setEval({st2, c}, 1);
            }
        }
        goto def;
    }

    case KING:
    {
        if (isSafe(KK) and !KK.isMoved)
        {
            Pos ksc = {consts::CTL_ROW, consts::KSC_COL};
            Pos rkc = {consts::CTL_ROW, consts::RKC_TO};
            Pos qsc = {consts::CTL_ROW, consts::QSC_COL};
            Pos rqc = {consts::CTL_ROW, consts::RQC_TO};
            Pos eqc = {consts::CTL_ROW, consts::EQC_COL};

            if (!isWhite)
            {
                ksc.second = revPos(ksc.second);
                rkc.second = revPos(rkc.second);
                qsc.second = revPos(qsc.second);
                rqc.second = revPos(rqc.second);
                eqc.second = revPos(eqc.second);
            }

            auto chk = [&](Pos pos)-> bool
            {
                Piece p;
                p.setPos(pos);
                return isEmpty(pos) && isSafe(p);
            };

            if (!R1.isMoved and chk(qsc) and chk(rqc) and isEmpty(eqc))
            {
                setEval(qsc, 1);
            }
            if (!R2.isMoved and chk(ksc) and chk(rkc))
            {
                setEval(ksc, 1);
            }
        }
        [[fallthrough]];
    }

    default:
    def:
    {
        auto evalu = [&](Pos ps)-> void
        {
            if (isEmpty(ps))
            {
                setEval(ps, 1);
            }
            else if (!cell(ps)->isMe)
            {
                setEval(ps, -1);
            }
        };
        for (auto [dr, dc] : cell(from)->deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            Pos ps = {nr, nc};
            if (inRange(ps))
            {
                if (id == PAWN)
                {
                    if (enpAI == ps)
                    {
                        setEval(ps, 1);
                        setEval({r, nc}, -1);
                    }
                    if (cell(ps) && !cell(ps)->isMe)
                    {
                        setEval(ps, -1);
                    }
                    continue;
                }
                if (cell(from)->isSliding)
                {
                    Pos pi = ps;
                    while (inRange(pi))
                    {
                        evalu(pi);
                        if (!isEmpty(pi))
                        {
                            break;
                        }
                        pi.first += dr;
                        pi.second += dc;
                    }
                    continue;
                }
                evalu(ps);
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

void Board::calcScore()
{
    int myS=0, aiS=0, val;
    auto sum = [&](int &src)-> void
    {
        src += val;
    };
    for (auto [_, pc] : pMap)
    {
        val = pc->score;
        pc->isMe ? sum(myS) : sum(aiS);
    }
    aiScore = std::max(0, aiS - myS);
    myScore = std::max(0, myS - aiS);
}
