#pragma once
/**
 * @file piece.hpp
 * @brief Piece data structure and canonical piece templates for the terminal chess engine.
 *
 * This header exposes the `Piece` struct used throughout the project to represent
 * board squares and chess pieces. It also declares canonical piece templates
 * (Pawn, Rook, Bishop, Knight, Queen, King, Square) which are defined in
 * `src/piece.cpp`. Use the templates with `Piece::set(...)` to initialize an
 * instance with the template's metadata while applying color/position.
 *
 * Design goals:
 *  - Small, self-contained POD-like struct with helpers (set, setPos, setEval).
 *  - Movement metadata (deltas, isSliding) to help move generation in `markValid`.
 *  - Clear, minimal dependencies (assets & colors).
 *
 * Author: a7mddra
 */

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "assets.hpp"
#include "colors.hpp"

class Board;

enum Identity
{
    PAWN,
    ROOK,
    BISHOP,
    KNIGHT,
    QUEEN,
    KING,
    SQUARE
};

/**
 * @brief Lightweight representation of a chess piece or empty square.
 *
 * Field notes:
 *  - `pos` uses signed ints so sentinel values like -1 are easy.
 *  - `baseSym` stores the unmodified glyph, `sym` stores what should be printed
 *    (e.g. colored or highlighted).
 *  - `deltas` and `isSliding` encode movement semantics useful for `markValid`.
 */
struct Piece
{
    /* Core identity + display */
    Identity identity = SQUARE;
    bool isWhite = false;
    bool isEmt = true;
    std::string baseSym;                     
    std::string sym;
    std::pair<size_t, size_t> pos = {-1, -1};                         

    /* Stateful chess flags */
    int eval = 0;                            
    bool isMe = false;                       
    bool isMoved = false;                    
    bool OO = false, OOO = false;            

    /* Movement metadata */
    bool isSliding = false;                  
    std::vector<std::pair<int,int>> deltas;  

    /* --------------------- methods --------------------- */

    Piece() = default;

    /**
     * @brief Initialize this Piece from a template.
     *
     * Copies movement metadata and flags from `tpl`, then applies `white` color
     * and sets the position `p`. `baseSym` / `sym` pick the correct glyph for color.
     */
    void set(std::shared_ptr<Board> board,
         std::pair<size_t, size_t> ps,
         bool white, const Piece &tpl);

    /**
     * @brief Set evaluation marker for display.
     * @param v 1 -> valid move (highlight glyph), -1 -> attack (red), 0 -> normal
     */
    void setEval(int v);

    /**
     * @brief Set position of the piece.
     * @param r, c -> row, and col.
     */
    void setPos(std::pair<size_t, size_t> ps);
};

/* Canonical templates declared here; definitions live in src/piece.cpp.
 * Use these templates in Board::init:
 *
 *   p1.set(isWhite, {6,0}, Pawn);
 *   promos = {{'q', Queen}, {'r', Rook}, ... };
 */
extern const Piece Pawn;
extern const Piece Rook;
extern const Piece Bishop;
extern const Piece Knight;
extern const Piece Queen;
extern const Piece King;
extern const Piece Square;
