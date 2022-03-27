#pragma once

#include <thread>

template<typename Callable, typename... Args>
void startThreadAndForget(Callable &&fn, Args &&... args) {
    std::thread thread(std::forward<Callable>(fn), std::forward<Args>(args)...);
    thread.detach();
}

template<typename T>
void deleteInTheBackground(T *obj) {
    startThreadAndForget([obj]() { delete obj; });
}
