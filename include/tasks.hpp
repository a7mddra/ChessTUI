#pragma once
#include "console.hpp"
#include "constants.hpp"

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
                
                // send fen then listen to resp
                /*---------------- Simulate AI thinking time --------------*/
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));

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
                    int dir = pc.isWhite ? 1 : -1;
                    board->gBoard[xf + dir][yf] = board->sq;
                }

                board->enpAI = (id == PAWN && abs(xf - xt) == 2);

                board->gBoard[xt][yt] = board->gBoard[xf][yf];
                board->gBoard[xf][yf] = board->sq;


                if (board->totEMT < board->cntEMT() or id == PAWN)
                {
                    board->hfmvCLK = 0;
                }
                board->totEMT = board->cntEMT();

                if (prom != '-')
                {
                    board->gBoard[xt][yt].set(
                        board, board->isWhite, board->promos[prom]);
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

                board->gBoard[xt][yt].moved = true;
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
