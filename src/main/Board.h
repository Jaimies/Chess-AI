#pragma once

#include <string>
#include <array>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "Piece.h"
#include "Move.h"

class Board {
public:
    std::array<int, 64> squares = {0};
    std::vector<Move *> legalMoves;

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

    void generateMoves();
    void generateCaptures();
    void checkIfLegalMovesExist();
    void makeMove(Move *move);
    void makeMoveWithoutGeneratingMoves(Move *move);
    void unmakeMove(Move *move);
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

    std::stack<Move *> moveHistory;
    std::stack<int> castlingPieceMovementHistory;

    std::array<bool, 64> attacksKing;
    std::unordered_set<int> squaresAttackedByOpponent;
    std::unordered_set<int> checkSolvingMovePositions;
    std::unordered_map<int, int> pins;

    int kingAttackerPosition;
    int kingSquare;
    int opponentKingSquare;
    bool _isInEndgame = false;
    uint64_t zobristHash = 0;

    Board();

    Board(int colourToMove, std::stack<Move *> moveHistory,
          std::unordered_map<int, bool> castlingPieceMoved, std::array<int, 64> squares);

    void computeMoveData();
    void loadFenString(std::string fenString);
    void generatePins();
    void generatePins(int pieceType, int startSquare);
    void generateSquaresAttackedByOpponent(int color);
    std::vector<Move *> generatePseudoLegalMoves(int color);
    std::vector<Move *> generatePseudoLegalCaptures(int color);
    void generateCheckSolvingMovePositions();
    void generateCheckSolvingMovePosition(int pieceType, int startSquare);
    void generateCastlingMoves(std::vector<Move *> &moves);
    void addCastlingMovesIfAvailable(int kingSquare, int colour, std::vector<Move *> &moves);
    void addCastlingMoveIfPossible(int kingSquare, int rookSquare, std::vector<Move *> &moves);
    bool isCastlingPossible(int kingSquare, int rookSquare, int targetCastlingPosition);
    bool allSquaresAreNotUnderAttackBetween(int kingSquare, int targetKingPosition);
    bool isSquareUnderAttack(int square) const;
    bool allSquaresAreClearBetween(int firstSquare, int secondSquare);
    bool isSideInEndgamePosition(int colour) const;
    bool determineIfIsInEndgame() const;
    void generateSlidingMoves(int startSquare, int piece, std::vector<Move *> &moves, bool canCaptureFriendly, bool capturesOnly);
    void generatePawnMoves(int startSquare, int piece, std::vector<Move *> &moves);
    void generateForwardPawnMoves(
        int startSquare, int piece, std::vector<Move *> &moves, bool isPawnAboutToPromote
    );
    bool IsSquareInFrontClear(int startSquare, int piece);
    void generateCapturePawnMoves(int startSquare, int piece, std::vector<Move *> &moves, bool isPawnAboutToPromote,
                                  bool canCaptureFriendly);
    void generateNormalPawnCaptures(int startSquare, int piece, std::vector<Move *> &moves);
    void generateKnightMoves(int startSquare, int piece, std::vector<Move *> &moves, bool canCaptureFriendly, bool capturesOnly);
    void generateEnPassantMoves(int square, int piece, std::vector<Move *> &moves);

    void updateCastlingPieceMovement(Move *move);
    void undoCastlingPieceMovementUpdate();

    bool isMoveLegal(Move *potentialMove);
    bool violatesPin(Move *move);
    bool coversCheck(Move *potentialMove) const;
    bool isValidEnPassantMove(EnPassantMove *move) const;
    void addMoveIfLegal(Move *move);
    bool legalMovesExist(int colour);

    bool IsKingUnderAttack();
    bool IsKingUnderAttack(Move *potentialMove);

    void changeColourToMove();
    void updateGameState();

    constexpr static const int directionOffsets[]{8, -8, 1, -1, 7, -7, 9, -9};
    constexpr static const int knightMoveOffsets[]{6, 10, 15, 17, -6, -10, -15, -17};
    constexpr static const int pawnCaptureOffsets[]{7, 9};

    constexpr static const int leftDirectionIndex = 3;
    constexpr static const int rightDirectionIndex = 2;

    unsigned long getMinorPieceCount(int colour) const;
    void updateEndgameState();

    static bool shouldStopGeneratingSlidingMoves(bool canCaptureFriendly, int colour, int targetPiece, int targetPieceColour) ;
};
