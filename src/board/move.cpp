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
        init(isWhite); 
        return;
    }

    if (input == "new")
    {
        init(isWhite);
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
        gBoard[to.first][to.second].set(
            isWhite, {consts::PR_ROW, to.second}, promos[input[0]]);
        promoting = false;
        reState();
        return;
    }
    
    Piece tmp;
    tmp = gBoard[from.first][from.second];
    gBoard[from.first][from.second] = sq;
    tmp.setPos(to.first, to.second);
    if (tmp.identity == PAWN)
    {
        if (to.first == consts::PR_ROW)
        {
            setState(gst::PROMOTION);
            promoting = true;
        }
        else if (tmp.enPassant.first != -1)
        {
            gBoard[tmp.enPassant.first][tmp.enPassant.second] = sq;
        }
    }
    else if (tmp.identity == KING)
    {
        if (tmp.OO and to.first == consts::CTL_ROW and to.second == consts::KSC_COL)
        {
            gBoard[consts::CTL_ROW][consts::RKC_FR] = sq;
            gBoard[consts::CTL_ROW][consts::RKC_TO] = r2;
            r2.setPos(consts::CTL_ROW, consts::RKC_TO);
            r2.moved = true;
        }
        else if (tmp.OOO and to.first == consts::CTL_ROW and to.second == consts::QSC_COL)
        {
            gBoard[consts::CTL_ROW][consts::RQC_FR] = sq;
            gBoard[consts::CTL_ROW][consts::RQC_TO] = r1;
            r1.setPos(consts::CTL_ROW, consts::RQC_TO);
            r1.moved = true;
        }
    }
    tmp.moved = true;
    gBoard[to.first][to.second] = tmp;
}

bool Board::tryMove(const std::string &input)
{
    if (promoting == true)
    {
        if (promos.count(tolower(input[0])) and input.length() == consts::PR_LEN)
            return true;
        return false;
    }

    if (input.length() != consts::PN_LEN and input.length() != consts::FL_LEN)
    {
        setState(gst::ERRINPUT);
        return false;
    }

    if (state == gst::PENDING and input.length() != consts::PN_LEN)
    {
        setState(gst::ERRINPUT);
        return false;
    }

    for (size_t i = 0; i < input.length(); i += consts::PN_LEN)
    {
        if (input[i] > consts::FILE_MAX or input[i] < consts::FILE_MIN)
        {
            setState(gst::ERRINPUT);
            return false;
        }
        if (input[i + 1] > consts::RANK_MAX or input[i + 1] < consts::RANK_MIN)
        {
            setState(gst::ERRINPUT);
            return false;
        }

        size_t x = consts::RANK_MAX - input[i + 1];
        size_t y = input[i] - consts::FILE_MIN;

        if (i < consts::PN_LEN and state != gst::PENDING)
        {
            from = {x, y};
            markValid(from);
            setState(gst::PENDING);
        }
        else
        {
            to = {x, y};
            if (eval[x][y] == 0)
            {
                setState(gst::ERRMOVE);
            }
            umarkValid();
        }
    }
    return true;
}
