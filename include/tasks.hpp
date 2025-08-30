#pragma once
#include "console.hpp"
#include "constants.hpp"

namespace tasks
{
    inline std::function<void()> makeSpinner(std::shared_ptr<Board> board)
    {
        board->processing.store(true);
        return [board]()
        {
            board->spinner();
        };
    }

    inline std::function<void()> showSplash(std::shared_ptr<Board> board)
    {
        board->printSplash();
        return [board]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(consts::SPL));
            board->processing.store(false);
        };
    }

    inline std::function<void()> makeAI(std::shared_ptr<Board> board)
    {
        board->setState(gst::THINKING);
        board->printContent();
        return [board]()
        {
            try
            {
                /*   -----------Simulate AI thinking time------------      */
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                board->gBoard[3][4] = board->gBoard[1][4];
                board->gBoard[1][4] = board->sq;
                board->eval[0][0] = -1;
                board->reState();
                board->refBoard();
            }
            catch (const std::exception)
            {
                board->setState(gst::ERRAI);
            }
            board->processing.store(false);
        };
    }
}
