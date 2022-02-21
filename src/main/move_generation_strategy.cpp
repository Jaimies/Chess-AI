#include "move_generation_strategy.h"

MoveGenerationStrategy *NormalMoveGenerationStrategy = new _NormalMoveGenerationStrategy();
MoveGenerationStrategy *CaptureGenerationStrategy = new _CaptureGenerationStrategy();
MoveGenerationStrategy *AttackedSquaresGenerationStrategy = new _AttackedSquaresGenerationStrategy();
