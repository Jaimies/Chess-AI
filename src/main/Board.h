#pragma once

#include <string>
#include <array>
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include "Piece.h"
#include "Move.h"
#include "move_processor.h"

class Board {
public:
    std::array<int, 64> squares = {0};
    std::vector<MoveVariant> legalMoves;

    int colourToMove = Piece::White;

    bool hasLegalMoves = true;
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

    ~Board();

    void generateMoves();
    void generateCaptures();
    void checkIfLegalMovesExist();
    void makeMove(MoveVariant &move);
    void makeMoveWithoutGeneratingMoves(MoveVariant &move);
    void unmakeMove(MoveVariant &move);
    Board *copy();

    int getKingSquare() const;
    int getOpponentKingSquare() const;
    int _getKingSquare(int colour) const;
    int _getKingSquare() const;
    int _getOpponentKingSquare() const;

    bool isInEndgame() const;
    uint64_t getZobristHash() const { return zobristHash; };

    bool canWhiteCastleLeft() {
        return !castlingPieceMoved[Piece::King | Piece::White]
               && !castlingPieceMoved[Piece::LeftRook | Piece::White];
    };

    bool canWhiteCastleRight() {
        return !castlingPieceMoved[Piece::King | Piece::White]
               && !castlingPieceMoved[Piece::RightRook | Piece::White];
    };

    bool canBlackCastleLeft() {
        return !castlingPieceMoved[Piece::King | Piece::Black]
               && !castlingPieceMoved[Piece::LeftRook | Piece::Black];

    };

    bool canBlackCastleRight() {
        return !castlingPieceMoved[Piece::King | Piece::Black]
               && !castlingPieceMoved[Piece::RightRook | Piece::Black];
    };

    static Board *fromFenString(std::string fenString, int colourToMove = Piece::White);

    static inline const std::string startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

private:
    int numSquaresToEdge[64][8];

    std::stack<MoveVariant > moveHistory;
    std::stack<int> castlingPieceMovementHistory;

    std::array<bool, 64> attacksKing;
    std::array<bool, 64> squaresAttackedByOpponent;
    std::unordered_set<int> checkSolvingMovePositions;
    std::unordered_map<int, int> pins;

    int kingAttackerPosition;
    int kingSquare;
    int opponentKingSquare;
    bool _isInEndgame = false;
    uint64_t zobristHash = 0;

    ApplyMoveVisitor applyMoveVisitor = ApplyMoveVisitor(this);
    UndoMoveVisitor undoMoveVisitor = UndoMoveVisitor(this);
    GetZobristHashVisitor getZobristHashVisitor = GetZobristHashVisitor(this);
    GetBasicMoveVisitor getBasicMoveVisitor = GetBasicMoveVisitor();
    IsCastlingMoveVisitor isCastlingMoveVisitor = IsCastlingMoveVisitor();
    GetEnPassantMoveVisitor getEnPassantMoveVisitor = GetEnPassantMoveVisitor();

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
    void generateCastlingMoves(MoveProcessor *processor);
    void addCastlingMovesIfAvailable(int kingSquare, int colour, MoveProcessor *processor);
    void addCastlingMoveIfPossible(int kingSquare, int rookSquare, MoveProcessor *processor);
    bool isCastlingPossible(int kingSquare, int rookSquare, int targetCastlingPosition);
    bool allSquaresAreNotUnderAttackBetween(int kingSquare, int targetKingPosition);
    bool isSquareUnderAttack(int square) const;
    bool allSquaresAreClearBetween(int firstSquare, int secondSquare);
    bool isSideInEndgamePosition(int colour) const;
    bool determineIfIsInEndgame() const;
    void generateSlidingMoves(int startSquare, int piece, MoveProcessor *processor);
    void generatePawnMoves(int startSquare, int piece, MoveProcessor *processor);
    void generateForwardPawnMoves(
        int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote
    );
    bool isSquareInFrontClear(int startSquare, int piece);
    void generateCapturePawnMoves(int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote,
                                  bool canCaptureFriendly);
    void generateNormalPawnCaptures(int startSquare, int piece, MoveProcessor *processor);
    void generateKnightMoves(int startSquare, int piece, MoveProcessor *processor);
    void generateEnPassantMoves(int square, int piece, MoveProcessor *processor);

    void updateCastlingPieceMovement(MoveVariant &move);
    void undoCastlingPieceMovementUpdate();

    bool isMoveLegal(MoveVariant potentialMove);
    bool violatesPin(MoveVariant &move);
    bool coversCheck(MoveVariant potentialMove) const;
    bool isValidEnPassantMove(EnPassantMove move) const;
    void addMoveIfLegal(MoveVariant &move);
    void legalMovesExist(int colour);

    bool IsKingUnderAttack();
    bool IsKingUnderAttack(MoveVariant potentialMove);

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
