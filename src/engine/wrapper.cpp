#include "game/board.hpp"
#include "game/consts.hpp"

#include <sys/wait.h>

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

std::string parseBestMove(const std::string &fen)
{
    int moveTimeMs = 1500;
    int to_child[2];    
    int from_child[2];  
    if (pipe(to_child) == -1 || pipe(from_child) == -1)
    {
        throw std::runtime_error("pipe() failed");
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        throw std::runtime_error("fork() failed");
    }

    if (pid == 0)
    {
        dup2(to_child[0], STDIN_FILENO);   
        dup2(from_child[1], STDOUT_FILENO); 
        dup2(from_child[1], STDERR_FILENO); 

        close(to_child[0]); close(to_child[1]);
        close(from_child[0]); close(from_child[1]);

        execl(consts::BI_PATH.c_str(), consts::BI_PATH.c_str(), (char*)NULL);
        _exit(1); 
    }
    
    close(to_child[0]);   
    close(from_child[1]); 

    FILE *engine_in = fdopen(to_child[1], "w");   
    FILE *engine_out = fdopen(from_child[0], "r"); 
    if (!engine_in || !engine_out)
    {
        throw std::runtime_error("fdopen() failed");
    }
    
    auto read_until = [&](const std::string &token)->std::string {
        char buf[512];
        std::string line;
        while (fgets(buf, sizeof(buf), engine_out))
        {
            line = buf;
            
            if (line.find(token)!= std::string::npos)
            {
                return line;
            }
        }
        return std::string();
    };
    
    fprintf(engine_in, "uci\n");
    fflush(engine_in);
    if (read_until("uciok").empty())
    {
        throw std::runtime_error("engine didn't reply uciok");
    }

    fprintf(engine_in, "isready\n");
    fflush(engine_in);
    if (read_until("readyok").empty())
    {
        throw std::runtime_error("engine didn't reply readyok");
    }
    
    fprintf(engine_in, "ucinewgame\n");
    fflush(engine_in);

    fprintf(engine_in, "position fen %s\n", fen.c_str());
    fprintf(engine_in, "go movetime %d\n", moveTimeMs);
    fflush(engine_in);
    
    std::string bestmove;
    char buf[512];
    while (fgets(buf, sizeof(buf), engine_out))
    {
        std::string line = buf;
        
        if (line.rfind("bestmove", 0) == 0)
        {
            std::istringstream ss(line);
            std::string kw;
            ss >> kw >> bestmove;
            break;
        }
    }
    
    fprintf(engine_in, "quit\n");
    fflush(engine_in);

    int status = 0;
    waitpid(pid, &status, 0);

    fclose(engine_in);
    fclose(engine_out);

    if (bestmove.empty())
    {
        throw std::runtime_error("err");
    }
    return bestmove;
}
