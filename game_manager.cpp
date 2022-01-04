#include "game_manager.h"
#include "dragwidget.h"

void GameManager::setup(DragWidget *wdg) {
    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None)
            pieces.push_back(new UiPiece(dynamic_cast<QWidget *>(wdg), square, piece));
    }
}

void GameManager::makeMove(Move *move) {
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
