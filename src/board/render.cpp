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
    pBoard.reserve(consts::ROWS + 1);

    for (int r = 0; r < consts::ROWS; ++r)
    {
        std::vector<std::string> tokens;
        tokens.reserve(consts::COLS);
        for (int c = 0; c < consts::COLS; ++c)
            tokens.push_back(gBoard[r][c].sym + " ");

        if (!isWhite)
            std::reverse(tokens.begin(), tokens.end());

        std::string row = std::to_string(consts::ROWS - r) + " ";
        for (auto &t : tokens) row += t;
        pBoard.emplace_back(std::move(row));
    }

    std::string footer = "  ";
    for (int c = 0; c < consts::COLS; ++c)
    {
        char file = isWhite ? (consts::FILE_MIN + c)
                            : (consts::FILE_MAX - c);
        footer += static_cast<char>(std::toupper(static_cast<unsigned char>(file)));
        footer += ' ';
    }

    if (!isWhite)
        std::reverse(pBoard.begin(), pBoard.end());

    pBoard.emplace_back(std::move(footer));
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
