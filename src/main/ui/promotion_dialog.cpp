#include "promotion_dialog.h"
#include "../board/piece.h"
#include "piece_ui.h"
#include <QMouseEvent>
#include <functional>

PromotionDialog::PromotionDialog(QWidget *wdg) : QWidget(wdg) {
    for (int i = 0; i < 4; i++) {
        auto uiPiece = new UiPiece(this, i, Piece::piecesToPromoteTo[i] | Piece::Black, false);
        uiPiece->move(i * 100 + 5, 5);
        pieces.push_back(uiPiece);
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
    for (int i = 0; i < 4; i++) {
        pieces[i]->setPiece(Piece::piecesToPromoteTo[i] | color);
    }
}

void PromotionDialog::mouseReleaseEvent(QMouseEvent *event) {
    int index = event->pos().x() / 100;
    auto pieceToPromoteTo = Piece::piecesToPromoteTo[index];
    this->onPieceSelected(pieceToPromoteTo);
}

PromotionDialogOverlay::PromotionDialogOverlay(QWidget *wdg) : QWidget(wdg) {
    auto thisAsWidget = static_cast<QWidget *>(this);
    thisAsWidget->setVisible(false);
    thisAsWidget->setFixedSize(800, 800);
    thisAsWidget->setObjectName("promotionDialogBackground");
    thisAsWidget->setStyleSheet("#promotionDialogBackground { background-color: rgba(255, 255, 255, 140); }");
}

void PromotionDialogOverlay::setOnClickListener(std::function<void()> onPieceSelected) {
    this->onPieceSelected = onPieceSelected;
}

void PromotionDialogOverlay::mouseReleaseEvent(QMouseEvent *event) {
    if (this->onPieceSelected) this->onPieceSelected();
}
