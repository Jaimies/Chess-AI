#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <boost/variant2/variant.hpp>
#include "../board/piece.h"

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
    MoveVariant toVariant();

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
    bool operator==(const NormalMove& other) const;
};

class CastlingMove : public Move {
public:
    int rookSquare;
    int rookTargetSquare;

    CastlingMove(int kingSquare, int targetSquare, int rookSquare, int rookTargetSquare);

    void apply(Board &board) override;
    void undo(Board &board) override;
    bool operator==(const CastlingMove& other) const;
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
    bool operator==(const EnPassantMove& other) const;
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
    bool operator==(const PromotionMove& other) const;
};
