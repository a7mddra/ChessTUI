#include "game/board.hpp"

void Board::setState(gst st)
{
    state = st;
    umarkValid();
    log = assets::logs[st];
}

void Board::reState()
{
    // check for check, checkmate, stalemate, draw
    setState(gst::INPUT);
}
