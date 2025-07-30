#include <semaphore.h>
#include <mutex>
#include <queue>

class ProducerConsumer {
private:
    std::queue<int> buffer;
    std::mutex buffer_mutex;
    sem_t empty_slots;  // counts empty buffer slots
    sem_t filled_slots; // counts filled buffer slots
    static const int BUFFER_SIZE = 10;

public:
    ProducerConsumer() {
        sem_init(&empty_slots, 0, BUFFER_SIZE);
        sem_init(&filled_slots, 0, 0);
    }

    void produce(int item) {
        sem_wait(&empty_slots);        // wait for empty slot
        buffer_mutex.lock();           // acquire exclusive access
        buffer.push(item);             // add item
        buffer_mutex.unlock();         // release access
        sem_post(&filled_slots);       // signal item available
    }

    int consume() {
        sem_wait(&filled_slots);       // wait for available item
        buffer_mutex.lock();           // acquire exclusive access
        int item = buffer.front();
        buffer.pop();                  // remove item
        buffer_mutex.unlock();         // release access
        sem_post(&empty_slots);        // signal empty slot
        return item;
    }
};