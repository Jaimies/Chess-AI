#pragma once

#include "icon.h"
#include <QWidget>

class UiPiece : public Icon {
public:
    UiPiece(QWidget *parent, int square) : Icon(parent), square(square) {};

    const int square;
};