#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

// Thread-safe FIFO job queue — used by ThreadPool and async systems.
class JobQueue {
public:
    using Job = std::function<void()>;

    void push(Job job);
    Job pop();
    void shutdown();
    bool isShutdown() const;

private:
    mutable std::mutex mutex;
    std::condition_variable cv;
    std::deque<Job> jobs;
    bool shutdownRequested = false;
};
