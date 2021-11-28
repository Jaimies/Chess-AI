#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "Piece.h"
#include "Move.h"

class Board {
public:
    std::array<int, 64> squares;
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

    static Board *fromFenString(std::string fenString, int colourToMove = Piece::White) {
        auto board = new Board();
        board->colourToMove = colourToMove;
        board->loadFenString(fenString);
        board->generateMoves();
        return board;
    }

    void generateMoves() {
        legalMoves.clear();

        int colourToMove = this->colourToMove;
        generateSquaresAttackedByOpponent(Piece::getOpponentColour(colourToMove));
        kingPosition = getKingPosition();
        isKingUnderAttack = IsKingUnderAttack();

        auto pseudoLegalMoves = generatePseudoLegalMoves(colourToMove);

        generateCheckSolvingMovePositions();
        generatePins();

        for (Move *potentialMove: pseudoLegalMoves)
            addMoveIfLegal(potentialMove);

        hasLegalMoves = !legalMoves.empty();
    }

    void checkIfLegalMovesExist() {
        auto opponentColour = Piece::getOpponentColour(colourToMove);

        kingPosition = getKingPosition();
        generateSquaresAttackedByOpponent(opponentColour);
        generatePins();
        isKingUnderAttack = IsKingUnderAttack();
        hasLegalMoves = LegalMovesExist(colourToMove);
    }

    Board *copy() {
        auto newBoard = new Board(colourToMove, moveHistory, castlingPieceMoved, squares);
        newBoard->generateMoves();
        return newBoard;
    }


    void makeMove(Move *move) {
        updateCastlingPieceMovement(move);
        _MakeMove(move);
        generateMoves();
        updateGameState();
    }

    void makeMoveWithoutGeneratingMoves(Move *move) {
        updateCastlingPieceMovement(move);
        _MakeMove(move);
    }

    void unmakeMove(Move *move) {
        move->undo(*this);

        changeColourToMove();
        moveHistory.pop();
    }

    static inline const std::string startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

private:
    int numSquaresToEdge[64][8];

    std::queue<Move *> moveHistory;
    std::queue<int> castlingPieceMovementHistory;

    std::array<std::unordered_set<int>, 64> squaresAttackedByOpponent;
    std::unordered_set<int> squaresAttackedByOpponentSet;
    std::unordered_set<int> checkSolvingMovePositions;
    std::unordered_map<int, int> pins;

    int kingPosition;

    Board() {
        computeMoveData();
    }

    Board(int colourToMove, std::queue<Move *> moveHistory,
          std::unordered_map<int, bool> castlingPieceMoved, std::array<int, 64> squares) {
        this->colourToMove = colourToMove;
        this->moveHistory = moveHistory;
        this->castlingPieceMoved = castlingPieceMoved;
        this->squares = squares;
        computeMoveData();
    }

    void _MakeMove(Move *move) {
        undoCastlingPieceMovementUpdate();
        move->apply(*this);
        changeColourToMove();
        moveHistory.push(move);
    }

    void undoCastlingPieceMovementUpdate() {
        auto lastMove = castlingPieceMovementHistory.back();

        if (lastMove != Piece::None)
            castlingPieceMoved[lastMove] = false;

        castlingPieceMovementHistory.pop();
    }

    void changeColourToMove() {
        colourToMove = Piece::getOpponentColour(colourToMove);
    }

    void updateGameState() {
        if (!legalMoves.empty()) return;

//        if (!isKingUnderAttack) {
//            gameState = GameState.Draw;
//        }
//        else {
//            gameState = ColourToMove == Piece.White
//                        ? GameState.VictoryByBlack
//                        : GameState.VictoryByWhite;
//        }
    }

    void updateCastlingPieceMovement(Move *move) {
        int piece = squares[move->startSquare];
        int castlingPiece = getCastlingPiece(piece, move->startSquare);

        auto shouldUpdate = castlingPiece != Piece::None && !castlingPieceMoved[castlingPiece];

        if (shouldUpdate)
            castlingPieceMoved[castlingPiece] = true;

        castlingPieceMovementHistory.push(shouldUpdate ? castlingPiece : 0);
    }

    static int getCastlingPiece(int piece, int square) {
        auto type = Piece::getType(piece);
        if (type == Piece::King) return piece;
        if (type != Piece::Rook) return Piece::None;

        int file = square % 8;
        int colour = Piece::getColour(piece);

        if (file == 0) return Piece::LeftRook | colour;
        if (file == 7) return Piece::RightRook | colour;

        return Piece::None;
    }

    bool LegalMovesExist(int colour) {
        for (int startSquare = 0; startSquare < 64; startSquare++) {
            int piece = squares[startSquare];
            if (Piece::getColour(piece) != colour) continue;

            std::vector<Move *> moves;

            if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, moves, false);
            else if (Piece::getType(piece) == Piece::Pawn) generatePawnMoves(startSquare, piece, moves);
            else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, moves, false);

            auto hasLegalMoves = std::find_if(moves.begin(), moves.end(), [this](Move *move) {
                return isMoveLegal(move);
            }) != moves.end();

            if (hasLegalMoves) return true;
        }

        return false;
    }

    void addMoveIfLegal(Move *potentialMove) {
        if (isMoveLegal(potentialMove))
            legalMoves.push_back(potentialMove);
    }

    bool isMoveLegal(Move *potentialMove) {
        auto *enPassantMove = dynamic_cast<EnPassantMove *>(potentialMove);

        if (enPassantMove != nullptr)
            return isValidEnPassantMove(enPassantMove);

        if (dynamic_cast<CastlingMove *>(potentialMove) != nullptr) return true;

        return !violatesPin(potentialMove)
               && (!IsKingUnderAttack(potentialMove) || coversCheck(potentialMove));
    }

    bool violatesPin(Move *move) {
        if (!pins.contains(move->startSquare)) return false;
        if (Piece::getType(squares[move->startSquare]) == Piece::Knight) return true;

        auto directionIndex = pins[move->startSquare];
        auto directionOffset = directionOffsets[directionIndex];

        auto squareDifference = move->targetSquare - move->startSquare;

        if (std::abs(directionOffset) == 1)
            return move->startSquare / 8 != move->targetSquare / 8;

        return squareDifference % directionOffset != 0;
    }

    bool coversCheck(Move *potentialMove) const {
        return potentialMove->startSquare != kingPosition &&
               checkSolvingMovePositions.contains(potentialMove->targetSquare);
    }

    bool isValidEnPassantMove(EnPassantMove *move) {
        auto kingFile = kingPosition / 8;
        auto pawnFile = move->startSquare / 8;
        if (kingFile != pawnFile) return true;

        auto opponentColour = Piece::getOpponentColour(colourToMove);

        auto rightPawnPosition = std::max(move->startSquare, move->capturedPawnPosition);
        auto leftPawnPosition = std::min(move->startSquare, move->capturedPawnPosition);
        auto isKingOnTheLeft = kingPosition < leftPawnPosition;
        auto directionIndex = isKingOnTheLeft ? rightDirectionIndex : leftDirectionIndex;

        auto edgePawnPosition = isKingOnTheLeft ? rightPawnPosition : leftPawnPosition;
        auto squaresToEdge = numSquaresToEdge[edgePawnPosition][directionIndex];

        auto searchDirection = isKingOnTheLeft ? 1 : -1;

        for (auto offset = 1; offset <= squaresToEdge; offset++) {
            auto position = edgePawnPosition + offset * searchDirection;
            auto piece = squares[position];

            if (piece != Piece::None) {
                return Piece::getColour(piece) != opponentColour
                       || !Piece::isLongRangeSlidingPiece(Piece::getType(piece));
            }
        }

        return true;
    }

    int getKingPosition() {
        for (int square = 0; square < 64; square++) {
            if (squares[square] == (Piece::King | colourToMove)) return square;
        }

        return -1;
    }

    void loadFenString(std::string fenString) {
        std::string position = fenString.substr(0, fenString.find(' '));

        int rank = 7, file = 0;

        for (char &symbol: position) {
            if (symbol == '/') {
                rank--;
                file = 0;
            } else if (std::isdigit(symbol)) {
                file += symbol - '0';
            } else {
                int pieceType = pieceTypeFromSymbol[std::tolower(symbol)];
                int pieceColor = std::isupper(symbol) ? Piece::White : Piece::Black;

                squares[rank * 8 + file] = pieceType | pieceColor;
                file++;
            }
        }
    }

    void computeMoveData() {
        for (int file = 0; file < 8; file++) {
            for (int rank = 0; rank < 8; rank++) {
                int numNorth = 7 - rank;
                int numSouth = rank;
                int numWest = file;
                int numEast = 7 - file;

                int squareIndex = rank * 8 + file;

                numSquaresToEdge[squareIndex][0] = numNorth;
                numSquaresToEdge[squareIndex][1] = numSouth;
                numSquaresToEdge[squareIndex][2] = numEast;
                numSquaresToEdge[squareIndex][3] = numWest;
                numSquaresToEdge[squareIndex][4] = std::min(numNorth, numWest);
                numSquaresToEdge[squareIndex][5] = std::min(numSouth, numEast);
                numSquaresToEdge[squareIndex][6] = std::min(numNorth, numEast);
                numSquaresToEdge[squareIndex][7] = std::min(numSouth, numWest);
            }
        }
    }

    void generatePins() {
        pins.clear();

        for (int square = 0; square < 64; square++) {
            auto pieceType = Piece::getType(squares[square]);

            if (Piece::isLongRangeSlidingPiece(pieceType))
                generatePins(pieceType, square);
        }
    }

    void generatePins(int pieceType, int startSquare) {
        int startDirIndex = pieceType == Piece::Bishop ? 4 : 0;
        int endDirIndex = pieceType == Piece::Rook ? 4 : 8;

        auto colour = Piece::getColour(squares[startSquare]);
        auto opponentColour = Piece::getOpponentColour(colour);

        for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++) {
            int maxOffset = numSquaresToEdge[startSquare][directionIndex];

            auto potentialPinnedPiecePosition = -1;

            for (int offset = 1; offset <= maxOffset; offset++) {
                int targetSquare = startSquare + directionOffsets[directionIndex] * offset;
                int targetPiece = squares[targetSquare];

                if (potentialPinnedPiecePosition == -1 &&
                    Piece::getColour(targetPiece) == opponentColour
                    && Piece::getType(targetPiece) != Piece::King) {
                    potentialPinnedPiecePosition = targetSquare;
                    continue;
                }

                if (Piece::getColour(targetPiece) == colour) break;

                if (potentialPinnedPiecePosition == -1 || targetPiece == Piece::None) continue;

                if (targetPiece == (Piece::King | opponentColour))
                    pins[potentialPinnedPiecePosition] = directionIndex;
                else break;
            }
        }
    }

    void generateCheckSolvingMovePositions() {
        unsigned int checkCount = 0;
        checkSolvingMovePositions.clear();
        if (!isKingUnderAttack) return;

        for (int square = 0; square < 64; square++) {
            auto attackedPositions = squaresAttackedByOpponent[square];

            if (attackedPositions.contains(kingPosition)) {
                checkCount++;
                if (checkCount >= 2) {
                    checkSolvingMovePositions.clear();
                    return;
                }

                checkSolvingMovePositions.insert(square);

                int pieceType = Piece::getType(squares[square]);
                if (Piece::isLongRangeSlidingPiece(pieceType)) {
                    generateCheckSolvingMovePosition(pieceType, square);
                }
            }
        }
    }

    void generateCheckSolvingMovePosition(int pieceType, int startSquare) {
        int startDirIndex = pieceType == Piece::Bishop ? 4 : 0;
        int endDirIndex = pieceType == Piece::Rook ? 4 : 8;

        auto colour = Piece::getColour(squares[startSquare]);

        for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++) {
            int maxOffset = numSquaresToEdge[startSquare][directionIndex];
            bool shouldAdd = false;
            std::unordered_set<int> squares;

            for (int offset = 1; offset <= maxOffset; offset++) {
                int targetSquare = startSquare + directionOffsets[directionIndex] * offset;

                int targetPiece = this->squares[targetSquare];

                if (targetPiece != Piece::None && targetPiece != (Piece::King | Piece::getOpponentColour(colour))) break;

                squares.insert(targetSquare);

                if (targetPiece == (Piece::King | Piece::getOpponentColour(colour))) {
                    shouldAdd = true;
                    break;
                }
            }

            if (!shouldAdd) continue;

            for (auto square: squares)
                checkSolvingMovePositions.insert(square);
        }
    }

    bool IsKingUnderAttack() {
        return squaresAttackedByOpponentSet.contains(kingPosition);
    }

    bool IsKingUnderAttack(Move *potentialMove) {
        if (potentialMove->startSquare != kingPosition) return isKingUnderAttack;
        return squaresAttackedByOpponentSet.contains(potentialMove->targetSquare);
    }

    void generateSquaresAttackedByOpponent(int colour) {
        squaresAttackedByOpponentSet.clear();

        for (int startSquare = 0; startSquare < 64; startSquare++) {
            int piece = squares[startSquare];
            squaresAttackedByOpponent[startSquare].clear();
            if (Piece::getColour(piece) != colour) continue;

            std::vector<Move *> moves;

            if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, moves, true);
            else if (Piece::getType(piece) == Piece::Pawn)
                generateCapturePawnMoves(startSquare, piece, moves, false, true);
            else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, moves, true);

            for (auto move: moves) {
                squaresAttackedByOpponent[startSquare].insert(move->targetSquare);
                squaresAttackedByOpponentSet.insert(move->targetSquare);
            }
        }
    }

    std::vector<Move *> generatePseudoLegalMoves(int colour) {
        auto moves = std::vector<Move *>();

        for (int startSquare = 0; startSquare < 64; startSquare++) {
            int piece = squares[startSquare];
            if (Piece::getColour(piece) != colour) continue;

            if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, moves, false);
            else if (Piece::getType(piece) == Piece::Pawn) generatePawnMoves(startSquare, piece, moves);
            else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, moves, false);
        }

        generateCastlingMoves(moves);

        return moves;
    }

    void generateCastlingMoves(std::vector<Move *> &moves) {
        if (colourToMove == Piece::White)
            addCastlingMovesIfAvailable(4, Piece::White, moves);
        else
            addCastlingMovesIfAvailable(60, Piece::Black, moves);
    }

    void addCastlingMovesIfAvailable(int kingSquare, int colour, std::vector<Move *> &moves) {
        int king = squares[kingSquare];

        if (king != (Piece::King | colour)) return;

        if (!castlingPieceMoved[king] && !isKingUnderAttack) {
            addCastlingMoveIfPossible(kingSquare, kingSquare - 4, moves);
            addCastlingMoveIfPossible(kingSquare, kingSquare + 3, moves);
        }
    }

    void addCastlingMoveIfPossible(int kingSquare, int rookSquare, std::vector<Move *> &moves) {
        if (IsCastlingPossible(kingSquare, rookSquare)) {
            moves.push_back(new CastlingMove(kingSquare, rookSquare));
        }
    }

    bool IsCastlingPossible(int kingSquare, int rookSquare) {
        int king = squares[kingSquare];
        int rook = squares[rookSquare];

        int colour = Piece::getColour(king);
        int rookType = rookSquare < kingSquare ? Piece::LeftRook : Piece::RightRook;

        auto targetCastlingPosition = getTargetCastlingPositionForKing(kingSquare, rookSquare);

        return rook == (Piece::Rook | colour) &&
               !castlingPieceMoved[rookType | colour] &&
               AllSquaresAreClearBetween(kingSquare, rookSquare) &&
               allSquaresAreNotUnderAttackBetween(kingSquare, targetCastlingPosition);
    }

    bool allSquaresAreNotUnderAttackBetween(int kingSquare, int targetKingPosition) {
        auto startSquare = std::min(kingSquare, targetKingPosition);
        for (int square = startSquare; square < startSquare + 3; square++) {
            if (isSquareUnderAttack(square)) return false;
        }

        return true;
    }

    bool isSquareUnderAttack(int square) const {
        return squaresAttackedByOpponentSet.contains(square);
    }

    static int getTargetCastlingPositionForKing(int kingPosition, int rookPosition) {
        int sign = rookPosition - kingPosition > 0 ? 1 : -1;
        return kingPosition + sign * 2;
    }

    bool AllSquaresAreClearBetween(int firstSquare, int secondSquare) {
        int startSquare = std::min(firstSquare, secondSquare);
        int endSquare = std::max(firstSquare, secondSquare);

        for (int square = startSquare + 1; square < endSquare; square++) {
            if (squares[square] != Piece::None) return false;
        }

        return true;
    }

    void generateSlidingMoves(int startSquare, int piece, std::vector<Move *> &moves, bool canCaptureFriendly) {
        int pieceType = Piece::getType(piece);
        int startDirIndex = pieceType == Piece::Bishop ? 4 : 0;
        int endDirIndex = pieceType == Piece::Rook ? 4 : 8;
        int maxMoveLength = pieceType == Piece::King ? 1 : 7;
        int colour = Piece::getColour(piece);

        for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++) {
            int maxOffset = std::min(maxMoveLength, numSquaresToEdge[startSquare][directionIndex]);
            for (int offset = 1; offset <= maxOffset; offset++) {
                auto targetSquare = startSquare + directionOffsets[directionIndex] * offset;
                auto targetPiece = squares[targetSquare];

                auto targetPieceColour = Piece::getColour(targetPiece);

                if (targetPieceColour == colour) {
                    if (canCaptureFriendly)
                        moves.push_back(new NormalMove(startSquare, targetSquare));

                    break;
                }

                auto targetPieceType = Piece::getType(targetPiece);

                moves.push_back(new NormalMove(startSquare, targetSquare, targetPiece));

                if (targetPiece != Piece::None
                    && (!canCaptureFriendly || targetPieceType != Piece::King))
                    break;
            }
        }
    }

    void generatePawnMoves(int startSquare, int piece, std::vector<Move *> &moves) {
        auto isAboutToPromote = isPawnAboutToPromote(startSquare, piece);

        generateForwardPawnMoves(startSquare, piece, moves, isAboutToPromote);
        generateCapturePawnMoves(startSquare, piece, moves, isAboutToPromote, false);
        generateEnPassantMoves(startSquare, piece, moves);
    }

    static bool isPawnAboutToPromote(int position, int piece) {
        int colour = Piece::getColour(piece);
        int rank = position / 8;

        return (rank == 6 && colour == Piece::White)
               || rank == 1 && colour == Piece::Black;
    }

    void generateForwardPawnMoves(
            int startSquare, int piece, std::vector<Move *> &moves, bool isPawnAboutToPromote
    ) {
        int possibleOffsets[]{8, 16};

        if (!IsSquareInFrontClear(startSquare, piece)) return;

        for (int pawnOffset: possibleOffsets) {
            int offset = Piece::getColour(piece) == Piece::White ? pawnOffset : -pawnOffset;

            if (pawnOffset == 16) {
                if (!isPawnAtStartSquare(startSquare, piece)) continue;
                int pieceInFront = squares[startSquare + offset / 2];
                if (pieceInFront != Piece::None) continue;
            }

            int targetSquarePosition = startSquare + offset;

            if (!IsValidSquarePosition(targetSquarePosition)) continue;

            int targetPiece = squares[targetSquarePosition];

            if (targetPiece != Piece::None) continue;
            generatePawnMove(startSquare, targetSquarePosition, isPawnAboutToPromote, Piece::None, moves);
        }
    }

    static bool isPawnAtStartSquare(int square, int piece) {
        int rank = square / 8;
        return rank == GetPawnRank(piece);
    }

    static int GetPawnRank(int pawnPiece) {
        int colour = Piece::getColour(pawnPiece);

        if (colour == Piece::None) throw std::invalid_argument("Expected a pawn with a colour, got one with Piece::None");

        return Piece::getColour(pawnPiece) == Piece::White ? 1 : 6;
    }

    bool IsSquareInFrontClear(int startSquare, int piece) {
        int positionOfPieceInFront = startSquare + (Piece::getColour(piece) == Piece::White ? 8 : -8);
        return squares[positionOfPieceInFront] == Piece::None;
    }

    void generateCapturePawnMoves(int startSquare, int piece, std::vector<Move *> &moves, bool isPawnAboutToPromote,
                                  bool canCaptureFriendly) {
        int file = startSquare % 8;

        for (int captureOffset: pawnCaptureOffsets) {
            int offset = Piece::getColour(piece) == Piece::White ? captureOffset : -captureOffset;

            if (offset == 7 && file == 0 || offset == 9 && file == 7
                || offset == -9 && file == 0 || offset == -7 && file == 7)
                continue;
            int targetSquarePosition = startSquare + offset;

            if (!IsValidSquarePosition(targetSquarePosition)) continue;
            int targetPiece = squares[targetSquarePosition];

            if (Piece::getColour(targetPiece) == Piece::getOpponentColourFromPiece(piece) || canCaptureFriendly)
                generatePawnMove(startSquare, targetSquarePosition, isPawnAboutToPromote, targetPiece, moves);
        }
    }

    static bool IsValidSquarePosition(int squarePosition) {
        return squarePosition >= 0 && squarePosition <= 63;
    }

    static void generatePawnMove(int startSquare, int targetSquare, bool isPawnAboutToPromote, int pieceToCapture,
                                 std::vector<Move *> &moves) {
        if (!isPawnAboutToPromote) {
            moves.push_back(new NormalMove(startSquare, targetSquare, pieceToCapture));
            return;
        }

        for (auto piece: Piece::piecesToPromoteTo)
            moves.push_back(new PromotionMove(startSquare, targetSquare, piece, pieceToCapture));
    }

    void generateKnightMoves(int startSquare, int piece, std::vector<Move *> &moves, bool canCaptureFriendly) {
        int file = startSquare % 8;

        for (auto offset: knightMoveOffsets) {
            if (file >= 6 && (offset == -6 || offset == 10)
                || file == 7 && (offset == 17 || offset == -15)
                || file <= 1 && (offset == 6 || offset == -10)
                || file == 0 && (offset == -17 || offset == 15))
                continue;

            int targetSquarePosition = startSquare + offset;
            if (!IsValidSquarePosition(targetSquarePosition)) continue;

            int pieceInTargetSquare = squares[targetSquarePosition];

            if (canCaptureFriendly || Piece::getColour(pieceInTargetSquare) != Piece::getColour(piece))
                moves.push_back(new NormalMove(startSquare, targetSquarePosition, pieceInTargetSquare));
        }
    }

    void generateEnPassantMoves(int square, int piece, std::vector<Move *> &moves) {
        auto file = square % 8;
        auto rank = square / 8;

        if (rank != 3 && rank != 4) return;

        int enPassantOffsets[]{-1, 1};

        for (int offset: enPassantOffsets) {
            if (offset == 1 && file == 7 || offset == -1 && file == 0) continue;

            int neighbourPosition = square + offset;

            if (!IsValidSquarePosition(neighbourPosition)) return;

            int neighbourPiece = squares[neighbourPosition];
            int enemyPawn = Piece::Pawn | Piece::getOpponentColourFromPiece(piece);

            if (moveHistory.empty()) continue;

            auto lastMove = moveHistory.back();

            if (neighbourPiece != enemyPawn
                || getRank(lastMove->startSquare) != GetPawnRank(neighbourPiece)
                || lastMove->targetSquare != neighbourPosition)
                continue;

            int targetPositionOffset = Piece::getColour(piece) == Piece::White ? 8 : -8;

            moves.push_back(
                    new EnPassantMove(square, neighbourPosition + targetPositionOffset, neighbourPiece, neighbourPosition)
            );
        }
    }

    static int getRank(int square) {
        return square / 8;
    }

    constexpr static const int directionOffsets[]{8, -8, 1, -1, 7, -7, 9, -9};
    constexpr static const int knightMoveOffsets[]{6, 10, 15, 17, -6, -10, -15, -17};
    constexpr static const int pawnCaptureOffsets[]{7, 9};

    constexpr static const int leftDirectionIndex = 2;
    constexpr static const int rightDirectionIndex = 2;
};

