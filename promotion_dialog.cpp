#include "promotion_dialog.h"
#include "Piece.h"
#include "ui_piece.h"
#include <QMouseEvent>
#include <functional>

PromotionDialog::PromotionDialog(QWidget *wdg) : QWidget(wdg){
    for(int i = 0; i < 4; i++) {
        pieces.push_back(new UiPiece(this, i, Piece::piecesToPromoteTo[i] | Piece::White, false));
    }

    setFixedSize(400, 100);

    setObjectName("promotionDialog");
    setStyleSheet("#promotionDialog { background-color: #fff; }");
}

void PromotionDialog::show(int color, int square, std::function<void(int)> onPieceSelected) {
    setColor(color);
    setVisible(true);
    int promotionRank = square / 8;
    int promotionFile = square % 8;
    move(std::min(promotionFile * 100, 400), promotionRank * 100);
    this->onPieceSelected = onPieceSelected;
}

void PromotionDialog::setColor(int color) {
    for(int i = 0; i < 4; i++) {
        pieces[i] = new UiPiece(this, i, Piece::piecesToPromoteTo[i] | color, false);
    }
}

void PromotionDialog::mouseReleaseEvent(QMouseEvent *event) {
    int index = event->pos().x() / 100;
    auto pieceToPromoteTo = Piece::piecesToPromoteTo[index];
    this->onPieceSelected(pieceToPromoteTo);
}
