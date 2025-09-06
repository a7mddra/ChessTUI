#include "game/board.hpp"
#include "utils/parallel/parallel.hpp"
#include "utils/parallel/tasks.hpp"
#include "game/consts.hpp"

bool Board::tryMove()
{
    bool valid = false;
    if (promoting)
    {
        if (input.length() == consts::PR_LEN)
        {
            char ch = static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(input[0])));
            valid = promos.count(ch) != 0;
        }
    }
    else if (state == gst::PENDING)
    {
        valid = (input.length() == consts::PN_LEN);
    }
    else
    {
        valid = (input.length() == consts::PN_LEN ||
                 input.length() == consts::FL_LEN);
    }

    if (!valid)
    {
        setState(gst::ERRINPUT);
        return false;
    }

    auto hanDest = [&](Pos p) -> bool
    {
        auto [x, y] = p;
        auto [r, c] = from;

        if (eval[x][y] == 0)
        {
            setState(gst::ERRMOVE);
            return false;
        }
        reState();

        hfmvCLK++;
        to = p;
        int tmp = cntEmpty();

        switch (pMap[from]->identity)
        {
        case PAWN:
            hfmvCLK = 0;
            if (abs(x - r) == 2)
            {
                char file = static_cast<char>(consts::FILE_MIN + c);
                char rank = static_cast<char>(consts::RANK_MAX - 3);
                enpME = {file, rank};
                break;
            }
            goto def;

        case KING:
            if (abs(y - c) == 2)
            {
                if (y > c)
                {
                    KK.OO = true;
                }
                else
                {
                    KK.OOO = true;
                }
                break;
            }
            [[fallthrough]];

        default:
        def:
            enpME = "-";
            KK.OO = false;
            KK.OOO = false;
            break;
        }

        if (totEmpty < tmp)
        {
            hfmvCLK = 0;
        }
        totEmpty = tmp;
        return true;
    };

    for (size_t i = 0; i < input.length(); i += consts::PN_LEN)
    {
        int low = std::tolower(input[i]);
        char file = static_cast<char>(low);
        char rank = input[i + 1];

        size_t x = static_cast<size_t>(consts::RANK_MAX - rank);
        size_t y = static_cast<size_t>(file - consts::FILE_MIN);
        
        if (!inRange({x, y}))
        {
            setState(gst::ERRINPUT);
            return false;
        }

        if (i == 0)
        {
            if (cell({x, y}) && cell({x, y})->isMe)
            {
                setState(gst::PENDING);
                from = {x, y};
                markValid();
            }
            else
            {
                if (!hanDest({x, y}))
                {
                    return false;
                }
            }
        }
        else
        {
            if (!hanDest({x, y}))
            {
                return false;
            }
        }
    }
    return true;
}

void Board::processMove()
{
    if (promoting)
    {
        auto self = shared_from_this();
        int low = std::tolower(input[0]);
        char ch = static_cast<char>(low);
        pMap[to]->set(to, true, promos[ch]);
        promoting = false;
        syncPos();
        reState();
        return;
    }
    applyMove(to);
}

void Board::applyMove(Pos t)
{
    auto   f  = from;
    size_t xf = f.first;
    size_t yf = f.second;
    size_t xt = t.first;
    size_t yt = t.second;

    switch (pMap[f]->identity)
    {
    case PAWN:
    {
        if (yt != yf && isEmpty(t))
        {
            pMap.erase({xf, yt});
            syncPos();
        }
        if (xt == consts::PR_ROW)
        {
            setState(gst::PROMOTION);
            promoting = true;
        }
        goto def;
    }
    case KING:
    {
        int rf = -1, rt;
        if (KK.OO or kk.OO)
        {
            rf = consts::RKC_FR;
            rt = consts::RKC_TO;
        }
        else if (KK.OOO or kk.OOO)
        {
            rf = consts::RQC_FR;
            rt = consts::RQC_TO;
        }
        if (rf != -1)
        {
            makeMove({xt, rf}, {xt, rt});
        }
        [[fallthrough]];
    }
    default:
    {
    def:
        makeMove(f, t);
        break;
    }
    }
}

void Board::makeMove(Pos f, Pos t)
{
    auto pc = cell(f);

    pMap.erase(f);
    pMap.erase(t);

    pc->setPos(t);
    pc->isMoved = true;
    pMap[t] = pc;

    syncPos();
}
