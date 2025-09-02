#include "board.hpp"
#include "tasks.hpp"
#include "assets.hpp"
#include "colors.hpp"
#include "console.hpp"
#include "parallel.hpp"

void Board::readLine()
{
    std::string prompt = assets::cursor;
    prompt += std::string(
         + promoting
        ? consts::PR_LEN
        : consts::FL_LEN, ' ');
    std::string line = std::string(
        padd(prompt), ' ');
    line += color::orange(assets::cursor);
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

    /*debug*/
    if (input == "fen")
    {
        log.assign({genFEN()});
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
        }
    }
}

void Board::applyMove(
    std::pair<size_t, size_t> t, std::pair<size_t, size_t> f)
{
    Piece tmp = gBoard[f.first][f.second];
    gBoard[f.first][f.second] = sq;

    if (tmp.identity == PAWN)
    {
        if (f.second != t.second &&
            gBoard[t.first][t.second].isEmt)
        {
            gBoard[f.first][t.second] = sq;
        }
    }
    else if (tmp.identity == KING)
    {
        if (tmp.OO && 
            t.first  == consts::CTL_ROW && 
            t.second == consts::KSC_COL)
        {
            gBoard[consts::CTL_ROW][consts::RKC_TO] = 
                gBoard[consts::CTL_ROW][consts::RKC_FR];
            gBoard[consts::CTL_ROW][consts::RKC_FR] = sq;
            gBoard[consts::CTL_ROW][consts::RKC_TO].isMoved = true;
            gBoard[consts::CTL_ROW][consts::RKC_TO].setPos(
                {consts::CTL_ROW, consts::RKC_TO});
        }
        else if (tmp.OOO &&
            t.first  == consts::CTL_ROW &&
            t.second == consts::QSC_COL)
        {
            gBoard[consts::CTL_ROW][consts::RQC_TO] = 
                gBoard[consts::CTL_ROW][consts::RQC_FR];
            gBoard[consts::CTL_ROW][consts::RQC_FR] = sq;
            gBoard[consts::CTL_ROW][consts::RQC_TO].isMoved = true;
            gBoard[consts::CTL_ROW][consts::RQC_TO].setPos(
                {consts::CTL_ROW, consts::RQC_TO});
        }
    }
    tmp.isMoved = true;
    tmp.setPos(t);
    gBoard[t.first][t.second] = tmp;
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
            self, to, isWhite, promos[ch]);
        promoting = false;
        reState();
        return;
    }
    if (cell(from.first, from.second).identity == PAWN && 
        to.first == consts::PR_ROW)
    {
        setState(gst::PROMOTION);
        promoting = true;
    }
    applyMove(to, from);
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
        to = {x, y};
        hfmvCLK++;
        reState();
        int tmp = cntEMT();
        auto [r, c] = from;
        auto id = gBoard[r][c].identity;
        bool enp = (id == PAWN && abs(x - r) == 2);
        if (enp)
        {
            enpME = input.length() > consts::PN_LEN
                ? input.substr(2, consts::PN_LEN)
                : input;
            enpME[1] = isWhite
                ? static_cast<char>(
                    static_cast<unsigned char>(enpME[1]) - 1)
                : static_cast<char>(
                    static_cast<unsigned char>(enpME[1]) + 1);
        }
        else
        {
            enpME = "-";
        }
        if (totEMT < tmp or id == PAWN)
        {
            hfmvCLK = 0;
        }
        totEMT = cntEMT();
        return true;
    };

    for (size_t i = 0; i < input.length(); i += consts::PN_LEN)
    {
        char rawFile = input[i];
        char file = static_cast<char>(
            std::tolower(
                static_cast<unsigned char>(rawFile)));
        char rank = input[i + 1];
        
        size_t x = static_cast<size_t>(consts::RANK_MAX - rank); 
        size_t y = static_cast<size_t>(file - consts::FILE_MIN);  

        if (!inBounds(static_cast<int>(x), static_cast<int>(y)))
        {
            setState(gst::ERRINPUT);
            return false;
        }

        if (i == 0)
        {
            if (gBoard[x][y].isMe)
            {
                setState(gst::PENDING);
                from = {x, y};
                markValid();
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
