#include "game_manager.h"
#include "dragwidget.h"
#include "promotion_dialog.h"

void GameManager::setup(DragWidget *wdg) {
    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None)
            pieces.push_back(new UiPiece(dynamic_cast<QWidget *>(wdg), square, piece));
    }

    promotionDialog = new PromotionDialog(wdg);
    promotionDialog->setVisible(false);
}

void GameManager::makeMove(Move *move) {
    auto promotionMove = dynamic_cast<PromotionMove *>(move);
    if(promotionMove) {
        if (promotionDialog) {
            promotionDialog->setVisible(true);
            getPieceAtSquare(move->startSquare)->moveToSquare(move->targetSquare);
            int promotionRank = move->targetSquare / 8;
            int promotionFile = move->targetSquare % 8;
            promotionDialog->move(std::min(promotionFile * 100, 400), promotionRank * 100);
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

    getPieceAtSquare(move->startSquare)->moveToSquare(move->targetSquare);
}

UiPiece *GameManager::getPieceAtSquare(int square) {
    for (auto piece: pieces) {
        if (piece->getSquare() == square) return piece;
    }

    return nullptr;
}
