#include "board.hpp"
#include "colors.hpp"
#include "assets.hpp"
#include "console.hpp"
#include "constants.hpp"

void Board::spinner()
{
    size_t idx = 0;
    console::saveCursor();  
    console::hideCursor();
    while (processing.load())
    {
        {
            std::string tmp = log[0];
            log[0] += assets::frames[idx];
            console::restCursor();
            printLog();
            log[0] = tmp;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(consts::SPN));
        idx = (idx + 1) % assets::frames.size();
    }

    {  
        console::showCursor();
    }
}

void Board::printBoard()
{
    std::vector<std::string> pBoard;
    pBoard.reserve(consts::ROWS +1);

    for (int r = 0; r < consts::ROWS; ++r)
    {
        std::string row = std::to_string(consts::ROWS - r) + " ";
        for (int c = 0; c < consts::COLS; ++c)
        {
            row += gBoard[r][c].sym;
            row += ' ';
        }
        pBoard.emplace_back(row);
    }

    std::string footer = "  ";
    for (int c = 0; c < consts::COLS; ++c)
    {
        footer += char('A' + c);
        footer += ' ';
    }
    pBoard.emplace_back(footer);

    print(pBoard, 'w', true);
    print({"\n"}, 'w', true);
}

void Board::printHeader()
{
    print(assets::header, 'g', true);
    print({"\n"}, 'w', true);
}

void Board::printLog()
{
    print(log, 'r', true);
    print({"\n"}, 'w', true);
}

void Board::printSplash()
{
    clearScreen();
    print(assets::splash, 'r', true);
    print({"\n"}, 'w', true);
}

void Board::printContent()
{
    clearScreen();
    printHeader();
    printBoard();
}
