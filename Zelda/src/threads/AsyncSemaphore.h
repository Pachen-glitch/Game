#pragma once

#include <condition_variable>
#include <mutex>

// Counting semaphore for producer/consumer backpressure (e.g. room prefetch slots).
class AsyncSemaphore {// Semaforo asincrono
public:
    explicit AsyncSemaphore(int initialCount = 0);// Constructor

    void release(int count = 1);// Libera el semaforo
    void acquire();// Espera a que el semaforo sea liberado
    bool tryAcquire();// Intenta adquirir el semaforo

private:
    std::mutex mutex;// Mutex para sincronizar el acceso al contador
    std::condition_variable cv;// Condicion para sincronizar el acceso al contador
    int count;
};
