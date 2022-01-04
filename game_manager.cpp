#include "game_manager.h"
#include "dragwidget.h"

void GameManager::setup(DragWidget *wdg) {
    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None)
            pieces.push_back(new UiPiece(dynamic_cast<QWidget *>(wdg), square, piece));
    }
}
