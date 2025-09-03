#include "board.hpp"
#include "assets.hpp"
#include "constants.hpp"
#include "colors.hpp"
#include "console.hpp"
#include "parallel.hpp"
#include "tasks.hpp"

std::string Board::genFEN()
{
    std::unordered_map<std::string, char> glyph2ch;
    glyph2ch.reserve(assets::pieces.size());
    for (auto const &[k, v] : assets::pieces)
        glyph2ch.emplace(v, k);

    std::string fen;
    for (size_t r = 0; r < consts::ROWS; ++r)
    {
        int emt = 0;
        for (auto &pc : gBoard[r])
        {
            if (pc.identity == SQUARE)
            {
                ++emt;
            }
            else
            {
                if (emt > 0)
                {
                    fen += std::to_string(emt);
                    emt = 0;
                }

                auto it = glyph2ch.find(pc.baseSym);
                if (it != glyph2ch.end())
                    fen += it->second;
            }
        }

        if (emt > 0)
            fen += std::to_string(emt);

        if (r + 1 < consts::ROWS)
            fen += '/';
    }

    fen += isWhite ? " w " : " b ";

    std::string cast;
    if (!KK.isMoved && !R2.isMoved)
        cast += 'K';
    if (!KK.isMoved && !R1.isMoved)
        cast += 'Q';
    if (!kk.isMoved && !r2.isMoved)
        cast += 'k';
    if (!kk.isMoved && !r1.isMoved)
        cast += 'q';
    if (!cast.size())
        cast = "-";

    fen += cast + ' ' + enpME + ' ';

    fen += std::to_string(hfmvCLK) + ' ';
    fen += std::to_string(flmvCNT);

    return fen;
}

int Board::cntEMT()
{
    int cnt = 0;
    for (auto &row : gBoard)
        for (auto &p : row)
            if (p.identity == SQUARE)
                cnt++;
    return cnt;
}

void Board::setState(gst st)
{
    state = st;
    umarkValid();
    log = assets::logs[st];
}

void Board::reState()
{
    // check for check, checkmate, stalemate, draw
    setState(gst::INPUT);
}

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

const Piece &Board::cell(int r, int c) const
{
    if (inBounds(r, c))
    {
        return gBoard[r][c];
    }
    return sq;
}

bool Board::inBounds(int r, int c) const
{
    return r >= 0 && r < consts::ROWS && c >= 0 && c < consts::COLS;
}
