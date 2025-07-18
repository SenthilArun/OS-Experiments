#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <string>
#include <cstdlib>      // for rand()
#include <ctime>        // for time()
#include <chrono>

// A simple task struct with priority and name
struct Task {
    int         priority;
    std::string name;
};

// Shared data
std::list<Task>             taskList;
std::mutex                  mtx;
std::condition_variable     cv;

// Insert a task into the list so that highest-priority tasks come first
void pushTask(const std::string& name, int priority) {
    std::unique_lock<std::mutex> lock(mtx);
    std::list<Task>::iterator it = taskList.begin();
    while (it != taskList.end() && it->priority >= priority) {
        ++it;
    }
    taskList.insert(it, Task{ priority, name });
    cv.notify_one();
}

// Producer thread function
void producerFunc(int producerId, int taskCount) {
    for (int i = 0; i < taskCount; ++i) {
        int prio = std::rand() % 100;
        std::string tname = "P" + std::to_string(producerId)
            + "_T" + std::to_string(i);
        pushTask(tname, prio);
        std::cout << "[Producer " << producerId << "] "
            << "pushed " << tname
            << " (priority=" << prio << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Consumer thread function
void consumerFunc(int consumerId, int totalToConsume) {
    for (int i = 0; i < totalToConsume; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        // wait until there is something to consume
        while (taskList.empty()) {
            cv.wait(lock);
        }
        // pop the front (highest-priority) task
        Task t = taskList.front();
        taskList.pop_front();
        lock.unlock();

        std::cout << "  [Consumer " << consumerId << "] "
            << "got " << t.name
            << " (priority=" << t.priority << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

int main() {
    std::srand(unsigned(std::time(nullptr)));

    const int numProducers = 2;
    const int numConsumers = 2;
    const int tasksPerProducer = 6;
    const int totalTasks = numProducers * tasksPerProducer;
    const int perConsumer = totalTasks / numConsumers;

    // Launch producer threads
    std::thread producers[numProducers];
    for (int i = 0; i < numProducers; ++i) {
        producers[i] = std::thread(producerFunc, i + 1, tasksPerProducer);
    }

    // Launch consumer threads
    std::thread consumers[numConsumers];
    for (int i = 0; i < numConsumers; ++i) {
        consumers[i] = std::thread(consumerFunc, i + 1, perConsumer);
    }

    // Wait for all to finish
    for (int i = 0; i < numProducers; ++i)
        producers[i].join();
    for (int i = 0; i < numConsumers; ++i)
        consumers[i].join();

    std::cout << "All work done.\n";
    return 0;
}
