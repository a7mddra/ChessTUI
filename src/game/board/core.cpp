#include "game/board.hpp"
#include "game/consts.hpp"
#include "utils/parallel/parallel.hpp"
#include "utils/parallel/tasks.hpp"
#include "utils/tui/tui.hpp"

Board::Board()
    : processing(false),
      promoting(false),
      isOver(false) {}

void Board::init()
{
    promos = {{'q', Queen}, {'r', Rook}, {'b', Bishop}, {'n', Knight}};

    {
        Piece* white_back[] = {
            &R1, &N1, &B1, &QQ, &KK, &B2, &N2, &R2 };
        const Piece white_back_tpls[] = {
            Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook };
        for (size_t i = 0; i < consts::COLS; ++i)
            white_back[i]->set({consts::ROWS - 1, i}, true, isWhite, white_back_tpls[i]);
    }

    {
        Piece* white_pawns[] = { 
            &P1, &P2, &P3, &P4, &P5, &P6, &P7, &P8 };
        for (size_t i = 0; i < consts::COLS; ++i)
            white_pawns[i]->set({consts::ROWS - 2, i}, true, isWhite, Pawn);
    }
    
    {
        Piece* black_pawns[] = { 
            &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8 };
        for (size_t i = 0; i < consts::COLS; ++i)
            black_pawns[i]->set({consts::PR_ROW + 1, i}, false, !isWhite, Pawn);
    }

    {
        Piece* black_back[] = { 
            &r1, &n1, &b1, &qq, &kk, &b2, &n2, &r2 };
        const Piece black_back_tpls[] = {
            Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook };
        for (size_t i = 0; i < consts::COLS; ++i)
            black_back[i]->set({consts::PR_ROW, i}, false, !isWhite, black_back_tpls[i]);
    }

    sq.set({-1, -1}, false, false, Square);

    gBoard = std::vector<std::vector<Piece>>(
        consts::ROWS, std::vector<Piece>(consts::COLS, sq));

    if (!isWhite)
    {
        auto tmp = KK.pos;
        KK.setPos(QQ.pos);
        QQ.setPos(tmp);
        tmp = kk.pos;
        kk.setPos(qq.pos);
        qq.setPos(tmp);
    }

    pMap = {
        {P1.pos, &P1}, {P2.pos, &P2}, {P3.pos, &P3}, {P4.pos, &P4},
        {P5.pos, &P5}, {P6.pos, &P6}, {P7.pos, &P7}, {P8.pos, &P8},
        {R1.pos, &R1}, {N1.pos, &N1}, {B1.pos, &B1}, {QQ.pos, &QQ},
        {KK.pos, &KK}, {B2.pos, &B2}, {N2.pos, &N2}, {R2.pos, &R2},
        {p1.pos, &p1}, {p2.pos, &p2}, {p3.pos, &p3}, {p4.pos, &p4},
        {p5.pos, &p5}, {p6.pos, &p6}, {p7.pos, &p7}, {p8.pos, &p8},
        {r1.pos, &r1}, {n1.pos, &n1}, {b1.pos, &b1}, {qq.pos, &qq},
        {kk.pos, &kk}, {b2.pos, &b2}, {n2.pos, &n2}, {r2.pos, &r2}
    };

    syncPos();
    myLost.clear();
    aiLost.clear();
    
    enpAI    = {-1, -1};
    enpME    = "-";
    myScore  = 0;
    aiScore  = 0;
    hfmvCLK  = 0;
    flmvCNT  = 1;
    totEmpty = cntEmpty();

    if (!isWhite)
    {
        auto self = shared_from_this();
        parallel::runParallel(
            tasks::makeSpinner(self),
            tasks::makeAI(self));
    }
}

void Board::readLine()
{
    std::string prompt = assets::cursor;
    size_t dywid;
    switch (state)
    {
    case gst::PROMOTION:
        dywid = consts::PR_LEN;
        break;
    
    case gst::PENDING:
        dywid = consts::PN_LEN;
        break;
    
    default:
        dywid = consts::FL_LEN;
        break;
    }
    prompt += std::string(dywid, ' ');
    std::string line = std::string(
        padd(prompt), ' ');
    line += color::orange(assets::cursor);
    std::cout << line;
    input.clear();
    std::getline(std::cin, input);
    processInput();
}

void Board::processInput()
{
    if (input == "quit")
    {
        setState(gst::EXITING);
        return;
    }

    if (input == "flip")
    {
        isWhite = !isWhite;
        myTurn = isWhite == true;
        init(); 
        return;
    }

    if (input == "new")
    {
        init();
        return;
    }

    /*-------- debug --------*/
    if (input == "fen")
    {
        log.assign({genFEN()});
        return;
    }
    /*-----------------------*/

    if (!tryMove() or isOver)
    {
        return;
    }

    if (state != gst::PENDING)
    {
        processMove();
        calcScore();
        if (state != gst::PROMOTION)
        {    
            auto self = shared_from_this();
            parallel::runParallel(
                tasks::makeSpinner(self),
                tasks::makeAI(self));
        }
    }
}

void Board::render()
{
    auto self = shared_from_this();
    parallel::runParallel(
        tasks::showSplash(self),
        tasks::makeSpinner(self));

    setState(gst::INPUT);
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
