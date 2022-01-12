#include "threadpool.h"

void ThreadPool::execute(const std::function<void()> &action) {
    if (activeThreadCount < threadCount)
        threads.push_back(new std::thread([this, action]() {
            action();
            mutex.lock();
            activeThreadCount--;
            mutex.unlock();
        }));
    else
        action();

    mutex.lock();
    activeThreadCount++;
    mutex.unlock();
}

void ThreadPool::awaitAll() {
    for(auto thread: threads) {
        thread->join();
        delete thread;
    }

    activeThreadCount = 0;
    threads.clear();
}
