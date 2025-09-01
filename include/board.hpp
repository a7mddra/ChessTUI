#pragma once

#include <set>
#include <vector>
#include <string>
#include <clocale>
#include <utility>
#include <numeric>
#include <atomic>
#include <memory>
#include <chrono>
#include <unordered_map>

#include "piece.hpp"

class Board
    : public std::enable_shared_from_this<Board> 
{
public:
    Board();

    bool inBounds(int r, int c) const;
    const Piece &cell(int r, int c) const;
    int cntEMT();

    void init();
    void run();
    void spinner();
    
    void syncEval();
    void printContent();
    void printSplash();
    void printLog();
    
    gst state;
    void reState();
    void setState(gst st);


    std::string genFEN();
    std::string enpME;

    bool isWhite   = true;
    bool promoting = false;
    bool enpAI     = false;

    int hfmvCLK = 0;
    int flmvCNT = 1;
    int totEMT  = 0;

    std::atomic<bool> processing;
    std::vector<std::string> log;

    std::vector<std::vector<int>> eval;

    std::pair<size_t, size_t> from, to, ai;

    std::unordered_map<char, Piece> promos;
    std::vector<std::vector<Piece>> gBoard;

    Piece
        p1, p2, p3, p4, p5, p6, p7, p8,
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
    void printHeader();
    void printBoard();

    void processInput(const std::string &input);
    bool tryMove(const std::string &input);
    void processMove(const std::string &input);
    void markValid(std::pair<size_t, size_t> from);
    void umarkValid();
};
