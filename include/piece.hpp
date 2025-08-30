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

#include "board.hpp"
#include "assets.hpp"
#include "colors.hpp"

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
    std::pair<int,int> pos = {-1, -1};       ///< row, col (0..7), -1 sentinel allowed
    std::string baseSym;                     ///< original glyph (uncolored)
    std::string sym;                         ///< glyph ready for printing (may be colored)

    /* Stateful chess flags */
    int eval = 0;                            ///< 1 = valid move highlight, -1 = attackable, 0 = none
    bool isme = false;                       ///< valid piece picking 
    bool moved = false;                      ///< has this piece moved (useful for castling/pawn two-step)
    bool OO = false, OOO = false;            ///< king castling rights (temporary flags)
    std::pair<int,int> enPassant = {-1, -1}; ///< valid en-passant capture target for pawn

    /* Movement metadata */
    bool isSliding = false;                  ///< true for Rook/Bishop/Queen
    std::vector<std::pair<int,int>> deltas;  ///< direction offsets (row,col) or knight jumps

    /* --------------------- methods --------------------- */

    Piece() = default;

    /**
     * @brief Initialize this Piece from a template.
     *
     * Copies movement metadata and flags from `tpl`, then applies `white` color
     * and sets the position `p`. `baseSym` / `sym` pick the correct glyph for color.
     */
    void set(std::shared_ptr<Board> board, bool white, std::pair<int,int> p, const Piece &tpl);

    /** Update stored board coordinates. */
    void setPos(int r, int c);

    /**
     * @brief Set evaluation marker for display.
     * @param v 1 -> valid move (highlight glyph), -1 -> attack (red), 0 -> normal
     */
    void setEval(int v);
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
