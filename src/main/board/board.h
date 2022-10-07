#pragma once

#include <string>
#include <array>
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include "piece.h"
#include "../move/move.h"
#include "../move/visitors.h"
#include "move_processor.h"

class Board {
public:
    std::array<int, 64> squares = {0};
    std::vector<MoveVariant> legalMoves;

    int colourToMove = Piece::White;

    bool hasLegalMoves = true;
    int enPassantTargetSquare = -1;
    bool isKingUnderAttack = false;

    std::unordered_map<int, bool> castlingPieceMoved{
            {Piece::King | Piece::White,      false},
            {Piece::King | Piece::Black,      false},
            {Piece::LeftRook | Piece::White,  false},
            {Piece::RightRook | Piece::White, false},
            {Piece::LeftRook | Piece::Black,  false},
            {Piece::RightRook | Piece::Black, false},
    };

    std::unordered_map<char, int> pieceTypeFromSymbol = {
            {'k', Piece::King},
            {'q', Piece::Queen},
            {'r', Piece::Rook},
            {'b', Piece::Bishop},
            {'p', Piece::Pawn},
            {'n', Piece::Knight},
    };

    ApplyMoveVisitor applyMoveVisitor = ApplyMoveVisitor{this};
    UndoMoveVisitor undoMoveVisitor = UndoMoveVisitor(this);

    ~Board();

    void generateMoves();
    void generateCaptures();
    void checkIfLegalMovesExist();
    void makeMove(MoveVariant &move);
    void makeMove(Move *move);
    void makeMoveWithoutGeneratingMoves(MoveVariant &move);
    void unmakeMove(MoveVariant &move);
    Move *getLastMove();
    void unmakeMove(Move *move);
    Board *copy() const;

    int getKingSquare() const;
    int getOpponentKingSquare() const;
    int _getKingSquare(int colour) const;
    int _getKingSquare() const;
    int _getOpponentKingSquare() const;

    bool isInEndgame() const;
    uint64_t getZobristHash() const;

    bool canWhiteCastleLeft() const;
    bool canWhiteCastleRight() const;
    bool canBlackCastleLeft() const;
    bool canBlackCastleRight() const;

    static Board *fromFenString(std::string fenString, int colourToMove = Piece::White);
    std::string toFenString() const;

    static inline const std::string startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq - 0 1";

private:
    int numSquaresToEdge[64][8];

    std::stack<MoveVariant> moveHistory;
    std::stack<int> castlingPieceMovementHistory;

    std::array<bool, 64> attacksKing;
    std::array<bool, 64> squaresAttackedByOpponent;
    std::unordered_set<int> checkSolvingMovePositions;
    std::unordered_map<int, int> pins;

    int kingAttackerPosition;
    int kingSquare;
    int opponentKingSquare;
    bool _isInEndgame = false;

    MoveGenerationProcessor *moveGenerationProcessor = new MoveGenerationProcessor(this);
    CaptureGenerationProcessor *captureGenerationProcessor = new CaptureGenerationProcessor(this);
    AttackedSquaresGenerationProcessor *attackedSquaresGenerationProcessor = new AttackedSquaresGenerationProcessor(this);
    LegalMoveSearchProcessor *legalMoveSearchProcessor = new LegalMoveSearchProcessor(this);

    Board();

    Board(int colourToMove, std::stack<MoveVariant > moveHistory,
          std::unordered_map<int, bool> castlingPieceMoved, std::array<int, 64> squares);

    void computeMoveData();
    void loadFenString(std::string &fenString);
    void generatePins();
    void generatePins(int pieceType, int startSquare);
    void generateSquaresAttackedByOpponent(int color);
    void generateLegalMoves(int color);
    void generateLegalCaptures(int color);
    void generateCheckSolvingMovePositions();
    void generateCheckSolvingMovePosition(int pieceType, int startSquare);
    void generateCastlingMoves(MoveProcessor *processor) const;
    void addCastlingMovesIfAvailable(int kingSquare, int colour, MoveProcessor *processor) const;
    void addCastlingMoveIfPossible(int kingSquare, int rookSquare, MoveProcessor *processor) const;
    bool isCastlingPossible(int kingSquare, int rookSquare, int targetCastlingPosition) const;
    bool allSquaresAreNotUnderAttackBetween(int kingSquare, int targetKingPosition) const;
    bool isSquareUnderAttack(int square) const;
    bool allSquaresAreClearBetween(int firstSquare, int secondSquare) const;
    bool isSideInEndgamePosition(int colour) const;
    bool determineIfIsInEndgame() const;
    void generateSlidingMoves(int startSquare, int piece, MoveProcessor *processor) const;
    void generatePawnMoves(int startSquare, int piece, MoveProcessor *processor) const ;
    void generateForwardPawnMoves(
        int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote
    ) const;
    bool isSquareInFrontClear(int startSquare, int piece) const;
    void generateCapturePawnMoves(int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote,
                                  bool canCaptureFriendly) const;
    void generateNormalPawnCaptures(int startSquare, int piece, MoveProcessor *processor) const;
    void generateKnightMoves(int startSquare, int piece, MoveProcessor *processor) const;
    void generateEnPassantMoves(int square, int piece, MoveProcessor *processor) const;

    void updateCastlingPieceMovement(MoveVariant &move);
    void undoCastlingPieceMovementUpdate();

    bool isMoveLegal(MoveVariant potentialMove) const;
    bool violatesPin(MoveVariant &move) const;
    bool coversCheck(MoveVariant potentialMove) const;
    bool isValidEnPassantMove(EnPassantMove move) const;
    void addMoveIfLegal(MoveVariant &move);
    void legalMovesExist(int colour);

    bool IsKingUnderAttack() const;
    bool IsKingUnderAttack(MoveVariant potentialMove) const;

    void changeColourToMove();
    void updateGameState();

    constexpr static const int directionOffsets[]{8, -8, 1, -1, 7, -7, 9, -9};
    constexpr static const int knightMoveOffsets[]{6, 10, 15, 17, -6, -10, -15, -17};
    constexpr static const int pawnCaptureOffsets[]{7, 9};

    constexpr static const int leftDirectionIndex = 3;
    constexpr static const int rightDirectionIndex = 2;

    unsigned long getMinorPieceCount(int colour) const;
    void updateEndgameState();

    friend class MoveGenerationProcessor;
    friend class AttackedSquaresGenerationProcessor;
    friend class LegalMoveSearchProcessor;
};
