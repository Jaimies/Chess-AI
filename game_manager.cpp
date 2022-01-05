#include "game_manager.h"
#include "dragwidget.h"
#include "promotion_dialog.h"

void GameManager::setup(DragWidget *wdg) {
    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None)
            pieces.push_back(new UiPiece(dynamic_cast<QWidget *>(wdg), square, piece));
    }

    promotionDialogBackground = new PromotionDialogOverlay(wdg);
    promotionDialog = new PromotionDialog(wdg);
    promotionDialog->setVisible(false);
}

void GameManager::makeMove(Move *move) {
    auto color = Piece::getColour(board->squares[move->startSquare]);
    auto promotionMove = dynamic_cast<PromotionMove *>(move);

    auto pieceToMove = getPieceAtSquare(move->startSquare);
    auto pieceToCapture = getPieceAtSquare(move->targetSquare);

    if (promotionMove) {
        if (promotionDialog) {

            promotionDialog->show(color, move->targetSquare, [this, move, color](int pieceType) {
                auto piece = pieceType | color;
                board->makeMove(new PromotionMove(move->startSquare, move->targetSquare, piece));
                getPieceAtSquare(move->targetSquare)->setPiece(piece);
                promotionDialog->setVisible(false);
                promotionDialogBackground->setVisible(false);
            });
            promotionDialogBackground->setOnClickListener([this, move, pieceToMove, pieceToCapture]() {
                pieceToCapture->setVisible(true);
                pieceToMove->moveToSquare(move->startSquare);
                promotionDialog->setVisible(false);
                promotionDialogBackground->setVisible(false);
            });

            promotionDialogBackground->setVisible(true);
            pieceToCapture->setVisible(false);
            pieceToMove->moveToSquare(move->targetSquare);
        }
        return;
    }

    board->makeMove(move);

    if (move->getCapturedSquare() != -1) {
        auto piece = getPieceAtSquare(move->getCapturedSquare());
        if (piece) piece->setVisible(false);
    }

    auto castlingMove = dynamic_cast<CastlingMove *>(move);

    if (castlingMove)
        getPieceAtSquare(castlingMove->rookSquare)->moveToSquare(castlingMove->rookTargetSquare);

    pieceToMove->moveToSquare(move->targetSquare);
}

UiPiece *GameManager::getPieceAtSquare(int square) {
    for (auto piece: pieces) {
        if (piece->getSquare() == square) return piece;
    }

    return nullptr;
}
