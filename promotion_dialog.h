#pragma once

#include <QWidget>
#include "ui_piece.h"

class PromotionDialog : public QWidget
{
public:
    PromotionDialog(QWidget *wdg);

    void setColor(int color);

private:
    std::vector<UiPiece *> pieces;
};
