#include "game/board.hpp"
#include "game/assets.hpp"
#include "game/consts.hpp"
#include "utils/tui/tui.hpp"

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

std::string formatScore(int n)
{
    if (n > 0)
    {
        std::ostringstream os;
        os << '+' << std::setw(2) << std::setfill('0') << n;
        return color::lgreen(os.str());
    }
    return std::string(3, ' ');
}

void Board::printBoard()
{
    using vec = std::vector<std::string>;
    vec pBoard;
    pBoard.reserve(consts::ROWS + 1);

    for (int r = 0; r < consts::ROWS; ++r)
    {
        vec tokens;
        tokens.reserve(consts::COLS);

        for (int c = 0; c < consts::COLS; ++c)
        {
            tokens.push_back(gBoard[r][c].sym + " ");
        }

        std::string row;
        row += std::to_string(consts::ROWS - r) + " ";
        for (auto &t : tokens)
        {
            row += t;
        }
        pBoard.emplace_back(std::move(row));
    }

    std::string tplRow;

    tplRow += formatScore(myScore);
    tplRow += "  " + pBoard[0] + " ";
    tplRow += formatScore(aiScore);
    pBoard[0] = tplRow;
    
    tplRow = "  ";
    for (int c = 0; c < consts::COLS; ++c)
    {
        char file = (consts::FILE_MIN + c);
        tplRow += static_cast<char>(std::toupper(
            static_cast<unsigned char>(file)));
        tplRow += ' ';
    }
    pBoard.emplace_back(tplRow);

    vec myLosses, aiLosses;
    auto genLosses = [&](const vec &src, vec &out)
    {
        out.clear();
        for (size_t i = 0; i + 1 < src.size(); i += 2)
        {
            tplRow = color::lgreen(src[i] + " " + src[i + 1]);
            out.emplace_back(tplRow);
        }
        if (src.size() % 2 == 1)
        {
            tplRow = color::lgreen(src.back() + "  ");
            out.emplace_back(tplRow);
        }
        for (size_t i = out.size(); i < consts::ROWS; ++i)
        {
            out.emplace_back("   ");
        }
    };

    genLosses(myLost, myLosses);
    genLosses(aiLost, aiLosses);

    for (size_t r = 1; r < pBoard.size(); ++r)
    {
        tplRow.clear();
        tplRow += myLosses.size() >= r ? myLosses[r - 1] : "  ";
        tplRow += "  " + pBoard[r] + " ";
        tplRow += aiLosses.size() >= r ? aiLosses[r - 1] : "  ";
        pBoard[r] = tplRow;
    }

    print(pBoard, 'w', true);
    print({"\n"}, 'w', false);
}


void Board::printHeader()
{
    print(assets::header, 'g', true);
    print({"\n"}, 'w', false);
}

void Board::printLog()
{
    print(log, 'r', true);
    print({"\n"}, 'w', false);
}

void Board::printSplash()
{
    console::clearScreen();
    print(assets::splash, 'r', true);
    print({"\n"}, 'w', true);
}

void Board::printContent()
{
    console::clearScreen();
    printHeader();
    printBoard();
}
