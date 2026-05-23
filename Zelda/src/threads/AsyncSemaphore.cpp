#include "AsyncSemaphore.h"

AsyncSemaphore::AsyncSemaphore(int initialCount)
    : count(initialCount)
{
}

void AsyncSemaphore::release(int amount) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        count += amount;
    }
    cv.notify_all();
}

void AsyncSemaphore::acquire() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return count > 0; });
    --count;
}

bool AsyncSemaphore::tryAcquire() {
    std::lock_guard<std::mutex> lock(mutex);
    if (count <= 0) return false;
    --count;
    return true;
}
