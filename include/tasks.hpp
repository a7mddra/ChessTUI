#pragma once
#include "console.hpp"
#include "constants.hpp"
#include <fstream>

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
        return [board]()
        {
            try
            {
                std::string fen = board->genFEN();
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
                        auto ltrim = [](std::string &s){
                            size_t p = 0;
                            while (p < s.size() && std::isspace(
                                static_cast<unsigned char>(s[p]))) ++p;
                            if (p) s.erase(0, p);
                        };
                        auto rtrim = [](std::string &s){
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
                board->ai = {xt, yt};
                char prom = resp.length() > 4 ? resp[4] : '-';
                auto &pc  = board->gBoard[xf][yf];
                auto id   = pc.identity;

                if (id == PAWN and yf != yt
                    && board->gBoard[xt][yt].identity == SQUARE)
                {
                    board->gBoard[xf][yt] = board->sq;
                }

                board->enpAI = (id == PAWN
                    && std::abs(static_cast<int>(xf) - static_cast<int>(xt)) == 2);

                board->gBoard[xt][yt] = board->gBoard[xf][yf];
                board->gBoard[xf][yf] = board->sq;
                board->gBoard[xt][yt].moved = true;

                if (board->totEMT < board->cntEMT() or id == PAWN)
                {
                    board->hfmvCLK = 0;
                }
                board->totEMT = board->cntEMT();

                if (prom != '-')
                {
                    board->gBoard[xt][yt].set(
                        board, !board->isWhite, board->promos[prom]);
                }

                if (id == KING)
                {
                    if (yf - yt == 2)
                    {
                        board->gBoard[xt][consts::RQC_TO] = board->gBoard[xt][consts::RQC_FR];
                        board->gBoard[xt][consts::RQC_FR] = board->sq;
                        board->gBoard[xt][consts::RQC_TO].moved = true;
                    }
                    else if (yt - yf == 2)
                    {
                        board->gBoard[xt][consts::RKC_TO] = board->gBoard[xt][consts::RKC_FR];
                        board->gBoard[xt][consts::RKC_FR] = board->sq;
                        board->gBoard[xt][consts::RKC_TO].moved = true;
                    }

                }

                board->flmvCNT++;
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
