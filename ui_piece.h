#pragma once

#include "icon.h"
#include <QWidget>

class UiPiece : public Icon {
public:
    UiPiece(QWidget *parent, int square, int piece) : Icon(parent), square(square) {
        setPiece(piece);
        moveToSquare(square);
        setFixedSize(70, 70);
        show();
    };

    void setPiece(int piece);
    void moveToSquare(int square);
    int getSquare();

private:
    int square;
};
