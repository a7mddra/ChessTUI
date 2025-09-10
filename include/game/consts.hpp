#pragma once

#include <cstddef>

namespace consts
{
    /* -- Board geometry --*/
    constexpr int ROWS   = 8;
    constexpr int COLS   = 8;
    constexpr int SIZE   = 8*8;

    constexpr int PR_ROW = 0;

    /* ------ Special ------*/
    constexpr int CTL_ROW = 7;
    
    constexpr int RKC_TO  = 5;
    constexpr int KSC_COL = 6;
    constexpr int RKC_FR  = 7;
    
    constexpr int RQC_FR  = 0;
    constexpr int EQC_COL = 1;
    constexpr int QSC_COL = 2;
    constexpr int RQC_TO  = 3;

    /* --- Algebraic bounds ----*/
    constexpr char FILE_MIN = 'a';
    constexpr char FILE_MAX = 'h';
    constexpr char RANK_MIN = '1';
    constexpr char RANK_MAX = '8';

    /* ------- misc -------*/
    constexpr int FPS = 50;
    constexpr int SPN = 50;

    /* ------ Input lens -------*/
    constexpr size_t FL_LEN   = 4;
    constexpr size_t PN_LEN   = 2;
    constexpr size_t PR_LEN   = 1;
    constexpr size_t CRSR_LEN = 2;

    /* ------------------------------ Binary path ---------------------------- */
    const std::string BI_PATH = "src/engine/binary/stockfish-ubuntu-x86-64-avx2";
}
