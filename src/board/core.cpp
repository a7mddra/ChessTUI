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

std::string Board::genFEN()
{
    std::unordered_map<std::string, char> glyph2ch;
    glyph2ch.reserve(assets::pieces.size());
    for (auto const &[k, v] : assets::pieces)
        glyph2ch.emplace(v, k);

    std::string fen;
    for (size_t r = 0; r < consts::ROWS; ++r)
    {
        int emt = 0;
        for (auto &pc : gBoard[r])
        {
            if (pc.identity == SQUARE)
            {
                ++emt;
            }
            else
            {
                if (emt > 0)
                {
                    fen += std::to_string(emt);
                    emt = 0;
                }

                auto it = glyph2ch.find(pc.baseSym);
                if (it != glyph2ch.end())
                    fen += it->second;
            }
        }

        if (emt > 0)
            fen += std::to_string(emt);

        if (r + 1 < consts::ROWS)
            fen += '/';
    }

    fen += isWhite ? " w " : " b ";

    std::string cast;
    if (!KK.isMoved && !R2.isMoved)
        cast += 'K';
    if (!KK.isMoved && !R1.isMoved)
        cast += 'Q';
    if (!kk.isMoved && !r2.isMoved)
        cast += 'k';
    if (!kk.isMoved && !r1.isMoved)
        cast += 'q';
    if (!cast.size())
        cast = "-";

    fen += cast + ' ' + enpME + ' ';

    fen += std::to_string(hfmvCLK) + ' ';
    fen += std::to_string(flmvCNT);

    return fen;
}

int Board::cntEMT()
{
    int cnt = 0;
    for (auto &row : gBoard)
        for (auto &p : row)
            if (p.identity == SQUARE)
                cnt++;
    return cnt;
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
