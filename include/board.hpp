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

class Board : public std::enable_shared_from_this<Board> 
{
public:
    Board();
    gst state;
    void run();
    void init();
    void spinner();
    void reState();
    void syncEval();
    void printLog();
    void printSplash();
    void printContent();
    void setState(gst st);
    bool isWhite, promoting;
    std::atomic<bool> processing;
    std::vector<std::string> log;
    std::pair<size_t, size_t> from, to;
    std::vector<std::vector<int>> eval;
    const Piece &cell(int r, int c) const;
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
    void umarkValid();
    void printBoard();
    void printHeader();
    bool tryMove(const std::string &input);
    void processMove(const std::string &input);
    void processInput(const std::string &input);
    void markValid(std::pair<size_t, size_t> from);
};
