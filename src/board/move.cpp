#include "board.hpp"
#include "tasks.hpp"
#include "assets.hpp"
#include "colors.hpp"
#include "console.hpp"
#include "parallel.hpp"

void Board::readLine()
{
    std::string line = std::string(padd("$ e2e4"), ' ');
    line += color::orange("$ ");
    std::cout << line;
    std::string input;
    std::getline(std::cin, input);
    processInput(input);
}

void Board::processInput(const std::string &input)
{
    if (input == "quit")
    {
        setState(gst::EXITING);
        return;
    }

    if (input == "flip")
    {
        isWhite = !isWhite;
        init(); 
        return;
    }

    if (input == "new")
    {
        init();
        return;
    }

    if (!tryMove(input))
    {
        return;
    }

    if (state != gst::PENDING)
    {
        processMove(input);
        if (state != gst::PROMOTION)
        {    
            auto self = shared_from_this();
            parallel::runParallel(
                tasks::makeSpinner(self),
                tasks::makeAI(self));
            reState();
        }
    }
}

void Board::processMove(const std::string &input)
{
    if (input.length() == consts::PR_LEN and promoting)
    {
        auto self = shared_from_this();
        char ch = static_cast<char>(
            std::tolower(
                static_cast<unsigned char>(input[0])));
        gBoard[to.first][to.second].set(
            self, isWhite, promos[ch]);
        promoting = false;
        reState();
        return;
    }
    
    Piece tmp = gBoard[from.first][from.second];
    gBoard[from.first][from.second] = sq;

    if (tmp.identity == PAWN)
    {
        auto &[enF, enS] = tmp.enPassant;

        if (enF > -1)
        {
            gBoard[enF][enS] = sq;
        }        
        else if (to.first == consts::PR_ROW)
        {
            setState(gst::PROMOTION);
            promoting = true;
        }

    }
    else if (tmp.identity == KING)
    {
        if (tmp.OO && 
            to.first  == consts::CTL_ROW && 
            to.second == consts::KSC_COL)
        {
            gBoard[consts::CTL_ROW][consts::RKC_FR] = sq;
            gBoard[consts::CTL_ROW][consts::RKC_TO] = r2;
            r2.moved = true;
        }
        else if (tmp.OOO &&
            to.first  == consts::CTL_ROW &&
            to.second == consts::QSC_COL)
        {
            gBoard[consts::CTL_ROW][consts::RQC_FR] = sq;
            gBoard[consts::CTL_ROW][consts::RQC_TO] = r1;
            r1.moved = true;
        }
    }
    tmp.moved = true;
    gBoard[to.first][to.second] = tmp;
}

bool Board::tryMove(const std::string &input)
{
    if (promoting)
    {
        if (input.length() == consts::PR_LEN)
        {
            char ch = static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(input[0])));
            return promos.count(ch) != 0;
        }
        setState(gst::ERRINPUT);
        return false;
    }

    if (input.length() != consts::PN_LEN &&
        input.length() != consts::FL_LEN)
    {
        setState(gst::ERRINPUT);
        return false;
    }

    if (state == gst::PENDING &&
        input.length() != consts::PN_LEN)
    {
        setState(gst::ERRINPUT);
        return false;
    }

    auto hanDest = [&](size_t x, size_t y) -> bool
    {
        if (eval[x][y] == 0)
        {
            setState(gst::ERRMOVE);
            return false;
        }
        to = { x, y };
        reState();
        return true;
    };

    for (size_t i = 0; i < input.length(); i += consts::PN_LEN)
    {
        char rawFile = input[i];
        char file = static_cast<char>(
            std::tolower(
                static_cast<unsigned char>(rawFile)));
        char rank = input[i + 1];

        if (file < consts::FILE_MIN || file > consts::FILE_MAX ||
            rank < consts::RANK_MIN || rank > consts::RANK_MAX)
        {
            setState(gst::ERRINPUT);
            return false;
        }

        size_t x = static_cast<size_t>(consts::RANK_MAX - rank); 
        size_t y = static_cast<size_t>(file - consts::FILE_MIN);  

        if (i == 0)
        {
            if (gBoard[x][y].isme)
            {
                setState(gst::PENDING);
                from = {x, y};
                markValid(from);
            }
            else
            {
                if (!hanDest(x, y))
                    return false;
            }
        }
        else
        {
            if (!hanDest(x, y))
                return false;
        }
    }
    return true;
}
