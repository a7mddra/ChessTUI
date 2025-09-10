#pragma once

/**
 * @file piece.hpp
 * @brief Piece data structure and canonical piece templates for the terminal chess engine.
 *
 * Lightweight POD-like representation of a board square or chess piece and
 * declarations for canonical templates (Pawn, Rook, Bishop, Knight, Queen, King, Square).
 *
 * Use the canonical templates with `Piece::set(...)` to initialize a runtime
 * piece instance (this applies color/position to a color-agnostic template).
 *
 * Author: a7mddra
 */

#include <string>
#include <vector>
#include <limits>
#include <memory>

#include "assets.hpp"

using Pos = std::pair<size_t, size_t>;

class Board;

/**
 * @enum Identity
 * @brief Type identity for a Piece.
 *
 * Values distinguish pawns, rooks, bishops, knights, queens, kings and
 * empty board squares (SQUARE).
 */
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
 * @struct Piece
 * @brief Compact representation for a piece or empty square.
 *
 * Fields contain display glyphs, runtime flags, movement metadata and a
 * small evaluation marker used by the renderer and move generator.
 *
 * Intended to be small and copyable. Canonical templates supply the
 * movement metadata and static values (score, deltas, isSliding).
 */
struct Piece
{
    /* identity and display */
    Identity identity = SQUARE; /**< piece identity (PAWN, ROOK, ... or SQUARE) */
    std::string baseSym;        /**< glyph for the piece (unmodified) */
    std::string sym;            /**< glyph to print (may be colored/highlighted) */
    Pos pos = {-1, -1};         /**< current position (row, col) or {-1,-1} sentinel */

    /* runtime flags and shorthand */
    char ch;                    /**< ASCII char representing piece type + color (e.g. 'P'/'p') */
    int eval = 0;               /**< evaluation marker for renderer: 1 valid, -1 attack, 0 normal */
    bool isMe = false;          /**< true if piece belongs to the current player */
    bool isMoved = false;       /**< true when the piece has moved (affects castling/pawn logic) */
    bool OO = false, OOO = false;/**< castling flags (rook/king availability markers) */

    /* movement metadata */
    bool isSliding = false;     /**< true if moves slide along deltas (rook/bishop/queen) */
    std::vector<std::pair<int,int>> deltas; /**< movement deltas used for move generation */

    /* static value */
    int score = 0;              /**< material score (Pawn=1, Knight/Bishop=3, Rook=5, Queen=9, King=inf) */

    /* methods */
    Piece() = default;

    /**
     * @brief Initialize this Piece from a canonical template.
     * @param ps  position to place the piece (row, col)
     * @param mine true if this piece belongs to the player using this instance
     * @param white true if the piece should be rendered as white (affects glyph choice)
     * @param tpl  canonical template to copy movement metadata and score from
     *
     * Copies deltas, isSliding, score and identity from tpl, sets glyphs according
     * to `white`, marks ownership using `mine` and sets `pos`.
     */
    void set(Pos ps, bool mine, bool white, const Piece &tpl);

    /**
     * @brief Update `eval` and `sym` for rendering a valid/attack square.
     * @param board owning Board (used to read board->eval)
     * @param p     the position whose eval value to apply
     *
     * If eval == 1 the piece will use a highlighted glyph; if eval == -1 it will
     * show an attack glyph; otherwise it displays baseSym.
     */
    void setEval(std::shared_ptr<Board> board, Pos p);

    /**
     * @brief Update the stored position.
     * @param ps new position (row, col)
     */
    void setPos(Pos ps);
};

/**
 * Canonical piece templates (color-agnostic).
 *
 * The templates provide movement metadata and a static score value. Use
 * Piece::set() with one of these templates to create a colored, positioned
 * piece instance for the board.
 *
 * Scores: Pawn=1, Knight/Bishop=3, Rook=5, Queen=9, King=large, Square=0.
 */
extern const Piece Pawn;
extern const Piece Rook;
extern const Piece Bishop;
extern const Piece Knight;
extern const Piece Queen;
extern const Piece King;
extern const Piece Square;
