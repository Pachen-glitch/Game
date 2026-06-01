#include "AsyncSemaphore.h"
// Constructor
AsyncSemaphore::AsyncSemaphore(int initialCount)
    : count(initialCount)// Inicia el contador con el valor inicial
{
}

void AsyncSemaphore::release(int amount) {// Libera el semaforo
    {
        std::lock_guard<std::mutex> lock(mutex);
        count += amount;// Incrementa el contador
    }
    cv.notify_all();// Notifica a todos los threads
}

void AsyncSemaphore::acquire() {// Espera a que el semaforo sea liberado
    std::unique_lock<std::mutex> lock(mutex);// Bloquea el mutex
    cv.wait(lock, [this] { return count > 0; });// Espera a que el contador sea mayor a 0
    --count;// Decrementa el contador
}

bool AsyncSemaphore::tryAcquire() {
    std::lock_guard<std::mutex> lock(mutex);// Bloquea el mutex 
    if (count <= 0) return false;// Si el contador es menor o igual a 0, devuelve false
    --count;// Decrementa el contador
    return true;
}
