#include "constants.h"

Evaluation minEvaluation = std::numeric_limits<Evaluation>::min() + 10;
Evaluation maxEvaluation = std::numeric_limits<Evaluation>::max() - 10;

Evaluation checkmateEvaluation = minEvaluation + 1000;
