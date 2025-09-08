#include "game/board.hpp"
#include "game/consts.hpp"

std::string Board::genFEN()
{
    syncPos();
    Vec ranks;
    std::string fen; 
    std::string cas;
    for (size_t r = 0; r < consts::ROWS; ++r)
    {
        std::string curr;
        int isSq = 0;
        for (auto &pc : gBoard[r])
        {
            if (pc.identity == SQUARE)
            {
                ++isSq;
            }
            else
            {
                if (isSq > 0)
                {
                    curr += std::to_string(isSq);
                    isSq = 0;
                }
                curr += pc.ch;
            }
        }
        if (isSq > 0)
        {
            curr += std::to_string(isSq);
        }
        if (!isWhite)   
        {
            std::reverse(curr.begin(), curr.end());
        }
        ranks.push_back(curr);
    }

    if (!isWhite) 
    {
        std::reverse(ranks.begin(), ranks.end());
    }
    
    for (size_t i = 0; i < consts::ROWS; ++i)
    {
        fen += ranks[i];
        if (i < consts::ROWS - 1)
        {
            fen += '/';
        }
    }
    
    if (isWhite)
    {
        fen += " b";
        if (!KK.isMoved && !R2.isMoved) cas += 'K';
        if (!KK.isMoved && !R1.isMoved) cas += 'Q';
        if (!kk.isMoved && !r2.isMoved) cas += 'k';
        if (!kk.isMoved && !r1.isMoved) cas += 'q';
    }
    else 
    {
        fen += " w";
        if (!kk.isMoved && !r2.isMoved) cas += 'K';
        if (!kk.isMoved && !r1.isMoved) cas += 'Q';
        if (!KK.isMoved && !R2.isMoved) cas += 'k';
        if (!KK.isMoved && !R1.isMoved) cas += 'q';
    }
    
    if (!cas.size())
    {
        cas = " -";
    }

    fen += ' ' + cas + ' ' + enpME;
    fen += ' ' + std::to_string(hfmvCLK);
    fen += ' ' + std::to_string(flmvCNT);

    return fen;
}

std::string parseBestMove()
{
    int moveTime = 1500;
    auto board = std::make_shared<Board>();
    std::string fen = board->genFEN();

    std::stringstream command;
    command << "echo -e 'position fen " << fen << "\\ngo movetime "
            << moveTime << "\\nquit' | " << consts::BI_PATH;

    FILE *pipe = popen(command.str().c_str(), "r");
    if (!pipe)
    {
        throw std::runtime_error("ERROR: popen() failed!");
    }

    std::string bestMove;
    std::array<char, 128> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        std::string line = buffer.data();

        if (line.rfind("bestmove", 0) == 0)
        {
            std::stringstream ss(line);
            std::string keyword;
            ss >> keyword >> bestMove;
            break;
        }
    }

    pclose(pipe);

    if (bestMove.empty())
    {
        throw std::runtime_error("e");
    }

    return bestMove;
}
