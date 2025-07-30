#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

// Thread-safe bounded queue
template<typename T>
class BlockingQueue {
public:
    BlockingQueue(size_t capacity) : capacity_(capacity) {}

    // Push an item; blocks if queue is full
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_not_full_.wait(lock, [&] { return queue_.size() < capacity_; });
        queue_.push(item);
        cond_not_empty_.notify_one();
    }

    // Pop an item; blocks if queue is empty
    T pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_not_empty_.wait(lock, [&] { return !queue_.empty(); });
        T item = queue_.front();
        queue_.pop();
        cond_not_full_.notify_one();
        return item;
    }

private:
    std::mutex mtx_;
    std::condition_variable cond_not_empty_, cond_not_full_;
    std::queue<T> queue_;
    size_t capacity_;
};

void producer(BlockingQueue<int>& q, int count) {
    for (int i = 1; i <= count; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // simulate work
        q.push(i);
        std::cout << "Produced: " << i << "\n";
    }
}

void consumer(BlockingQueue<int>& q, int count) {
    for (int i = 1; i <= count; ++i) {
        int item = q.pop();
        std::cout << "Consumed: " << item << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(150));  // simulate work
    }
}

int main() {
    const int itemCount = 10;
    BlockingQueue<int> queue(5);  // capacity of 5

    std::thread prodThread(producer, std::ref(queue), itemCount);
    std::thread consThread(consumer, std::ref(queue), itemCount);

    prodThread.join();
    consThread.join();

    std::cout << "All items produced and consumed.\n";
    return 0;
}
