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
 *  - Movement metadata (deltas, isSliding, score) to help move generation & evaluation.
 *  - Clear, minimal dependencies (assets & colors).
 *
 * Author: a7mddra
 */

#include <string>
#include <vector>
#include <limits>

#include "assets.hpp"
using Pos = std::pair<size_t, size_t>;

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
 *  - `score` encodes static piece value (e.g. 1 pawn, 9 queen).
 */
struct Piece
{
    /* Core identity + display */
    Identity identity = SQUARE;
    std::string baseSym;
    std::string sym;
    Pos pos = {-1, -1};

    /* Stateful chess flags */
    char ch;
    int eval = 0;
    bool isMe = false;                       
    bool isMoved = false;                    
    bool OO = false, OOO = false;            

    /* Movement metadata */
    bool isSliding = false;
    std::vector<std::pair<int,int>> deltas;

    /* Static material value (fixed per canonical template) */
    int score = 0;

    /* --------------------- methods --------------------- */

    Piece() = default;

    /**
     * @brief Initialize this Piece from a template.
     *
     * Copies movement metadata and flags from `tpl`, then applies `mine` color
     * and sets the position `p`. `baseSym` / `sym` pick the correct glyph for color.
     */
    void set(Pos ps, bool mine, bool white, const Piece &tpl);

    /**
     * @brief Set evaluation marker for display.
     * @param v 1 -> valid move (highlight glyph), -1 -> attack (red), 0 -> normal
     */
    void setEval(int v);

    /**
     * @brief Set position of the piece.
     * @param r, c -> row, and col.
     */
    void setPos(Pos ps);
};

/* Canonical templates declared here; definitions live in src/piece.cpp.
 * Each template has its score set once:
 *   Pawn.score = 1, Knight/Bishop = 3, Rook = 5, Queen = 9, King = 1000, Square = 0.
 */
extern const Piece Pawn;
extern const Piece Rook;
extern const Piece Bishop;
extern const Piece Knight;
extern const Piece Queen;
extern const Piece King;
extern const Piece Square;
