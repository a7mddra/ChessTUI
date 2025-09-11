#include "game/piece.hpp" 
#include "game/board.hpp"

static char char_for(Identity id, bool white)
{
    switch (id)
    {
        case PAWN:   return white ? 'P' : 'p';
        case ROOK:   return white ? 'R' : 'r';
        case BISHOP: return white ? 'B' : 'b';
        case KNIGHT: return white ? 'N' : 'n';
        case QUEEN:  return white ? 'Q' : 'q';
        case KING:   return white ? 'K' : 'k';
        default:     return 's';
    }
}

/* -------- Implementation: Piece methods ---------- */

void Piece::set(Pos ps, bool mine, bool white, const Piece &tpl)
{
    ch        = char_for(tpl.identity, white);
    baseSym   = assets::pieces[ch];
    isSliding = tpl.isSliding;
    identity  = tpl.identity;
    deltas    = tpl.deltas;
    score     = tpl.score;
    sym       = baseSym;
    isMoved   = false;
    OO = OOO  = false;
    isMe      = mine;
    pos       = ps;
    eval      = 0;
}

void Piece::setEval(std::shared_ptr<Board> board, Pos p)
{
    auto [r, c] = p;
    eval = board->eval[r][c];
    if (eval == 1)
    {
        auto tg = ((r + c) & 1) != board->isWhite;
        auto dot = assets::pieces['h'];
        sym = tg ? color::gray1(dot) : color::gray2(dot);
    }
    else if (eval == -1)
    {
        sym = color::red(baseSym);
    }
    else
    {
        sym = baseSym;
    }
}

void Piece::setPos(Pos ps)
{
    pos = ps;
}

/* -- Canonical piece templates (color-agnostic) -- */

static Piece makePawnTpl()
{
    Piece p;
    p.identity = PAWN;
    p.isMoved = false;
    p.isSliding = false;
    p.deltas = {{ -1, -1 }, { -1, 1 }}; 
    p.baseSym = assets::pieces['p'];
    p.sym = p.baseSym;
    p.score = 1;
    return p;
}

static Piece makeRookTpl()
{
    Piece p;
    p.identity = ROOK;
    p.isMoved = false;
    p.isSliding = true;
    p.deltas = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1}};
    p.baseSym = assets::pieces['r'];
    p.sym = p.baseSym;
    p.score = 5;
    return p;
}

static Piece makeBishopTpl()
{
    Piece p;
    p.identity = BISHOP;
    p.isMoved = false;
    p.isSliding = true;
    p.deltas = {{ 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}};
    p.baseSym = assets::pieces['b'];
    p.sym = p.baseSym;
    p.score = 3;
    return p;
}

static Piece makeKnightTpl()
{
    Piece p;
    p.identity = KNIGHT;
    p.isMoved = false;
    p.isSliding = false;
    p.deltas = {{ 1,  2}, { 2,  1}, {-1,  2}, {-2,  1},
                { 1, -2}, { 2, -1}, {-1, -2}, {-2, -1}};
    p.baseSym = assets::pieces['n'];
    p.sym = p.baseSym;
    p.score = 3;
    return p;
}

static Piece makeQueenTpl()
{
    Piece p;
    p.identity = QUEEN;
    p.isMoved = false;
    p.isSliding = true;
    p.deltas = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
                { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}};
    p.baseSym = assets::pieces['q'];
    p.sym = p.baseSym;
    p.score = 9;
    return p;
}

static Piece makeKingTpl()
{
    Piece p;
    p.identity = KING;
    p.isMoved = false;
    p.OO = p.OOO = false;
    p.isSliding = false;
    p.deltas = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
                { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}};
    p.baseSym = assets::pieces['k'];
    p.sym = p.baseSym;
    p.score = std::numeric_limits<int>::max();
    return p;
}

static Piece makeSquareTpl()
{ 
    Piece p;
    p.identity = SQUARE;
    p.isMoved = false;
    p.isSliding = false;
    p.baseSym = assets::pieces['s'];
    p.sym = p.baseSym;
    p.score = 0;
    return p;
}

const Piece Pawn   = makePawnTpl();
const Piece Rook   = makeRookTpl();
const Piece Bishop = makeBishopTpl();
const Piece Knight = makeKnightTpl();
const Piece Queen  = makeQueenTpl();
const Piece King   = makeKingTpl();
const Piece Square = makeSquareTpl();
