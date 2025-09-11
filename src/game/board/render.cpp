#include "game/board.hpp"
#include "game/assets.hpp"
#include "game/consts.hpp"
#include "utils/tui/tui.hpp"

using vec = std::vector<std::string>;
const int GWD = consts::COLS + 1;

void Board::spinner()
{
    size_t idx = 0;
    console::saveCursor();
    console::hideCursor();
    while (processing.load())
    {
        {
            cursor = assets::frames[idx];
            cursor = color::blue(cursor);
            console::restCursor();
            printCursor();
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
    if (n == 0)
    {
        return "   ";
    }

    std::ostringstream os;

    if (n > 0)
    {
        os << '+' << std::setw(2) << std::setfill('0') << n;
        return color::lgreen(os.str());
    }
    else
    {
        os << '-' << std::setw(2) << std::setfill('0') << abs(n);
        return color::red(os.str());
    }
}

void Board::printBoard()
{
    vec pBoard;
    pBoard.resize(consts::ROWS + 1);
    
    std::string tplRow;
    std::string score;
    
    score = formatScore(myScore - aiScore);
    int pad = GWD - utf8Width(score + lastAtt);
    tplRow += std::string(pad, ' ');
    tplRow += score;
    tplRow += lastAtt;
    pBoard[0] = tplRow;

    for (size_t r = 0; r < consts::ROWS; ++r)
    {
        vec tokens;
        tokens.reserve(consts::COLS);

        for (size_t c = 0; c < consts::COLS; ++c)
        {
            tokens.push_back(gBoard[r][c].sym);
        }

        std::string row;
        int rank = isWhite? consts::ROWS - r : r + 1;
        row += std::to_string(rank);
        for (size_t c = 0; c < consts::COLS; ++c)
        {
            auto &t = tokens[c];
            auto tg = ((r + c) & 1) != isWhite;
            auto curr = [&](Pos p)
            {
                return p.first == r && p.second == c;
            };

            using CLR = std::function<std::string(std::string)>;
            auto colorize = [&](CLR c1, CLR c2) -> CLR
            {
                return tg ? c1 : c2;
            };

            auto floor = [&](const std::string& s)
            {
                return colorize(color::sq1, color::sq2)(s);
            };

            auto mark = [&](const std::string& s)
            {
                return colorize(color::hsq1, color::hsq2)(s);
            };

            auto check = [&](const std::string& s)
            {
                return colorize(color::rsq1, color::rsq2)(s);
            };

            if (!isSafe(KK) && curr(KK.pos))
            {
                t = check(t);
            }

            if (curr(ffrom) || curr(tto) || curr(from))
            {
                t = mark(t);
            }
            else
            {
                t = floor(t);
            }

            row += t;
        }
        pBoard[r+1] = row;
    }
    
    tplRow = " ";
    for (int c = 0; c < consts::COLS; ++c)
    {
        char filw = consts::FILE_MIN + c;
        char filb = consts::FILE_MAX - c;
        char file = isWhite ? filw : filb;
        tplRow += static_cast<char>(file);
    }
    pBoard.emplace_back(tplRow);

    console::clearScreen();
    print(pBoard, 'w', true);
}

void Board::printCursor()
{
    auto pad = padd(std::string(GWD, ' '));
    std::string cur = std::string(pad, ' ');
    if (!processing.load())
    {
        cursor = color::lgreen(">");
    }
    cur += cursor;
    std::cout << cur;
}
