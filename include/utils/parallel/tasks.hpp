#pragma once

#include <fstream>

#include "game/board.hpp"
#include "game/consts.hpp"

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
                // std::string fen = board->genFEN();
                std::string resp;

                /*
                 * Run wrapper.py with arg fen and lestin to resp.
                 *
                 *------------------ Simulate AI thinking ----------------*/
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                {
                    std::ifstream fin("debug.txt");
                    if (fin)
                    {
                        std::getline(fin, resp);
                        auto ltrim = [](std::string &s)
                        {
                            size_t p = 0;
                            while (p < s.size() && std::isspace(
                                static_cast<unsigned char>(s[p])))
                                ++p;
                            if (p)
                                s.erase(0, p);
                        };
                        auto rtrim = [](std::string &s)
                        {
                            while (!s.empty() && std::isspace(
                                static_cast<unsigned char>(s.back())))
                                s.pop_back();
                        };
                        ltrim(resp);
                        rtrim(resp);
                    }
                }
                /*-------------------------------------------------------*/

                char file = resp[0];
                char rank = resp[1];
                size_t xf = static_cast<size_t>(consts::RANK_MAX - rank);
                size_t yf = static_cast<size_t>(file - consts::FILE_MIN);
                file      = resp[2];
                rank      = resp[3];
                size_t xt = static_cast<size_t>(consts::RANK_MAX - rank);
                size_t yt = static_cast<size_t>(file - consts::FILE_MIN);
                char prom = resp.length() > 4 ? resp[4] : '-';
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
