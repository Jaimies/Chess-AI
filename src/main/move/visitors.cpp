#include "visitors.h"
#include "../board/board.h"

uint64_t GetZobristHashVisitor::operator()(NormalMove &move) const {
    return move.getZorbristHash(board->squares);
}

uint64_t GetZobristHashVisitor::operator()(PromotionMove &move) const {
    return move.getZorbristHash(board->squares);
}

uint64_t GetZobristHashVisitor::operator()(EnPassantMove &move) const {
    return move.getZorbristHash(board->squares);
}

uint64_t GetZobristHashVisitor::operator()(CastlingMove &move) const {
    return move.getZorbristHash(board->squares);
}

_GetEnPassantMoveVisitor GetEnPassantMoveVisitor;
_IsCastlingMoveVisitor IsCastlingMoveVisitor;
_GetBasicMoveVisitor GetBasicMoveVisitor;
_GetMoveAddedValueVisitor GetMoveAddedValueVisitor;
_GetMovePointerVisitor GetMovePointerVisitor;
_DetermineIfMoveCanCaptureVisitor DetermineIfMoveCanCaptureVisitor;
