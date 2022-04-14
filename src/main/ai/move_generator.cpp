#include <thread>
#include <mutex>
#include "../board/piece.h"
#include "../move/move.h"
#include "../board/zobrist_hash_generator.h"
#include "move_generator.h"
#include "../util/vector_util.h"
#include "single_depth_move_generator.h"

Move *MoveGenerator::getBestMove(Board *board, AiSettings settings) {
    using namespace std::chrono;

    thread = new std::thread([board, settings, this]() {
        int depth = 1;

        while (!analysisFinished) {
            Board *boardCopy = board->copy();
            auto generator = new SingleDepthMoveGenerator(this, boardCopy, depth);
            auto supposedBestMove = generator->getBestMove(bestMove, settings);
            delete generator;
            delete boardCopy;
            if (analysisFinished) break;
            bestMove = supposedBestMove;
            auto millisCount = duration_cast<milliseconds>(steady_clock::now() - begin).count();
            analysisInfo = new AnalysisInfo{positionsAnalyzed, depth + 1, bestMove, millisCount};
            depth++;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (!bestMove) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    analysisFinished = true;

    return bestMove;
}
