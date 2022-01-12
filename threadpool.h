#pragma once

#include <thread>
#include <functional>
#include <mutex>

class ThreadPool {
public:
    void execute(const std::function<void()>& action);
    void awaitAll();

private :
    std::mutex mutex;
    std::vector<std::thread *> threads;
    unsigned int threadCount = std::thread::hardware_concurrency();
    unsigned int activeThreadCount = 0;
};