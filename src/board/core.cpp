#include "board.hpp"
#include "tasks.hpp"
#include "parallel.hpp"
#include "assets.hpp"

Board::Board()
    : isWhite(true), promoting(false)
{
    init(isWhite);
}

void Board::init(bool white)
{
    isWhite = white;
    promos = {{'q', Queen}, {'r', Rook}, {'b', Bishop}, {'n', Knight}};
    
    auto self = shared_from_this();

    N1.set(self, isWhite, {7, 1}, Knight);
    N2.set(self, isWhite, {7, 6}, Knight);
    B1.set(self, isWhite, {7, 2}, Bishop);
    B2.set(self, isWhite, {7, 5}, Bishop);
    QQ.set(self, isWhite, {7, 3}, Queen);
    KK.set(self, isWhite, {7, 4}, King);
    R1.set(self, isWhite, {7, 0}, Rook);
    R2.set(self, isWhite, {7, 7}, Rook);
    P1.set(self, isWhite, {6, 0}, Pawn);
    P2.set(self, isWhite, {6, 1}, Pawn);
    P3.set(self, isWhite, {6, 2}, Pawn);
    P4.set(self, isWhite, {6, 3}, Pawn);
    P5.set(self, isWhite, {6, 4}, Pawn);
    P6.set(self, isWhite, {6, 5}, Pawn);
    P7.set(self, isWhite, {6, 6}, Pawn);
    P8.set(self, isWhite, {6, 7}, Pawn);

    n1.set(self, !isWhite, {0, 1}, Knight);
    n2.set(self, !isWhite, {0, 6}, Knight);
    b1.set(self, !isWhite, {0, 2}, Bishop);
    b2.set(self, !isWhite, {0, 5}, Bishop);
    qq.set(self, !isWhite, {0, 3}, Queen);
    kk.set(self, !isWhite, {0, 4}, King);
    r1.set(self, !isWhite, {0, 0}, Rook);
    r2.set(self, !isWhite, {0, 7}, Rook);
    p1.set(self, !isWhite, {1, 0}, Pawn);
    p2.set(self, !isWhite, {1, 1}, Pawn);
    p3.set(self, !isWhite, {1, 2}, Pawn);
    p4.set(self, !isWhite, {1, 3}, Pawn);
    p5.set(self, !isWhite, {1, 4}, Pawn);
    p6.set(self, !isWhite, {1, 5}, Pawn);
    p7.set(self, !isWhite, {1, 6}, Pawn);
    p8.set(self, !isWhite, {1, 7}, Pawn);

    sq.set(self, false, {-1, -1}, Square);

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
    umarkValid();
    pending = false;
    promoting = false;
    processing.store(false);
}

void Board::setState(gst st)
{
    state = st;
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
