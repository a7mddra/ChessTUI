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

bool Board::inBounds(int r, int c) const
{
    return r >= 0 && r < consts::ROWS && c >= 0 && c < consts::COLS;
}

bool Board::isSafe(const Piece &p)
{
    int r = static_cast<int>(p.pos.first);
    int c = static_cast<int>(p.pos.second);

    for (auto [dr, dc] : Knight.deltas)
    {
        int nr = r + dr;
        int nc = c + dc;
        if (inBounds(nr, nc) and !cell(nr, nc).isMe)
        {
            if (cell(nr, nc).identity == KNIGHT)
            {
                return false;
            }
        }
    }

    int inc = 1;
    while(inc < consts::ROWS)
    {
        //up
        if (inBounds(r-inc, c) and !cell(r-inc, c).isMe)
        {
            if (cell(r-inc, c).isSliding &&
                cell(r-inc, c).identity != BISHOP)
            {
                return false;
            }
            if (cell(r-inc, c).identity == KING)
            {
                return false;
            }             
        }

        //down
        if (inBounds(r+inc, c) and !cell(r+inc, c).isMe)
        {
            if (cell(r+inc, c).isSliding &&
                cell(r+inc, c).identity != BISHOP)
            {
                return false;
            }
            if (cell(r+inc, c).identity == KING)
            {
                return false;
            } 
        }

        //left
        if (inBounds(r, c-inc) and !cell(r, c-inc).isMe)
        {
            if (cell(r, c-inc).isSliding &&
                cell(r, c-inc).identity != BISHOP)
            {
                return false;
            }
            if (cell(r, c-inc).identity == KING)
            {
                return false;
            } 
        }

        //right
        if (inBounds(r, c+inc) and !cell(r, c+inc).isMe)
        {
            if (cell(r, c+inc).isSliding &&
                cell(r, c+inc).identity != BISHOP)
            {
                return false;
            }
            if (cell(r, c+inc).identity == KING)
            {
                return false;
            } 
        }

        //up-right
        if (inBounds(r-inc, c+inc) and !cell(r-inc, c+inc).isMe)
        {
            if (cell(r-inc, c+inc).isSliding &&
                cell(r-inc, c+inc).identity != ROOK)
            {
                return false;
            }
            if (isWhite and cell(r-inc, c+inc).identity == PAWN)
            {
                return false;
            }
            if (cell(r-inc, c+inc).identity == KING)
            {
                return false;
            }    
        }

        //up-left
        if (inBounds(r-inc, c-inc) and !cell(r-inc, c-inc).isMe)
        {
            if (cell(r-inc, c-inc).isSliding &&
                cell(r-inc, c-inc).identity != ROOK)
            {
                return false;
            }
            if (isWhite and cell(r-inc, c-inc).identity == PAWN)
            {
                return false;
            }
            if (cell(r-inc, c-inc).identity == KING)
            {
                return false;
            }                
        }

        //down-right
        if (inBounds(r+inc, c+inc) and !cell(r+inc, c+inc).isMe)
        {
            if (cell(r+inc, c+inc).isSliding &&
                cell(r+inc, c+inc).identity != ROOK)
            {
                return false;
            }
            if (!isWhite and cell(r+inc, c+inc).identity == PAWN)
            {
                return false;
            }
            if (cell(r+inc, c+inc).identity == KING)
            {
                return false;
            }            
        }

        //down-left
        if (inBounds(r+inc, c-inc) and !cell(r+inc, c-inc).isMe)
        {
            if (cell(r+inc, c-inc).isSliding &&
                cell(r+inc, c-inc).identity != ROOK)
            {
                return false;
            }
            if (!isWhite and cell(r+inc, c-inc).identity == PAWN)
            {
                return false;
            }
            if (cell(r+inc, c-inc).identity == KING)
            {
                return false;
            }
        }

        inc++;
    }
    return true;
}

const Piece &Board::cell(int r, int c) const
{
    if (inBounds(r, c))
    {
        return gBoard[r][c];
    }
    return sq;
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

void Board::markValid()
{
    int r = static_cast<int>(from.first);
    int c = static_cast<int>(from.second);
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

        for (auto [dr, dc] : cell(r, c).deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            if (inBounds(nr, nc))
            {
                if (!cell(nr, nc).isMe &&
                    !cell(nr, nc).isEmt)
                {
                    setEval(nr, nc, -1);
                }
                if (enpAI and ar == r and ac == nc)
                {
                    setEval(nr, nc, 1);
                    setEval(r, nc, -1);
                }
            }
        }
        break;
    }

    default:
    {
        for (auto [dr, dc] : cell(r, c).deltas)
        {
            int nr = r + dr;
            int nc = c + dc;
            if (inBounds(nr, nc))
            {
                if (cell(nr, nc).isEmt)
                {
                    setEval(nr, nc, 1);
                }
                else if (!cell(nr, nc).isMe)
                {
                    setEval(nr, nc, -1);
                }

            case KNIGHT:
            {
                break;
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
                break;
            }
            
            if (cell(r, c).isSliding)
            {
                int inc = 1;
                while (inBounds(nr + dr * inc, nc + dc * inc))
                {
                    int sr = nr + dr * inc;
                    int sc = nc + dc * inc;
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
                        break;
                    }
                    inc++;
                }
            }
            break;
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
