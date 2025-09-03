#include "board.hpp"
#include "tasks.hpp"
#include "parallel.hpp"
#include "assets.hpp"

Board::Board()
    : isWhite(true), promoting(false),
      processing(false) {}

void Board::init()
{
    promos = {{'q', Queen}, {'r', Rook}, {'b', Bishop}, {'n', Knight}};
    auto self = shared_from_this();
    {
        Piece* white_back[] = {
            &R1, &N1, &B1, &QQ, &KK, &B2, &N2, &R2 };
        const Piece white_back_tpls[] = {
            Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook };
        for (size_t i = 0; i < consts::COLS; ++i)
            white_back[i]->set(
                self, {consts::ROWS - 1, i}, true, white_back_tpls[i]);
    }

    {
        Piece* white_pawns[] = { 
            &P1, &P2, &P3, &P4, &P5, &P6, &P7, &P8 };
        for (size_t i = 0; i < consts::COLS; ++i)
            white_pawns[i]->set(
                self, {consts::ROWS - 2, i}, true, Pawn);
    }
    
    {
        Piece* black_pawns[] = { 
            &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8 };
        for (size_t i = 0; i < consts::COLS; ++i)
            black_pawns[i]->set(
                self, {consts::PR_ROW + 1, i}, false, Pawn);
    }

    {
        Piece* black_back[] = { 
            &r1, &n1, &b1, &qq, &kk, &b2, &n2, &r2 };
        const Piece black_back_tpls[] = {
            Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook };
        for (size_t i = 0; i < consts::COLS; ++i)
            black_back[i]->set(
                self, {consts::PR_ROW, i}, false, black_back_tpls[i]);
    }
    
    sq.set(self, {-1, -1}, false, Square);
    
    gBoard = {
        { r1, n1, b1, qq, kk, b2, n2, r2 },   
        { p1, p2, p3, p4, p5, p6, p7, p8 },   
        { sq, sq, sq, sq, sq, sq, sq, sq },   
        { sq, sq, sq, sq, sq, sq, sq, sq },   
        { sq, sq, sq, sq, sq, sq, sq, sq },   
        { sq, sq, sq, sq, sq, sq, sq, sq },   
        { P1, P2, P3, P4, P5, P6, P7, P8 },   
        { R1, N1, B1, QQ, KK, B2, N2, R2 }    
    };

    reState();
    enpME = "-";
    hfmvCLK = 0;
    flmvCNT = 1;
    totEMT = cntEMT();

    if (!isWhite)
    {
        parallel::runParallel(
            tasks::makeSpinner(self),
            tasks::makeAI(self));
    }
}

void Board::render()
{
    auto self = shared_from_this();
    parallel::runParallel(
        tasks::showSplash(self),
        tasks::makeSpinner(self));

    while (state != gst::EXITING)
    {
        printContent();
        printLog();
        readLine();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                consts::FPS));
    }

    parallel::runParallel(
        tasks::showSplash(self),
        tasks::makeSpinner(self));
}

void Board::run()
{
    setlocale(LC_ALL, "");
    setState(gst::LOADING);
    render();
    exit(0);
}
