#include "evaluation_update_strategy.h"
#include "transposition.h"

void EvaluationUpdateStrategy::_updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType) {
    if (evaluation >= beta) {
        shouldExit = true;
        nodeType = Transposition::LOWER;
        alpha = beta;
        return;
    }
    if (evaluation > alpha) {
        alpha = evaluation;
        if (nodeType == Transposition::UPPER)
            nodeType = Transposition::EXACT;
    }
}

void ParallelizedUpdateStrategy::updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType) {
    mutex.lock();
    _updateEvaluation(evaluation, shouldExit, alpha, beta, nodeType);
    mutex.unlock();
}

void NonParallelizedUpdateStrategy::updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType) {
    _updateEvaluation(evaluation, shouldExit, alpha, beta, nodeType);
}
