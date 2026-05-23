#pragma once

#include <condition_variable>
#include <mutex>

// Counting semaphore for producer/consumer backpressure (e.g. room prefetch slots).
class AsyncSemaphore {
public:
    explicit AsyncSemaphore(int initialCount = 0);

    void release(int count = 1);
    void acquire();
    bool tryAcquire();

private:
    std::mutex mutex;
    std::condition_variable cv;
    int count;
};
