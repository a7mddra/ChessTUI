#include "game/board.hpp"
#include "engine/engine.hpp"

bool Board::canMove()
{
    std::string fen = genFEN();
    std::string pbm = parseBestMove(fen);
    return pbm != "none" && state != gst::ERRAI;
}

void Board::setState(gst st)
{
    state = st;
    umarkValid();
    log = assets::logs[st];
}

bool Board::insufficientMaterial()
{
    int totN = 0;
    int totB = 0;

    int B = 0;
    int b = 0;
    int cB = -1;
    int cb = -1;

    for (auto &[pos, pc] : pMap)
    {
        if (!pc)
        {
            continue;
        }
        char c = std::tolower(pc->ch);

        if (c == 'p' || c == 'r' || c == 'q')
        {
            return false;
        }

        if (c == 'n')
        {
            ++totN;
        }

        if (c == 'b')
        {
            ++totB;
            auto [r, c] = pos;
            int parity = (r + c) & 1;

            if (pc->isMe)
            {
                ++B;
                if (cB == -1)
                {
                    cB = parity;
                }
            }
            else
            {
                ++b;
                if (cb == -1)
                {
                    cb = parity;
                }
            }
        }
    }

    if (totB == 0 && totN == 0)
    {
        return true;
    }

    if ((totB + totN) == 1)
    {
        return true;
    }

    if (totB == 0 && totN == 2)
    {
        return true;
    }

    if (totB == 2 && totN == 0 && B == 1 && b == 1)
    {
        if (cB != -1 && cb != -1 && cB == cb)
        {
            return true;
        }
    }

    return false;
}

void Board::reState()
{
    auto st = gst::INPUT;
    // bool w = isWhite;

    // if (!canMove())
    // {
    //     if ((w && !isSafe(KK)) || (!w && !isSafe(kk)))
    //     {
    //         st = gst::CHECKMATE;
    //     }
    //     else
    //     {
    //         st = gst::STALEMATE;
    //     }
    // }
    // else if (insufficientMaterial() || hfmvCLK >= 100)
    // {
    //     st = gst::DRAW;
    // }

    setState(st);
}
