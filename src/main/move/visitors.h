#pragma once

#include "move.h"

class Board;

struct ApplyMoveVisitor {
    explicit ApplyMoveVisitor(Board *board) : board(board) {};

    void operator()(NormalMove &move) const { move.apply(*board); }
    void operator()(PromotionMove &move) const { move.apply(*board); }
    void operator()(CastlingMove &move) const { move.apply(*board); }
    void operator()(EnPassantMove &move) const { move.apply(*board); }

private:
    Board *board;
};

struct UndoMoveVisitor {
    explicit UndoMoveVisitor(Board *board) : board(board) {};

    void operator()(NormalMove &move) const { move.undo(*board); }
    void operator()(PromotionMove &move) const { move.undo(*board); }
    void operator()(CastlingMove &move) const { move.undo(*board); }
    void operator()(EnPassantMove &move) const { move.undo(*board); }

private:
    Board *board;
};

struct _GetEnPassantMoveVisitor {
    std::optional<EnPassantMove> operator()(NormalMove &move) const { return {}; }
    std::optional<EnPassantMove> operator()(PromotionMove &move) const { return {}; }
    std::optional<EnPassantMove> operator()(CastlingMove &move) const { return {}; }
    std::optional<EnPassantMove> operator()(EnPassantMove &move) const { return move; }
};

extern _GetEnPassantMoveVisitor GetEnPassantMoveVisitor;

struct _IsCastlingMoveVisitor {
    bool operator()(NormalMove &move) const { return false; }
    bool operator()(PromotionMove &move) const { return false; }
    bool operator()(CastlingMove &move) const { return true; }
    bool operator()(EnPassantMove &move) const { return false; }
};

extern _IsCastlingMoveVisitor IsCastlingMoveVisitor;

struct _GetBasicMoveVisitor {
    Move &operator()(NormalMove &move) const { return move; }
    Move &operator()(PromotionMove &move) const { return move; }
    Move &operator()(CastlingMove &move) const { return move; }
    Move &operator()(EnPassantMove &move) const { return move; }
};

extern _GetBasicMoveVisitor GetBasicMoveVisitor;

struct _GetMovePointerVisitor {
    Move *operator()(NormalMove &move) const { return new NormalMove(move); }
    Move *operator()(PromotionMove &move) const { return new PromotionMove(move); }
    Move *operator()(CastlingMove &move) const { return new CastlingMove(move); }
    Move *operator()(EnPassantMove &move) const { return new EnPassantMove(move); }
};

extern _GetMovePointerVisitor GetMovePointerVisitor;

struct _DetermineIfMoveCanCaptureVisitor {
    bool operator()(NormalMove &move) const { return move.canCapture(); }
    bool operator()(PromotionMove &move) const { return move.canCapture(); }
    bool operator()(CastlingMove &move) const { return move.canCapture(); }
    bool operator()(EnPassantMove &move) const { return move.canCapture(); }
};

extern _DetermineIfMoveCanCaptureVisitor DetermineIfMoveCanCaptureVisitor;

struct _GetMoveAddedValueVisitor {
    int operator()(NormalMove &move) const { return move.getAddedValue(); }
    int operator()(PromotionMove &move) const { return move.getAddedValue(); }
    int operator()(CastlingMove &move) const { return move.getAddedValue(); }
    int operator()(EnPassantMove &move) const { return move.getAddedValue(); }
};

extern _GetMoveAddedValueVisitor GetMoveAddedValueVisitor;
