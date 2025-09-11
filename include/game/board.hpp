#pragma once

#include <set>
#include <array>    
#include <memory>
#include <atomic>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>   
#include <sstream>
#include <utility>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <optional>
#include <unordered_map>
#include <functional>

#include "piece.hpp"

using Pos = std::pair<size_t, size_t>;
using Vec = std::vector<std::string>;

struct PosHash {
    size_t operator()(Pos const& k) const noexcept
    {
        auto k1 = std::hash<size_t>{}(k.first) << 1;
        auto k2 = std::hash<size_t>{}(k.second);
        return k1 ^ k2;
    }
};

class Board
    : public std::enable_shared_from_this<Board>
    {
public:
    Board();

    void init();
    void run();
    void spinner();

    void calcScore();
    void syncPos();
    void syncEval();
    void setEval(Pos t, int v);
    std::vector<std::vector<int>> eval;
    
    void kill(Pos p);
    void applyMove(Pos t);
    void makeMove(Pos f, Pos t);
    
    gst state;
    void reState();
    void printBoard();
    void setState(gst st);
    
    std::string genFEN();
    std::string enpME;
    int cntEmpty();
    int revPos(int src);
    int totEmpty;
    int hfmvCLK;
    int flmvCNT;
    int myScore;
    int aiScore;
    
    std::string cursor;
    std::atomic<bool> processing;
    bool promoting, isOver, isWhite, myTurn;

    Pos from, to, ffrom, tto, enpAI;
    Piece* cell(Pos p) const;
    std::optional<char> stagedPromo;
    std::unordered_map<char, Piece> promos;
    std::vector<std::vector<Piece>> gBoard;
    std::unordered_map<Pos, Piece*, PosHash> pMap;
    
    bool isEmpty(Pos p) const;
    bool inRange(Pos p) const;
    
    template <typename F>
    void forEachCell(F f);
    
    bool isSafe(const Piece &pc);
    Piece p1, p2, p3, p4, p5, p6, p7, p8,
    P1, P2, P3, P4, P5, P6, P7, P8,
    r1, r2, R1, R2,
    n1, n2, N1, N2,
    b1, b2, B1, B2,
    qq, QQ,
    kk, KK,
    sq;
    
    private:
    void render();
    void readLine();
    void printCursor();
    
    std::string input;
    std::string lastAtt;
    void processInput();
    bool tryMove();
    void processMove();
    void markValid();
    void umarkValid();
    
    bool canMove();
    bool insufficientMaterial();
};
