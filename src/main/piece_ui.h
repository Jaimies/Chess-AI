#pragma once

#include "icon.h"
#include <QWidget>

class UiPiece : public Icon {
public:
    UiPiece(QWidget *parent, int square, int piece, bool isDraggable = true)
        : Icon(parent), isDraggable(isDraggable), square(square) {
        setPiece(piece);
        moveToSquare(square);
        setFixedSize(70, 70);
        show();
    };

    const bool isDraggable;

    void setPiece(int piece);
    void moveToSquare(int square);
    int getSquare();
    void removeFromBoard();

private:
    int square;
};
