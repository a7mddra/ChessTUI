#pragma once

#include <fstream>

#include "game/board.hpp"
#include "game/consts.hpp"
#include "engine/engine.hpp"

namespace tasks
{
    inline std::function<void()> makeSpinner(
        std::shared_ptr<Board> board)
    {
        board->processing.store(true);
        return [board]()
        {
            board->spinner();
        };
    }

    inline std::function<void()> showSplash(
        std::shared_ptr<Board> board)
    {
        board->printSplash();
        return [board]()
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(consts::SPL));
            board->processing.store(false);
            board->reState();
        };
    }

    inline std::function<void()> makeAI(
        std::shared_ptr<Board> board)
    {
        board->setState(gst::THINKING);
        board->printContent();
        board->myTurn = false;
        return [board]()
        {
            try
            {
                auto out  = parseBestMove();
                char file = out[0];
                char rank = out[1];
                size_t xf = static_cast<size_t>(consts::RANK_MAX - rank);
                size_t yf = static_cast<size_t>(file - consts::FILE_MIN);
                file      = out[2];
                rank      = out[3];
                size_t xt = static_cast<size_t>(consts::RANK_MAX - rank);
                size_t yt = static_cast<size_t>(file - consts::FILE_MIN);
                char prom = out.length() > 4 ? out[4] : '-';
                auto &pc  = board->pMap[{xf, yf}];
                auto id   = pc->identity;
                auto tmp  = board->cntEmpty();

                board->enpAI  = {-1, -1};
                board->KK.OO  = false;
                board->KK.OOO = false;


                if (id == PAWN)
                {
                    int s1 = static_cast<int>(xf);
                    int s2 = static_cast<int>(xt);
                    if (abs(s1 - s2) == 2)
                    {
                        board->enpAI.first = xt - 1;
                        board->enpAI.second = yt;
                    }
                    board->hfmvCLK = 0;
                }
                else if (id == KING)
                {
                    if (yt - yf == 2)
                    {
                        board->kk.OO = true;
                    }
                    else if (yf - yt == 2)
                    {
                        board->kk.OOO = true;
                    }
                }

                Pos to = {xt, yt};
                board->from = {xf, yf};
                board->applyMove(to);

                if (board->totEmpty < tmp)
                {
                    board->hfmvCLK = 0;
                }
                board->totEmpty = tmp;

                if (prom != '-')
                {
                    auto pc = board->promos[prom];
                    board->pMap[to]->set(to, false, !board->isWhite, pc);
                    board->syncPos();
                }

                board->from   = {-1, -1};
                board->kk.OO  = false;
                board->kk.OOO = false;
                board->myTurn = true;
                board->flmvCNT++;
                board->calcScore();
                board->reState();
            }
            catch (const std::exception &e)
            {
                board->setState(gst::ERRAI);
            }
            board->processing.store(false);
        };
    }
}
