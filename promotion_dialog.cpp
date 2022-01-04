#include "promotion_dialog.h"
#include "Piece.h"
#include "ui_piece.h"

PromotionDialog::PromotionDialog(QWidget *wdg) : QWidget(wdg){
    for(int i = 0; i < 4; i++) {
        pieces.push_back(new UiPiece(this, i, Piece::piecesToPromoteTo[i] | Piece::White, false));
    }

    setFixedSize(400, 100);

    setObjectName("promotionDialog");
    setStyleSheet("#promotionDialog { background-color: #fff; }");
}

void PromotionDialog::setColor(int color) {
    for(int i = 0; i < 4; i++) {
        pieces[i] = new UiPiece(this, i, Piece::piecesToPromoteTo[i] | color, false);
    }
}
