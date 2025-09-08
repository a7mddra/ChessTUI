#include "game/board.hpp"
#include "game/consts.hpp"
#include "utils/parallel/parallel.hpp"
#include "utils/parallel/tasks.hpp"

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
            valid = promos.count(ch) == 1;
        }
        if (valid)
        {
            return true;
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
        int dbl = 2;
        int tmp = cntEmpty();

        switch (pMap[from]->identity)
        {
        case PAWN:
            hfmvCLK = 0;
            if (abs(x - r) == dbl)
            {
                char filw = consts::FILE_MIN + c;
                char filb = consts::FILE_MAX - c;
                char rnkw = consts::RANK_MIN + dbl;
                char rnkb = consts::RANK_MAX - dbl;
                char file = isWhite ? filw : filb;
                char rank = isWhite ? rnkw : rnkb;
                enpME = {file, rank};
                break;
            }
            goto def;

        case KING:
            if (abs(y - c) == 2)
            {
                KK.OO = (isWhite == (y > c));
                KK.OOO = !KK.OO;
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

        int irank = isWhite ? consts::RANK_MAX - rank : rank - consts::RANK_MIN;
        int ifile = isWhite ? file - consts::FILE_MIN : consts::FILE_MAX - file;
        size_t x = static_cast<size_t>(irank);
        size_t y = static_cast<size_t>(ifile);
        
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
        pMap[to]->set(to, true, isWhite, promos[ch]);
        promoting = false;
        syncPos();
        reState();
        return;
    }
    applyMove(to);
}

void Board::applyMove(Pos t)
{
    auto f = from;
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
            kill({xf, yt});
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
            if (!isWhite)
            {
                rf = revPos(rf);
                rt = revPos(rt);
            }
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

    kill(t);

    pMap.erase(f);
    pMap.erase(t);

    pc->setPos(t);
    pc->isMoved = true;
    pMap[t] = pc;

    syncPos();
}
