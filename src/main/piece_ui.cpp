#include "piece_ui.h"
#include <QIcon>
#include <QWidget>
#include <QLayout>
#include "Piece.h"

std::map<int, std::string> iconNames{
        {1, "king"},
        {2, "queen"},
        {3, "bishop"},
        {4, "knight"},
        {5, "rook"},
        {6, "pawn"},
};

void UiPiece::moveToSquare(int square) {
    int rank = square / 8;
    int file = square % 8;

    this->move(file * 100 + 15, rank * 100 + 15);
    this->square = square;
}

void UiPiece::setPiece(int piece) {
    int pieceType = Piece::getType(piece);
    auto iconName = iconNames[pieceType];
    auto iconColor = (Piece::getColour(piece) == Piece::White) ? "white" : "black";
    auto iconPath = ":/images/" + iconName + "_" + iconColor + ".svg";
    auto pixmap = QIcon(iconPath.c_str()).pixmap(QSize(70, 70));
    this->setPixmap(pixmap);
}

int UiPiece::getSquare() { return this->square; }

void UiPiece::removeFromBoard() {
    setVisible(false);
    this->square = -1;
}
