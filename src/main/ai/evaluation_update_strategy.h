#pragma once

#include <cstdint>
#include <mutex>

class EvaluationUpdateStrategy {
public:
    virtual void updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType) = 0;

protected:
    static void _updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType);
};

class ParallelizedUpdateStrategy : public EvaluationUpdateStrategy {
    std::mutex mutex = std::mutex();

public:
    void updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType) override;
};

class NonParallelizedUpdateStrategy : public EvaluationUpdateStrategy {
public:
    void updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta, int &nodeType) override;
};
