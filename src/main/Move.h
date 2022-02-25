#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <boost/variant2/variant.hpp>
#include "Piece.h"

class Board;
class NormalMove;
class CastlingMove;
class EnPassantMove;
class PromotionMove;

typedef boost::variant2::variant<NormalMove, CastlingMove, EnPassantMove, PromotionMove> MoveVariant;

class Move {
public:
    int startSquare;
    int targetSquare;

    virtual bool canCapture() { return false; };

    virtual void apply(Board &board);
    virtual void undo(Board &board);

    virtual int getCapturedSquare() { return -1; };
    virtual uint64_t getZorbristHash(std::array<int, 64> squares);
    virtual int getAddedValue() { return 0; };
    std::string toString() const;

    virtual ~Move() = default;
protected:
    Move(int startSquare, int targetSquare);
};

class NormalMove : public Move {
public:
    int capturedPiece;
    NormalMove(int startSquare, int targetSquare, int capturedPiece = Piece::None);

    static NormalMove fromString(std::string str);

    void apply(Board &board) override;
    void undo(Board &board) override;

    bool canCapture() override { return true; }
    int getCapturedSquare() override { return targetSquare; }
};

class CastlingMove : public Move {
public:
    int rookSquare;
    int rookTargetSquare;

    CastlingMove(int kingSquare, int targetSquare, int rookSquare, int rookTargetSquare);

    void apply(Board &board) override;
    void undo(Board &board) override;
};

class EnPassantMove : public NormalMove {
public:
    int capturedPawnPosition;

    EnPassantMove(int startSquare, int targetSquare, int capturedPawn, int capturedPawnPosition);

    void apply(Board &board) override;
    void undo(Board &board) override;

    uint64_t getZorbristHash(std::array<int, 64> squares) override;

    bool canCapture() override { return false; }
    int getCapturedSquare() override { return capturedPawnPosition; }
};

class PromotionMove : public NormalMove {
public:
    int pieceToPromoteTo;

    PromotionMove(int startSquare, int targetSquare, int pieceToPromoteTo, int capturedPiece = Piece::None);

    void apply(Board &board) override;
    void undo(Board &board) override;

    uint64_t getZorbristHash(std::array<int, 64> squares) override;

    int getCapturedSquare() override { return targetSquare; }
    int getAddedValue() override { return Piece::getValue(pieceToPromoteTo); }
};

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

struct GetEnPassantMoveVisitor {
    std::optional<EnPassantMove> operator()(NormalMove &move) const { return {}; }
    std::optional<EnPassantMove> operator()(PromotionMove &move) const { return {}; }
    std::optional<EnPassantMove> operator()(CastlingMove &move) const { return {}; }
    std::optional<EnPassantMove> operator()(EnPassantMove &move) const { return move; }
};

struct GetZobristHashVisitor {
    explicit GetZobristHashVisitor(Board *board) : board(board) {};

    uint64_t operator()(NormalMove &move) const;
    uint64_t operator()(PromotionMove &move) const;
    uint64_t operator()(CastlingMove &move) const;
    uint64_t operator()(EnPassantMove &move) const;

private:
    Board *board;
};

struct IsCastlingMoveVisitor {
    bool operator()(NormalMove &move) const { return false; }
    bool operator()(PromotionMove &move) const { return false; }
    bool operator()(CastlingMove &move) const { return true; }
    bool operator()(EnPassantMove &move) const { return false; }
};

struct GetBasicMoveVisitor {
    Move &operator()(NormalMove &move) const { return move; }
    Move &operator()(PromotionMove &move) const { return move; }
    Move &operator()(CastlingMove &move) const { return move; }
    Move &operator()(EnPassantMove &move) const { return move; }
};

struct GetMovePointerVisitor {
    Move *operator()(NormalMove &move) const { return new NormalMove(move); }
    Move *operator()(PromotionMove &move) const { return new PromotionMove(move); }
    Move *operator()(CastlingMove &move) const { return new CastlingMove(move); }
    Move *operator()(EnPassantMove &move) const { return new EnPassantMove(move); }
};

struct DetermineIfMoveCanCaptureVisitor {
    bool operator()(NormalMove &move) const { return move.canCapture(); }
    bool operator()(PromotionMove &move) const { return move.canCapture(); }
    bool operator()(CastlingMove &move) const { return move.canCapture(); }
    bool operator()(EnPassantMove &move) const { return move.canCapture(); }
};

struct GetMoveAddedValueVisitor {
    int operator()(NormalMove &move) const { return move.getAddedValue(); }
    int operator()(PromotionMove &move) const { return move.getAddedValue(); }
    int operator()(CastlingMove &move) const { return move.getAddedValue(); }
    int operator()(EnPassantMove &move) const { return move.getAddedValue(); }
};
