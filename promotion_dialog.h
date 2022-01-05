#pragma once

#include <QWidget>
#include "ui_piece.h"

class PromotionDialog : public QWidget
{
public:
    PromotionDialog(QWidget *wdg);

    void show(int color, int square, std::function<void(int)> onPieceSelected);
    void setColor(int color);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    std::vector<UiPiece *> pieces;
    std::function<void(int)> onPieceSelected;
};
