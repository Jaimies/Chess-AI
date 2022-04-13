#include "game_manager.h"
#include "chessboard.h"
#include "promotion_dialog.h"
#include "../ai/move_generator.h"
#include "../util/thread_util.h"

void GameManager::setup(ChessBoardWidget *wdg, AnalysisInfoDisplay *info) {
    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None)
            pieces.push_back(new UiPiece(dynamic_cast<QWidget *>(wdg), square, piece));
    }

    this->info = info;
    this->wdg = wdg;
    promotionDialogBackground = new PromotionDialogOverlay(wdg);
    promotionDialog = new PromotionDialog(wdg);
    promotionDialog->setVisible(false);
}

void GameManager::makeMove(Move *move, bool isMachineMove) {
    auto color = Piece::getColour(board->squares[move->startSquare]);
    auto promotionMove = dynamic_cast<PromotionMove *>(move);

    auto pieceToMove = getPieceAtSquare(move->startSquare);
    auto pieceToCapture = getPieceAtSquare(move->targetSquare);

    if (promotionMove) {
        if (promotionDialog && !isMachineMove) {
            promotionDialog->show(color, move->targetSquare, [this, move, color](int pieceType) {
                auto piece = pieceType | color;
                MoveVariant moveToMake = PromotionMove(move->startSquare, move->targetSquare, piece);
                board->makeMove(moveToMake);
                getPieceAtSquare(move->targetSquare)->setPiece(piece);
                promotionDialog->setVisible(false);
                promotionDialogBackground->setVisible(false);
                makeMachineMoveIfNecessary();
            });
            promotionDialogBackground->setOnClickListener([this, move, pieceToMove, pieceToCapture]() {
                if (pieceToCapture) pieceToCapture->setVisible(true);
                pieceToMove->moveToSquare(move->startSquare);
                promotionDialog->setVisible(false);
                promotionDialogBackground->setVisible(false);
            });

            promotionDialogBackground->setVisible(true);
            if (pieceToCapture) pieceToCapture->removeFromBoard();
            pieceToMove->moveToSquare(move->targetSquare);
        }

        if (isMachineMove) {
            board->makeMove(move);
            if (pieceToCapture) pieceToCapture->removeFromBoard();
            pieceToMove->moveToSquare(move->targetSquare);
            pieceToMove->setPiece(promotionMove->pieceToPromoteTo);
        }
        return;
    }

    board->makeMove(move);

    if (move->getCapturedSquare() != -1) {
        auto piece = getPieceAtSquare(move->getCapturedSquare());
        if (piece) {
            piece->removeFromBoard();
        }
    }

    auto castlingMove = dynamic_cast<CastlingMove *>(move);

    if (castlingMove)
        getPieceAtSquare(castlingMove->rookSquare)->moveToSquare(castlingMove->rookTargetSquare);

    pieceToMove->moveToSquare(move->targetSquare);
    makeMachineMoveIfNecessary();
}

void findTheBestMove(Board *board, GameManager *gameManager) {
    auto generator = new MoveGenerator();
    auto machineMove = generator->getBestMove(board);
    gameManager->makeMove(machineMove, true);
    gameManager->info->updateInfo(generator->analysisInfo);
    deleteInTheBackground(generator);
}

void GameManager::makeMachineMoveIfNecessary() {
    if (board->colourToMove == Piece::Black) {
        info->showAnalysisActive();
        startThreadAndForget(findTheBestMove, board, this);
    }
}

UiPiece *GameManager::getPieceAtSquare(int square) {
    for (auto piece: pieces) {
        if (piece->getSquare() == square) return piece;
    }

    return nullptr;
}

void GameManager::undoLastMove() {
    board->unmakeMove(board->getLastMove());
    board->unmakeMove(board->getLastMove());

    board->generateMoves();

    for (auto piece: pieces) delete piece;
    pieces.clear();

    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None)
            pieces.push_back(new UiPiece(dynamic_cast<QWidget *>(wdg), square, piece));
    }
}
