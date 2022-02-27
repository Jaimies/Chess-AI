#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include "Piece.h"
#include "Move.h"
#include "Board.h"
#include "zobrist_hash_generator.h"
#include "move_generation_strategy.h"

static bool isValidSquarePosition(int squarePosition) {
    return squarePosition >= 0 && squarePosition <= 63;
}

static bool isRookAtStartRank(int colour, int square) {
    int rank = square / 8;
    return colour == Piece::White && rank == 0 || colour == Piece::Black && rank == 7;
}

static int getCastlingPiece(int piece, int square) {
    auto type = Piece::getType(piece);
    if (type == Piece::King) return piece;
    if (type != Piece::Rook) return Piece::None;

    int file = square % 8;
    int colour = Piece::getColour(piece);

    if (!isRookAtStartRank(colour, square)) return Piece::None;

    if (file == 0) return Piece::LeftRook | colour;
    if (file == 7) return Piece::RightRook | colour;

    return Piece::None;
}

static void generatePawnMove(int startSquare, int targetSquare, bool isPawnAboutToPromote, int pieceToCapture,
                             MoveProcessor *processor) {
    if (!isPawnAboutToPromote) {
        processor->processMove(NormalMove(startSquare, targetSquare, pieceToCapture));
        return;
    }

    for (auto piece: Piece::piecesToPromoteTo)
        processor->processMove(PromotionMove(startSquare, targetSquare, piece, pieceToCapture));
}

Board *Board::fromFenString(std::string fenString, int colourToMove) {
    auto board = new Board();
    board->colourToMove = colourToMove;
    board->loadFenString(fenString);
    board->generateMoves();
    board->updateEndgameState();
    return board;
}

void Board::generateMoves() {
    legalMoves.clear();

    int colourToMove = this->colourToMove;
    generateSquaresAttackedByOpponent(Piece::getOpponentColour(colourToMove));
    isKingUnderAttack = IsKingUnderAttack();

    generateCheckSolvingMovePositions();
    generatePins();

    generateLegalMoves(colourToMove);

    hasLegalMoves = !legalMoves.empty();
}

void Board::checkIfLegalMovesExist() {
    auto opponentColour = Piece::getOpponentColour(colourToMove);

    generateSquaresAttackedByOpponent(opponentColour);
    generatePins();
    isKingUnderAttack = IsKingUnderAttack();
    legalMovesExist(colourToMove);
}

void Board::makeMove(MoveVariant &move) {
    makeMoveWithoutGeneratingMoves(move);
    generateMoves();
    updateGameState();
    updateEndgameState();
}

void Board::updateEndgameState() { _isInEndgame = determineIfIsInEndgame(); }

void Board::unmakeMove(MoveVariant &move) {
    undoCastlingPieceMovementUpdate();
    visit(undoMoveVisitor, move);

    changeColourToMove();
    moveHistory.pop();
    zobristHash ^= visit(getZobristHashVisitor, move);
}

Board *Board::copy() {
    auto board = new Board(colourToMove, moveHistory, castlingPieceMoved, squares);
    board->generateMoves();
    return board;
}

Board::Board() {
    computeMoveData();
}

Board::Board(int colourToMove, std::stack<MoveVariant> moveHistory, std::unordered_map<int, bool> castlingPieceMoved,
             std::array<int, 64> squares) {
    this->colourToMove = colourToMove;
    this->moveHistory = moveHistory;
    this->castlingPieceMoved = castlingPieceMoved;
    this->squares = squares;
    computeMoveData();
    updateEndgameState();
    kingSquare = _getKingSquare();
    opponentKingSquare = _getOpponentKingSquare();
    zobristHash = hash(this);
}

void Board::computeMoveData() {
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

void Board::generatePins() {
    pins.clear();

    for (int square = 0; square < 64; square++) {
        auto pieceType = Piece::getType(squares[square]);

        if (Piece::isLongRangeSlidingPiece(pieceType))
            generatePins(pieceType, square);
    }
}

void Board::generatePins(int pieceType, int startSquare) {
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

void Board::undoCastlingPieceMovementUpdate() {
    auto lastMove = castlingPieceMovementHistory.top();

    if (lastMove != Piece::None)
        castlingPieceMoved[lastMove] = false;

    castlingPieceMovementHistory.pop();
}

void Board::generateCheckSolvingMovePositions() {
    unsigned int checkCount = 0;
    checkSolvingMovePositions.clear();
    if (!isKingUnderAttack) return;

    for (int square = 0; square < 64; square++) {
        if (attacksKing[square]) {
            kingAttackerPosition = square;
            checkCount++;

            if (checkCount >= 2) {
                kingAttackerPosition = -1;
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

void Board::generateLegalMoves(int colour) {
    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        if (Piece::getColour(piece) != colour) continue;

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, moveGenerationProcessor, NormalMoveGenerationStrategy);
        else if (Piece::getType(piece) == Piece::Pawn) generatePawnMoves(startSquare, piece, moveGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, moveGenerationProcessor, NormalMoveGenerationStrategy);
    }

    generateCastlingMoves(moveGenerationProcessor);
}

void Board::generateLegalCaptures(int color) {
    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        if (Piece::getColour(piece) != color) continue;

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, moveGenerationProcessor, CaptureGenerationStrategy);
        else if (Piece::getType(piece) == Piece::Pawn) generateNormalPawnCaptures(startSquare, piece, moveGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, moveGenerationProcessor, CaptureGenerationStrategy);
    }
}

void Board::generateSquaresAttackedByOpponent(int colour) {
    for (int square = 0; square < 64; square++)
        squaresAttackedByOpponent[square] = false;

    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        attacksKing[startSquare] = false;
        if (Piece::getColour(piece) != colour) continue;

        if (Piece::isSlidingPiece(piece))generateSlidingMoves(startSquare, piece, attackedSquaresGenerationProcessor, AttackedSquaresGenerationStrategy);
        else if (Piece::getType(piece) == Piece::Pawn) generateCapturePawnMoves(startSquare, piece, attackedSquaresGenerationProcessor, false, true);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, attackedSquaresGenerationProcessor, AttackedSquaresGenerationStrategy);
    }
}

void Board::loadFenString(std::string &fenString) {
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

    kingSquare = _getKingSquare();
    opponentKingSquare = _getOpponentKingSquare();
    zobristHash = hash(this);
}

void Board::updateCastlingPieceMovement(MoveVariant &move) {
    auto basicMove = visit(getBasicMoveVisitor, move);
    int piece = squares[basicMove.startSquare];
    int castlingPiece = getCastlingPiece(piece, basicMove.startSquare);

    auto shouldUpdate = castlingPiece != Piece::None && !castlingPieceMoved[castlingPiece];

    if (shouldUpdate)
        castlingPieceMoved[castlingPiece] = true;

    castlingPieceMovementHistory.push(shouldUpdate ? castlingPiece : 0);
}

void Board::addMoveIfLegal(MoveVariant &potentialMove) {
    if (isMoveLegal(potentialMove))
        legalMoves.push_back(potentialMove);
}

bool Board::isMoveLegal(MoveVariant potentialMove) {
    if (visit(isCastlingMoveVisitor, potentialMove)) return true;

    auto enPassantMove = visit(getEnPassantMoveVisitor, potentialMove);

    return !violatesPin(potentialMove)
           && (!IsKingUnderAttack(potentialMove) || coversCheck(potentialMove) ||
               (enPassantMove.has_value() && enPassantMove->capturedPawnPosition == kingAttackerPosition))
           && (!enPassantMove.has_value() || isValidEnPassantMove(*enPassantMove));
}

void Board::legalMovesExist(int colour) {
    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        if (Piece::getColour(piece) != colour) continue;

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, legalMoveSearchProcessor, NormalMoveGenerationStrategy);
        else if (Piece::getType(piece) == Piece::Pawn) generatePawnMoves(startSquare, piece, legalMoveSearchProcessor);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, legalMoveSearchProcessor, NormalMoveGenerationStrategy);

        if (hasLegalMoves) return;
    }
}

bool Board::IsKingUnderAttack() {
    return squaresAttackedByOpponent[kingSquare];
}

bool Board::IsKingUnderAttack(MoveVariant potentialMove) {
    auto basicMove = visit(getBasicMoveVisitor, potentialMove);
    if (basicMove.startSquare != kingSquare) return isKingUnderAttack;
    return squaresAttackedByOpponent[basicMove.targetSquare];
}

void Board::changeColourToMove() {
    colourToMove = Piece::getOpponentColour(colourToMove);
}

void Board::updateGameState() {
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

void Board::makeMoveWithoutGeneratingMoves(MoveVariant &move) {
    zobristHash ^= visit(getZobristHashVisitor, move);
    updateCastlingPieceMovement(move);
    visit(applyMoveVisitor, move);
    changeColourToMove();
    moveHistory.push(move);
    kingSquare = _getKingSquare();
    opponentKingSquare = _getOpponentKingSquare();
}

bool Board::violatesPin(MoveVariant &move) {
    auto basicMove = visit(getBasicMoveVisitor, move);
    if (!pins.contains(basicMove.startSquare)) return false;
    if (Piece::getType(squares[basicMove.startSquare]) == Piece::Knight) return true;

    auto directionIndex = pins[basicMove.startSquare];
    auto directionOffset = directionOffsets[directionIndex];

    auto squareDifference = basicMove.targetSquare - basicMove.startSquare;

    if (std::abs(directionOffset) == 1)
        return basicMove.startSquare / 8 != basicMove.targetSquare / 8;

    return squareDifference % directionOffset != 0;
}

bool Board::coversCheck(MoveVariant potentialMove) const {
    auto basicMove = visit(getBasicMoveVisitor, potentialMove);
    return basicMove.startSquare != kingSquare &&
           checkSolvingMovePositions.contains(basicMove.targetSquare);
}

bool Board::isValidEnPassantMove(EnPassantMove move) const {
    auto kingFile = kingSquare / 8;
    auto pawnFile = move.startSquare / 8;
    if (kingFile != pawnFile) return true;

    auto opponentColour = Piece::getOpponentColour(colourToMove);

    auto rightPawnPosition = std::max(move.startSquare, move.capturedPawnPosition);
    auto leftPawnPosition = std::min(move.startSquare, move.capturedPawnPosition);
    auto isKingOnTheLeft = kingSquare < leftPawnPosition;
    auto rookDirectionIndex = isKingOnTheLeft ? rightDirectionIndex : leftDirectionIndex;
    auto kingDirectionIndex = !isKingOnTheLeft ? rightDirectionIndex : leftDirectionIndex;

    auto edgePawnPosition = isKingOnTheLeft ? rightPawnPosition : leftPawnPosition;
    auto squaresToRookEdge = numSquaresToEdge[edgePawnPosition][rookDirectionIndex];
    auto squaresToKingEdge = numSquaresToEdge[edgePawnPosition][kingDirectionIndex];

    auto rookDirection = isKingOnTheLeft ? 1 : -1;

    for (auto offset = 2; offset <= squaresToKingEdge; offset++) {
        auto position = edgePawnPosition - offset * rookDirection;
        auto piece = squares[position];

        if (position == kingSquare) break;
        if (piece != Piece::None) return true;
    }

    for (auto offset = 1; offset <= squaresToRookEdge; offset++) {
        auto position = edgePawnPosition + offset * rookDirection;
        auto piece = squares[position];

        if (piece != Piece::None) {
            return Piece::getColour(piece) != opponentColour
                   || !Piece::isLongRangeSlidingPiece(Piece::getType(piece));
        }
    }

    return true;
}

void Board::generateCheckSolvingMovePosition(int pieceType, int startSquare) {
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

void Board::generateCastlingMoves(MoveProcessor *processor) {
    if (colourToMove == Piece::White)
        addCastlingMovesIfAvailable(4, Piece::White, processor);
    else
        addCastlingMovesIfAvailable(60, Piece::Black, processor);
}

void Board::addCastlingMovesIfAvailable(int kingSquare, int colour, MoveProcessor *processor) {
    int king = squares[kingSquare];

    if (king != (Piece::King | colour)) return;

    if (!castlingPieceMoved[king] && !isKingUnderAttack) {
        addCastlingMoveIfPossible(kingSquare, kingSquare - 4, processor);
        addCastlingMoveIfPossible(kingSquare, kingSquare + 3, processor);
    }
}

void Board::addCastlingMoveIfPossible(int kingSquare, int rookSquare, MoveProcessor *processor) {
    int directionMultiplier = rookSquare > kingSquare ? 1 : -1;
    int kingTargetSquare = kingSquare + 2 * directionMultiplier;
    int rookTargetSquare = kingSquare + 1 * directionMultiplier;

    if (isCastlingPossible(kingSquare, rookSquare, kingTargetSquare))
        processor->processMove(CastlingMove(kingSquare, kingTargetSquare, rookSquare, rookTargetSquare));
}

bool Board::isCastlingPossible(int kingSquare, int rookSquare, int targetCastlingPosition) {
    int king = squares[kingSquare];
    int rook = squares[rookSquare];

    int colour = Piece::getColour(king);
    int rookType = rookSquare < kingSquare ? Piece::LeftRook : Piece::RightRook;

    return rook == (Piece::Rook | colour) &&
           !castlingPieceMoved[rookType | colour] &&
           allSquaresAreClearBetween(kingSquare, rookSquare) &&
           allSquaresAreNotUnderAttackBetween(kingSquare, targetCastlingPosition);
}

bool Board::allSquaresAreNotUnderAttackBetween(int kingSquare, int targetKingPosition) {
    auto startSquare = std::min(kingSquare, targetKingPosition);
    for (int square = startSquare; square < startSquare + 3; square++) {
        if (isSquareUnderAttack(square)) return false;
    }

    return true;
}

bool Board::isSquareUnderAttack(int square) const {
    return squaresAttackedByOpponent[square];
}

bool Board::allSquaresAreClearBetween(int firstSquare, int secondSquare) {
    int startSquare = std::min(firstSquare, secondSquare);
    int endSquare = std::max(firstSquare, secondSquare);

    for (int square = startSquare + 1; square < endSquare; square++) {
        if (squares[square] != Piece::None) return false;
    }

    return true;
}

void Board::generateSlidingMoves(int startSquare, int piece, MoveProcessor *processor, MoveGenerationStrategy *strategy) {
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

            if (strategy->shouldAddMove(targetPiece, colour))
                processor->processMove(NormalMove(startSquare, targetSquare, targetPiece));

            if (strategy->shouldStopGeneratingSlidingMoves(targetPiece, colour))
                break;
        }
    }
}

static bool isPawnAboutToPromote(int position, int piece) {
    int colour = Piece::getColour(piece);
    int rank = position / 8;

    return (rank == 6 && colour == Piece::White)
           || rank == 1 && colour == Piece::Black;
}

void Board::generatePawnMoves(int startSquare, int piece, MoveProcessor *processor) {
    auto isAboutToPromote = isPawnAboutToPromote(startSquare, piece);

    generateForwardPawnMoves(startSquare, piece, processor, isAboutToPromote);
    generateCapturePawnMoves(startSquare, piece, processor, isAboutToPromote, false);
    generateEnPassantMoves(startSquare, piece, processor);
}

static int getPawnRank(int pawnPiece) {
    int colour = Piece::getColour(pawnPiece);

    if (colour == Piece::None) throw std::invalid_argument("Expected a pawn with a colour, got one with Piece::None");

    return Piece::getColour(pawnPiece) == Piece::White ? 1 : 6;
}

static bool isPawnAtStartSquare(int square, int piece) {
    int rank = square / 8;
    return rank == getPawnRank(piece);
}

void Board::generateForwardPawnMoves(int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote) {
    int possibleOffsets[]{8, 16};

    if (!isSquareInFrontClear(startSquare, piece)) return;

    for (int pawnOffset: possibleOffsets) {
        int offset = Piece::getColour(piece) == Piece::White ? pawnOffset : -pawnOffset;

        if (pawnOffset == 16) {
            if (!isPawnAtStartSquare(startSquare, piece)) continue;
            int pieceInFront = squares[startSquare + offset / 2];
            if (pieceInFront != Piece::None) continue;
        }

        int targetSquarePosition = startSquare + offset;

        if (!isValidSquarePosition(targetSquarePosition)) continue;

        int targetPiece = squares[targetSquarePosition];

        if (targetPiece != Piece::None) continue;
        generatePawnMove(startSquare, targetSquarePosition, isPawnAboutToPromote, Piece::None, processor);
    }
}

bool Board::isSquareInFrontClear(int startSquare, int piece) {
    int positionOfPieceInFront = startSquare + (Piece::getColour(piece) == Piece::White ? 8 : -8);
    return squares[positionOfPieceInFront] == Piece::None;
}

void Board::generateCapturePawnMoves(int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote,
                                     bool canCaptureFriendly) {
    int file = startSquare % 8;

    for (int captureOffset: pawnCaptureOffsets) {
        int offset = Piece::getColour(piece) == Piece::White ? captureOffset : -captureOffset;

        if (offset == 7 && file == 0 || offset == 9 && file == 7
            || offset == -9 && file == 0 || offset == -7 && file == 7)
            continue;
        int targetSquarePosition = startSquare + offset;

        if (!isValidSquarePosition(targetSquarePosition)) continue;
        int targetPiece = squares[targetSquarePosition];

        if (Piece::getColour(targetPiece) == Piece::getOpponentColourFromPiece(piece) || canCaptureFriendly)
            generatePawnMove(startSquare, targetSquarePosition, isPawnAboutToPromote, targetPiece, processor);
    }
}

void Board::generateNormalPawnCaptures(int startSquare, int piece, MoveProcessor *processor) {
    bool isAboutToPromote = isPawnAboutToPromote(startSquare, piece);
    generateCapturePawnMoves(startSquare, piece, processor, isAboutToPromote, false);
    generateEnPassantMoves(startSquare, piece, processor);
}

void Board::generateKnightMoves(int startSquare, int piece, MoveProcessor *processor, MoveGenerationStrategy *strategy) {
    int file = startSquare % 8;

    for (auto offset: knightMoveOffsets) {
        if (file >= 6 && (offset == -6 || offset == 10)
            || file == 7 && (offset == 17 || offset == -15)
            || file <= 1 && (offset == 6 || offset == -10)
            || file == 0 && (offset == -17 || offset == 15))
            continue;

        int targetSquarePosition = startSquare + offset;
        if (!isValidSquarePosition(targetSquarePosition)) continue;

        int pieceInTargetSquare = squares[targetSquarePosition];

        if (strategy->shouldAddMove(pieceInTargetSquare, Piece::getColour(piece)))
            processor->processMove(NormalMove(startSquare, targetSquarePosition, pieceInTargetSquare));
    }
}

static int getRank(int square) {
    return square / 8;
}

void Board::generateEnPassantMoves(int square, int piece, MoveProcessor *processor) {
    auto file = square % 8;
    auto rank = square / 8;

    if (rank != 3 && rank != 4) return;

    int enPassantOffsets[]{-1, 1};

    for (int offset: enPassantOffsets) {
        if (offset == 1 && file == 7 || offset == -1 && file == 0) continue;

        int neighbourPosition = square + offset;

        if (!isValidSquarePosition(neighbourPosition)) return;

        int neighbourPiece = squares[neighbourPosition];
        int enemyPawn = Piece::Pawn | Piece::getOpponentColourFromPiece(piece);

        if (moveHistory.empty()) continue;

        auto lastMove = moveHistory.top();
        auto basicLastMove = visit(getBasicMoveVisitor, lastMove);

        if (neighbourPiece != enemyPawn
            || getRank(basicLastMove.startSquare) != getPawnRank(neighbourPiece)
            || basicLastMove.targetSquare != neighbourPosition)
            continue;

        int targetPositionOffset = Piece::getColour(piece) == Piece::White ? 8 : -8;

        processor->processMove(
                EnPassantMove(square, neighbourPosition + targetPositionOffset, neighbourPiece, neighbourPosition)
        );
    }
}

bool Board::isInEndgame() const {
    return _isInEndgame;
}

template<typename T>
bool contains(std::array<T, 64> array, T element) {
    return std::find(array.begin(), array.end(), element) != array.end();
}

bool Board::determineIfIsInEndgame() const {
    return isSideInEndgamePosition(Piece::White) && isSideInEndgamePosition(Piece::Black);
}

bool Board::isSideInEndgamePosition(int colour) const {
    bool hasQueen = contains(squares, Piece::Queen | colour);
    bool hasRook = contains(squares, Piece::Rook | colour);
    return !hasQueen || !hasRook && getMinorPieceCount(colour) <= 1;
}

unsigned long Board::getMinorPieceCount(int colour) const {
    return std::count_if(squares.begin(), squares.end(), [colour](int piece) {
        return piece == (Piece::Knight | colour) || piece == (Piece::Bishop | colour);
    });
}

int Board::_getKingSquare(int colour) const {
    for (int square = 0; square < 64; square++) {
        if (squares[square] == (Piece::King | colour)) return square;
    }

    return -1;
}

int Board::_getKingSquare() const {
    return _getKingSquare(colourToMove);
}

int Board::_getOpponentKingSquare() const {
    return _getKingSquare(Piece::getOpponentColour(colourToMove));
}

int Board::getKingSquare() const {
    return kingSquare;
}

int Board::getOpponentKingSquare() const {
    return opponentKingSquare;
}

void Board::generateCaptures() {
    legalMoves.clear();

    int colourToMove = this->colourToMove;
    generateSquaresAttackedByOpponent(Piece::getOpponentColour(colourToMove));
    isKingUnderAttack = IsKingUnderAttack();

    generateCheckSolvingMovePositions();
    generatePins();

    generateLegalCaptures(colourToMove);

    hasLegalMoves = !legalMoves.empty();
}

Board::~Board() {
    delete moveGenerationProcessor;
    delete legalMoveSearchProcessor;
    delete attackedSquaresGenerationProcessor;
}
