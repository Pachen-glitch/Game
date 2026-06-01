#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

// Thread-safe FIFO job queue — used by ThreadPool and async systems.
class JobQueue {// Cola de jobs
public:
    using Job = std::function<void()>;

    void push(Job job);// Agrega un job a la cola
    Job pop();// Elimina un job de la cola
    void shutdown();// Cierra la cola
    bool isShutdown() const;// Devuelve si la cola esta cerrada

private:
    mutable std::mutex mutex;// Mutex para sincronizar el acceso a la cola
    std::condition_variable cv;// Condicion para sincronizar el acceso a la cola
    std::deque<Job> jobs;// Cola de jobs
    bool shutdownRequested = false;// Flag de shutdown
};
