#include "piece.hpp"
#include "board.hpp" 

static std::string symbol_for(Identity id, bool white)
{
    switch (id)
    {
        case PAWN:   return assets::pieces[ white ? 'p' : 'P' ];
        case ROOK:   return assets::pieces[ white ? 'r' : 'R' ];
        case BISHOP: return assets::pieces[ white ? 'b' : 'B' ];
        case KNIGHT: return assets::pieces[ white ? 'n' : 'N' ];
        case QUEEN:  return assets::pieces[ white ? 'q' : 'Q' ];
        case KING:   return assets::pieces[ white ? 'k' : 'K' ];
        default:     return assets::pieces['s'];
    }
}

/* ------------ Implementation: Piece methods ------------------ */

void Piece::set(
    std::shared_ptr<Board> board,
    bool white, std::pair<int,int> p, const Piece &tpl)
{
    isme      = tpl.identity != SQUARE and board->isWhite == white;
    baseSym   = symbol_for(tpl.identity, white);
    isSliding = tpl.isSliding;
    identity  = tpl.identity;
    deltas    = tpl.deltas;
    sym       = baseSym;
    enPassant = {-1,-1};
    moved     = false;
    OO = OOO  = false;
    isWhite   = white;
    pos       = p;
    eval      = 0;
}

void Piece::setPos(int r, int c)
{
    pos.first  = r;
    pos.second = c;
}

void Piece::setEval(int v)
{
    eval = v;
    if (eval == 1)
    {
        sym = assets::pieces['h'];
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

/* -- Canonical piece templates (color-agnostic) -- */

static Piece makePawnTpl()
{
    Piece p;
    p.identity = PAWN;
    p.moved = false;
    p.enPassant = {-1,-1};
    p.isSliding = false;
    p.deltas = {{ -1, -1 }, { -1, 1 }}; 
    p.baseSym = assets::pieces.at('p');
    p.sym = p.baseSym;
    return p;
}

static Piece makeRookTpl()
{
    Piece p;
    p.identity = ROOK;
    p.moved = false;
    p.isSliding = true;
    p.deltas = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1}};
    p.baseSym = assets::pieces.at('r');
    p.sym = p.baseSym;
    return p;
}

static Piece makeBishopTpl()
{
    Piece p;
    p.identity = BISHOP;
    p.moved = false;
    p.isSliding = true;
    p.deltas = {{ 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}};
    p.baseSym = assets::pieces.at('b');
    p.sym = p.baseSym;
    return p;
}

static Piece makeKnightTpl()
{
    Piece p;
    p.identity = KNIGHT;
    p.moved = false;
    p.isSliding = false;
    p.deltas = {{ 1,  2}, { 2,  1}, {-1,  2}, {-2,  1},
                { 1, -2}, { 2, -1}, {-1, -2}, {-2, -1}};
    p.baseSym = assets::pieces.at('n');
    p.sym = p.baseSym;
    return p;
}

static Piece makeQueenTpl()
{
    Piece p;
    p.identity = QUEEN;
    p.moved = false;
    p.isSliding = true;
    p.deltas = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
                { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}};
    p.baseSym = assets::pieces.at('q');
    p.sym = p.baseSym;
    return p;
}

static Piece makeKingTpl()
{
    Piece p;
    p.identity = KING;
    p.moved = false;
    p.OO = p.OOO = false;
    p.isSliding = false;
    p.deltas = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
                { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}};
    p.baseSym = assets::pieces.at('k');
    p.sym = p.baseSym;
    return p;
}

static Piece makeSquareTpl()
{
    Piece p;
    p.identity = SQUARE;
    p.moved = false;
    p.isSliding = false;
    p.baseSym = assets::pieces.at('s');
    p.sym = p.baseSym;
    return p;
}

const Piece Pawn   = makePawnTpl();
const Piece Rook   = makeRookTpl();
const Piece Bishop = makeBishopTpl();
const Piece Knight = makeKnightTpl();
const Piece Queen  = makeQueenTpl();
const Piece King   = makeKingTpl();
const Piece Square = makeSquareTpl();
