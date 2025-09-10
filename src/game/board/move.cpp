#include "game/board.hpp"
#include "game/consts.hpp"
#include "utils/parallel/parallel.hpp"
#include "utils/parallel/tasks.hpp"

bool Board::tryMove()
{
    if (promoting)
    {
        if (input.length() == consts::PR_LEN)
        {
            char ch = static_cast<char>(
                std::tolower(static_cast<unsigned char>(input[0])));
            if (promos.count(ch) == 1)
            {
                return true;
            }
        }
        setState(gst::ERRINPUT);
        return false;
    }

    const size_t L     = input.length();
    const bool isPN    = (L == consts::PN_LEN);
    const bool isFL    = (L == consts::FL_LEN);
    const bool isPN_PR = (L == consts::PN_LEN + consts::PR_LEN);
    const bool isFL_PR = (L == consts::FL_LEN + consts::PR_LEN);

    if (!(isPN || isFL || isPN_PR || isFL_PR))
    {
        setState(gst::ERRINPUT);
        return false;
    }

    auto parsePos = [&](size_t offset) -> std::optional<Pos>
    {
        if (offset + 1 >= input.length())
        {
            return std::nullopt;
        }
        int low = std::tolower(static_cast<unsigned char>(input[offset]));
        char file = static_cast<char>(low);
        char rank = input[offset + 1];

        int irank = isWhite ? consts::RANK_MAX - rank : rank - consts::RANK_MIN;
        int ifile = isWhite ? file - consts::FILE_MIN : consts::FILE_MAX - file;

        Pos p = { static_cast<size_t>(irank), static_cast<size_t>(ifile) };
        if (!inRange(p))
        {
            return std::nullopt;
        }
        return p;
    };

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
            {
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
            }
            case KING:
            {
                if (abs(y - c) == 2)
                {
                    KK.OO = (isWhite == (y > c));
                    KK.OOO = !KK.OO;
                    break;
                }
            }
            [[fallthrough]];
            default:
            {
            def:
                enpME = "-";
                KK.OO = false;
                KK.OOO = false;
                break;
            }
        }

        if (totEmpty < tmp)
        {
            hfmvCLK = 0;
        }
        totEmpty = tmp;
        return true;
    };

    stagedPromo.reset();

    auto pos1_opt = parsePos(0);
    if (!pos1_opt)
    {
        setState(gst::ERRINPUT);
        return false;
    }
    Pos pos1 = *pos1_opt;

    auto wouldBePromotion = [&](const Pos &src, const Pos &dst) -> bool
    {
        if (!inRange(src) || !inRange(dst))
        {
            return false;
        }
        if (!pMap.count(src))
        {
            return false;
        }
        if (pMap[src]->identity != PAWN)
        {
            return false;
        }
        return (dst.first == static_cast<size_t>(consts::PR_ROW));
    };

    if (state == gst::PENDING)
    {
        if (isPN)
        {
            if (cell(pos1) && cell(pos1)->isMe)
            {
                setState(gst::PENDING);
                from = pos1;
                markValid();
                return true;
            }
            return hanDest(pos1);
        }

        if (isPN_PR)
        {
            char ch = static_cast<char>(
                std::tolower(static_cast<unsigned char>(input[consts::PN_LEN])));
            if (promos.count(ch) == 0)
            {
                setState(gst::ERRINPUT);
                return false;
            }

            if (!hanDest(pos1))
            {
                return false;
            }

            if (!wouldBePromotion(from, pos1))
            {
                setState(gst::ERRINPUT);
                return false;
            }

            stagedPromo = ch;
            return true;
        }

        if (isFL || isFL_PR)
        {
            auto pos2_opt = parsePos(consts::PN_LEN);
            if (!pos2_opt)
            {
                setState(gst::ERRINPUT);
                return false;
            }
            Pos pos2 = *pos2_opt;

            if (!(cell(pos1) && cell(pos1)->isMe))
            {
                setState(gst::ERRINPUT);
                return false;
            }

            setState(gst::PENDING);
            from = pos1;
            markValid();

            if (!hanDest(pos2))
            {
                return false;
            }

            if (isFL_PR)
            {
                char ch = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(input[consts::FL_LEN])));
                if (promos.count(ch) == 0)
                {
                    reState();
                    setState(gst::ERRINPUT);
                    return false;
                }
                if (!wouldBePromotion(from, pos2))
                {
                    reState();
                    setState(gst::ERRINPUT);
                    return false;
                }
                stagedPromo = ch;
            }
            return true;
        }
    }
    else
    {
        if (cell(pos1) && cell(pos1)->isMe)
        {
            setState(gst::PENDING);
            from = pos1;
            markValid();

            if (isFL || isFL_PR)
            {
                auto pos2_opt = parsePos(consts::PN_LEN);
                if (!pos2_opt)
                {
                    reState();
                    setState(gst::ERRINPUT);
                    return false;
                }
                Pos pos2 = *pos2_opt;

                if (!hanDest(pos2))
                {
                    return false;
                }

                if (isFL_PR)
                {
                    char ch = static_cast<char>(
                        std::tolower(static_cast<unsigned char>(input[consts::FL_LEN])));
                    if (promos.count(ch) == 0)
                    {
                        reState();
                        setState(gst::ERRINPUT);
                        return false;
                    }
                    if (!wouldBePromotion(from, pos2))
                    {
                        reState();
                        setState(gst::ERRINPUT);
                        return false;
                    }
                    stagedPromo = ch;
                }
            }
            return true;
        }
        else
        {
            setState(gst::ERRINPUT);
            return false;
        }
    }

    setState(gst::ERRINPUT);
    return false;
}

void Board::processMove()
{
    if (promoting && !stagedPromo.has_value())
    {
        int low = std::tolower(static_cast<unsigned char>(input[0]));
        char ch = static_cast<char>(low);
        if (promos.count(ch) == 0)
        {
            setState(gst::ERRINPUT);
            return;
        }
        pMap[to]->set(to, true, isWhite, promos[ch]);
        promoting = false;
        syncPos();
        reState();
        return;
    }

    applyMove(to);

    if (promoting && stagedPromo.has_value())
    {
        char ch = stagedPromo.value();
        stagedPromo.reset();

        int low = std::tolower(static_cast<unsigned char>(ch));
        char cch = static_cast<char>(low);
        if (promos.count(cch) == 0)
        {
            setState(gst::ERRINPUT);
            promoting = false;
            return;
        }

        pMap[to]->set(to, true, isWhite, promos[cch]);
        promoting = false;
        syncPos();
        reState();
        return;
    }
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
        }
        [[fallthrough]];
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
