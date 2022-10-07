#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include "piece.h"
#include "../move/move.h"
#include "board.h"
#include "board_util.h"
#include "zobrist_hash_generator.h"

static int getCastlingPiece(int piece, int square) {
    auto type = Piece::getType(piece);
    if (type == Piece::King) return piece;
    if (type != Piece::Rook) return Piece::None;

    int file = square % 8;
    int colour = Piece::getColour(piece);

    if (!BoardUtil::isRookAtStartRank(square, colour)) return Piece::None;

    if (file == 0) return Piece::LeftRook | colour;
    if (file == 7) return Piece::RightRook | colour;

    return Piece::None;
}

static void generatePawnMove(int startSquare, int targetSquare, bool isPawnAboutToPromote, int pieceToCapture,
                             MoveProcessor *processor) {
    if (!isPawnAboutToPromote) {
        processor->processMove(NormalMove{startSquare, targetSquare, pieceToCapture});
        return;
    }

    for (auto piece: Piece::piecesToPromoteTo)
        processor->processMove(PromotionMove{startSquare, targetSquare, piece, pieceToCapture});
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
    kingSquare = _getKingSquare();
    moveHistory.pop();
}

Board *Board::copy() const {
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

            if (Piece::getColour(targetPiece) == colour) break;

            if (potentialPinnedPiecePosition == -1) {
                if (Piece::getColour(targetPiece) == opponentColour
                    && Piece::getType(targetPiece) != Piece::King)
                    potentialPinnedPiecePosition = targetSquare;

                continue;
            }

            if (targetPiece == Piece::None) continue;

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

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, moveGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Pawn) generatePawnMoves(startSquare, piece, moveGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, moveGenerationProcessor);
    }

    generateCastlingMoves(moveGenerationProcessor);
}

void Board::generateLegalCaptures(int color) {
    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        if (Piece::getColour(piece) != color) continue;

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, captureGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Pawn) generateNormalPawnCaptures(startSquare, piece, captureGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, captureGenerationProcessor);
    }
}

void Board::generateSquaresAttackedByOpponent(int colour) {
    for (int square = 0; square < 64; square++)
        squaresAttackedByOpponent[square] = false;

    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        attacksKing[startSquare] = false;
        if (Piece::getColour(piece) != colour) continue;

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, attackedSquaresGenerationProcessor);
        else if (Piece::getType(piece) == Piece::Pawn) generateCapturePawnMoves(startSquare, piece, attackedSquaresGenerationProcessor, false, true);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, attackedSquaresGenerationProcessor);
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
}

void Board::updateCastlingPieceMovement(MoveVariant &move) {
    auto basicMove = visit(GetBasicMoveVisitor, move);
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

bool Board::isMoveLegal(MoveVariant potentialMove) const {
    if (visit(IsCastlingMoveVisitor, potentialMove)) return true;

    auto enPassantMove = visit(GetEnPassantMoveVisitor, potentialMove);

    return !violatesPin(potentialMove)
           && (!IsKingUnderAttack(potentialMove) || coversCheck(potentialMove) ||
               (enPassantMove.has_value() && enPassantMove->capturedPawnPosition == kingAttackerPosition))
           && (!enPassantMove.has_value() || isValidEnPassantMove(*enPassantMove));
}

void Board::legalMovesExist(int colour) {
    for (int startSquare = 0; startSquare < 64; startSquare++) {
        int piece = squares[startSquare];
        if (Piece::getColour(piece) != colour) continue;

        if (Piece::isSlidingPiece(piece)) generateSlidingMoves(startSquare, piece, legalMoveSearchProcessor);
        else if (Piece::getType(piece) == Piece::Pawn) generatePawnMoves(startSquare, piece, legalMoveSearchProcessor);
        else if (Piece::getType(piece) == Piece::Knight) generateKnightMoves(startSquare, piece, legalMoveSearchProcessor);

        if (hasLegalMoves) return;
    }
}

bool Board::IsKingUnderAttack() const {
    return squaresAttackedByOpponent[kingSquare];
}

bool Board::IsKingUnderAttack(MoveVariant potentialMove) const {
    auto basicMove = visit(GetBasicMoveVisitor, potentialMove);
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
    updateCastlingPieceMovement(move);
    visit(applyMoveVisitor, move);
    changeColourToMove();
    moveHistory.push(move);
    kingSquare = _getKingSquare();
    opponentKingSquare = _getOpponentKingSquare();
    enPassantTargetSquare = -1;
}

bool Board::violatesPin(MoveVariant &move) const {
    auto basicMove = visit(GetBasicMoveVisitor, move);
    if (!pins.contains(basicMove.startSquare)) return false;
    if (Piece::getType(squares[basicMove.startSquare]) == Piece::Knight) return true;

    auto directionIndex = pins.at(basicMove.startSquare);
    auto directionOffset = directionOffsets[directionIndex];

    auto squareDifference = basicMove.targetSquare - basicMove.startSquare;

    if (std::abs(directionOffset) == 1)
        return basicMove.startSquare / 8 != basicMove.targetSquare / 8;

    return squareDifference % directionOffset != 0;
}

bool Board::coversCheck(MoveVariant potentialMove) const {
    auto basicMove = visit(GetBasicMoveVisitor, potentialMove);
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

void Board::generateCastlingMoves(MoveProcessor *processor) const {
    if (colourToMove == Piece::White)
        addCastlingMovesIfAvailable(4, Piece::White, processor);
    else
        addCastlingMovesIfAvailable(60, Piece::Black, processor);
}

void Board::addCastlingMovesIfAvailable(int kingSquare, int colour, MoveProcessor *processor) const {
    int king = squares[kingSquare];

    if (king != (Piece::King | colour)) return;

    if (!castlingPieceMoved.at(king) && !isKingUnderAttack) {
        addCastlingMoveIfPossible(kingSquare, kingSquare - 4, processor);
        addCastlingMoveIfPossible(kingSquare, kingSquare + 3, processor);
    }
}

void Board::addCastlingMoveIfPossible(int kingSquare, int rookSquare, MoveProcessor *processor) const {
    int directionMultiplier = rookSquare > kingSquare ? 1 : -1;
    int kingTargetSquare = kingSquare + 2 * directionMultiplier;
    int rookTargetSquare = kingSquare + 1 * directionMultiplier;

    if (isCastlingPossible(kingSquare, rookSquare, kingTargetSquare))
        processor->processMove(CastlingMove{kingSquare, kingTargetSquare, rookSquare, rookTargetSquare});
}

bool Board::isCastlingPossible(int kingSquare, int rookSquare, int targetCastlingPosition) const {
    int king = squares[kingSquare];
    int rook = squares[rookSquare];

    int colour = Piece::getColour(king);
    int rookType = rookSquare < kingSquare ? Piece::LeftRook : Piece::RightRook;

    return rook == (Piece::Rook | colour) &&
           !castlingPieceMoved.at(rookType | colour) &&
           allSquaresAreClearBetween(kingSquare, rookSquare) &&
           allSquaresAreNotUnderAttackBetween(kingSquare, targetCastlingPosition);
}

bool Board::allSquaresAreNotUnderAttackBetween(int kingSquare, int targetKingPosition) const {
    auto startSquare = std::min(kingSquare, targetKingPosition);
    for (int square = startSquare; square < startSquare + 3; square++) {
        if (isSquareUnderAttack(square)) return false;
    }

    return true;
}

bool Board::isSquareUnderAttack(int square) const {
    return squaresAttackedByOpponent[square];
}

bool Board::allSquaresAreClearBetween(int firstSquare, int secondSquare) const {
    int startSquare = std::min(firstSquare, secondSquare);
    int endSquare = std::max(firstSquare, secondSquare);

    for (int square = startSquare + 1; square < endSquare; square++) {
        if (squares[square] != Piece::None) return false;
    }

    return true;
}

void Board::generateSlidingMoves(int startSquare, int piece, MoveProcessor *processor) const {
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

            if (processor->shouldAddMove(targetPiece, colour))
                processor->processMove(NormalMove{startSquare, targetSquare, targetPiece});

            if (processor->shouldStopGeneratingSlidingMoves(targetPiece, colour))
                break;
        }
    }
}

void Board::generatePawnMoves(int startSquare, int piece, MoveProcessor *processor) const {
    auto isAboutToPromote = BoardUtil::isPawnAboutToPromote(startSquare, piece);

    generateForwardPawnMoves(startSquare, piece, processor, isAboutToPromote);
    generateCapturePawnMoves(startSquare, piece, processor, isAboutToPromote, false);
    generateEnPassantMoves(startSquare, piece, processor);
}

void Board::generateForwardPawnMoves(int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote) const {
    int possibleOffsets[]{8, 16};

    if (!isSquareInFrontClear(startSquare, piece)) return;

    for (int pawnOffset: possibleOffsets) {
        int offset = Piece::isWhite(piece) ? pawnOffset : -pawnOffset;

        if (pawnOffset == 16) {
            if (!BoardUtil::isPawnAtStartSquare(startSquare, piece)) continue;
            int pieceInFront = squares[startSquare + offset / 2];
            if (pieceInFront != Piece::None) continue;
        }

        int targetSquarePosition = startSquare + offset;

        if (!BoardUtil::isValidSquare(targetSquarePosition)) continue;

        int targetPiece = squares[targetSquarePosition];

        if (targetPiece != Piece::None) continue;
        generatePawnMove(startSquare, targetSquarePosition, isPawnAboutToPromote, Piece::None, processor);
    }
}

bool Board::isSquareInFrontClear(int startSquare, int piece) const {
    int positionOfPieceInFront = startSquare + (Piece::isWhite(piece) ? 8 : -8);
    return squares[positionOfPieceInFront] == Piece::None;
}

void Board::generateCapturePawnMoves(int startSquare, int piece, MoveProcessor *processor, bool isPawnAboutToPromote,
                                     bool canCaptureFriendly) const {
    int file = startSquare % 8;

    for (int captureOffset: pawnCaptureOffsets) {
        int offset = Piece::isWhite(piece) ? captureOffset : -captureOffset;

        if (offset == 7 && file == 0 || offset == 9 && file == 7
            || offset == -9 && file == 0 || offset == -7 && file == 7)
            continue;
        int targetSquarePosition = startSquare + offset;

        if (!BoardUtil::isValidSquare(targetSquarePosition)) continue;
        int targetPiece = squares[targetSquarePosition];

        if (Piece::getColour(targetPiece) == Piece::getOpponentColourFromPiece(piece) || canCaptureFriendly)
            generatePawnMove(startSquare, targetSquarePosition, isPawnAboutToPromote, targetPiece, processor);
    }
}

void Board::generateNormalPawnCaptures(int startSquare, int piece, MoveProcessor *processor) const {
    bool isAboutToPromote = BoardUtil::isPawnAboutToPromote(startSquare, piece);
    generateCapturePawnMoves(startSquare, piece, processor, isAboutToPromote, false);
    generateEnPassantMoves(startSquare, piece, processor);
}

void Board::generateKnightMoves(int startSquare, int piece, MoveProcessor *processor) const {
    int file = startSquare % 8;

    for (auto offset: knightMoveOffsets) {
        if (file >= 6 && (offset == -6 || offset == 10)
            || file == 7 && (offset == 17 || offset == -15)
            || file <= 1 && (offset == 6 || offset == -10)
            || file == 0 && (offset == -17 || offset == 15))
            continue;

        int targetSquarePosition = startSquare + offset;
        if (!BoardUtil::isValidSquare(targetSquarePosition)) continue;

        int pieceInTargetSquare = squares[targetSquarePosition];

        if (processor->shouldAddMove(pieceInTargetSquare, Piece::getColour(piece)))
            processor->processMove(NormalMove{startSquare, targetSquarePosition, pieceInTargetSquare});
    }
}

void Board::generateEnPassantMoves(int square, int piece, MoveProcessor *processor) const {
    auto file = square % 8;
    auto rank = square / 8;

    if (rank != 3 && rank != 4) return;

    int enPassantOffsets[]{-1, 1};

    for (int offset: enPassantOffsets) {
        if (offset == 1 && file == 7 || offset == -1 && file == 0) continue;

        int neighbourPosition = square + offset;

        if (!BoardUtil::isValidSquare(neighbourPosition)) return;

        int neighbourPiece = squares[neighbourPosition];
        int enemyPawn = Piece::Pawn | Piece::getOpponentColourFromPiece(piece);

        if (moveHistory.empty()) continue;

        auto lastMove = moveHistory.top();
        auto basicLastMove = visit(GetBasicMoveVisitor, lastMove);

        if (neighbourPiece != enemyPawn
            || BoardUtil::rank(basicLastMove.startSquare) != BoardUtil::initialRankOfPawn(neighbourPiece)
            || basicLastMove.targetSquare != neighbourPosition)
            continue;

        int targetPositionOffset = Piece::isWhite(piece) ? 8 : -8;

        processor->processEnPassantMove(
                {square, neighbourPosition + targetPositionOffset, neighbourPiece, neighbourPosition}
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
    delete captureGenerationProcessor;
    delete legalMoveSearchProcessor;
    delete attackedSquaresGenerationProcessor;
}

void Board::makeMove(Move *move) {
    MoveVariant variant = move->toVariant();
    makeMove(variant);
}

void Board::unmakeMove(Move *move) {
    MoveVariant variant = move->toVariant();
    unmakeMove(variant);
}

Move *Board::getLastMove() {
    return visit(GetMovePointerVisitor, moveHistory.top());
}

char getPieceLetter(int type) {
    if (type == Piece::Pawn) return 'p';
    if (type == Piece::Rook) return 'r';
    if (type == Piece::Queen) return 'q';
    if (type == Piece::Bishop) return 'b';
    if (type == Piece::Knight) return 'n';
    if (type == Piece::King) return 'k';
    return '1';
}

uint64_t Board::getZobristHash() const {
    return ZobristHashGenerator.hash(this);
}

std::string Board::toFenString() const {
    std::string output;
    uint32_t emptyPieceCount = 0;

    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            auto square = rank * 8 + file;
            auto piece = squares[square];

            if (piece == Piece::None) {
                emptyPieceCount++;
            } else {
                if (emptyPieceCount > 0) {
                    output += std::to_string(emptyPieceCount);
                    emptyPieceCount = 0;
                }

                auto type = Piece::getType(piece);
                auto pieceLetter = getPieceLetter(type);
                char finalChar = Piece::isWhite(piece) ? toupper(pieceLetter) : pieceLetter;
                output += finalChar;
            }
        }

        if (emptyPieceCount > 0) {
            output += std::to_string(emptyPieceCount);
            emptyPieceCount = 0;
        }

        output += "/";
    }

    return output;
}

bool Board::canWhiteCastleLeft() const {
    return !castlingPieceMoved.at(Piece::King | Piece::White)
           && !castlingPieceMoved.at(Piece::LeftRook | Piece::White);
}

bool Board::canWhiteCastleRight() const {
    return !castlingPieceMoved.at(Piece::King | Piece::White)
           && !castlingPieceMoved.at(Piece::RightRook | Piece::White);
}

bool Board::canBlackCastleLeft() const {
    return !castlingPieceMoved.at(Piece::King | Piece::Black)
           && !castlingPieceMoved.at(Piece::LeftRook | Piece::Black);
}

bool Board::canBlackCastleRight() const {
    return !castlingPieceMoved.at(Piece::King | Piece::Black)
           && !castlingPieceMoved.at(Piece::RightRook | Piece::Black);
}
