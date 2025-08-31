#include "board.hpp"
#include "tasks.hpp"
#include "parallel.hpp"
#include "assets.hpp"

Board::Board()
    : isWhite(true), promoting(false), processing(false) {}

void Board::init()
{
    promos = {{'q', Queen}, {'r', Rook}, {'b', Bishop}, {'n', Knight}};

    auto self = shared_from_this();

    R1.set(self, 1, Rook);
    N1.set(self, 1, Knight);
    B1.set(self, 1, Bishop);
    QQ.set(self, 1, Queen);
    KK.set(self, 1, King);
    B2.set(self, 1, Bishop);
    N2.set(self, 1, Knight);
    R2.set(self, 1, Rook);
    
    P1.set(self, 1, Pawn);
    P2.set(self, 1, Pawn);
    P3.set(self, 1, Pawn);
    P4.set(self, 1, Pawn);
    P5.set(self, 1, Pawn);
    P6.set(self, 1, Pawn);
    P7.set(self, 1, Pawn);
    P8.set(self, 1, Pawn);

    p1.set(self, 0, Pawn);
    p2.set(self, 0, Pawn);
    p3.set(self, 0, Pawn);
    p4.set(self, 0, Pawn);
    p5.set(self, 0, Pawn);
    p6.set(self, 0, Pawn);
    p7.set(self, 0, Pawn);
    p8.set(self, 0, Pawn);

    r1.set(self, 0, Rook);
    n1.set(self, 0, Knight);
    b1.set(self, 0, Bishop);
    qq.set(self, 0, Queen);
    kk.set(self, 0, King);
    b2.set(self, 0, Bishop);
    n2.set(self, 0, Knight);
    r2.set(self, 0, Rook);

    sq.set(self, 0, Square);

    gBoard = {
        {r1, n1, b1, qq, kk, b2, n2, r2},
        {p1, p2, p3, p4, p5, p6, p7, p8},
        {sq, sq, sq, sq, sq, sq, sq, sq},
        {sq, sq, sq, sq, sq, sq, sq, sq},
        {sq, sq, sq, sq, sq, sq, sq, sq},
        {sq, sq, sq, sq, sq, sq, sq, sq},
        {P1, P2, P3, P4, P5, P6, P7, P8},
        {R1, N1, B1, QQ, KK, B2, N2, R2}};

    reState();
    promoting = false;
    processing.store(false);
}

void Board::setState(gst st)
{
    state = st;
    umarkValid();
    log = assets::logs[st];
}

void Board::render()
{
    auto self = shared_from_this();
    parallel::runParallel(
        tasks::showSplash(self),
        tasks::makeSpinner(self));

    if (isWhite)
    {
        setState(gst::INPUT);
    }
    else
    {
        parallel::runParallel(
            tasks::makeAI(self),
            tasks::makeSpinner(self));
    }

    while (state != gst::EXITING)
    {
        printContent();
        printLog();
        readLine();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(consts::FPS));
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
