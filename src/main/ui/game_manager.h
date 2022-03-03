#pragma once

#include "../board/Board.h"
#include "piece_ui.h"
#include "promotion_dialog.h"
#include "analysis_info_display.h"
#include <vector>

class ChessBoardWidget;
class AnalysisInfoDisplay;

class GameManager {
public:
    Board *board = Board::fromFenString(Board::startPosition);

    explicit GameManager() {};
    void setup(ChessBoardWidget *wdg, AnalysisInfoDisplay *info);
    void makeMove(Move *move, bool isMachineMove = false);
    void unmakeMove(Move *move);
    void undoLastMove();

    AnalysisInfoDisplay *info;
    QWidget *wdg;
private:
    std::vector<UiPiece *> pieces;
    PromotionDialog *promotionDialog;
    PromotionDialogOverlay *promotionDialogBackground;

    UiPiece *getPieceAtSquare(int square);
    void makeMachineMoveIfNecessary();
};
