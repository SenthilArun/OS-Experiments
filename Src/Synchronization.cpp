/**
 * @file sync_queue.cpp
 * @brief Thread-safe blocking queue implementation for producer-consumer patterns
 *
 * This file contains a template-based thread-safe queue that provides blocking
 * operations for producers and consumers. The queue has a fixed capacity and
 * uses condition variables to coordinate between multiple producer and consumer
 * threads efficiently.
 *
 * Key Features:
 * - Thread-safe operations using mutex and condition variables
 * - Blocking push/pop operations when queue is full/empty
 * - Template-based design for type flexibility
 * - Exception safety and proper resource management
 * - Support for multiple producers and consumers
 *
 * @author Senthil Arunachalam
 * @version 1.0
 *
 * @copyright Copyright (c) 2025 [Your Name/Organization]
 *
 * @license MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 
 
 *
 * @note This implementation requires C++11 or later for std::thread,
 *       std::condition_variable, and lambda expressions.
 *
 * @warning When using multiple consumer threads, ensure proper coordination
 *          to avoid race conditions in application logic outside the queue.
 *
 * @see std::condition_variable
 * @see std::mutex
 * @see std::thread
 *
 
 *
 * Change Log:
 * -----------
 * v1.0 (2025-01-XX) - Initial implementation
 *   - Basic thread-safe queue with blocking operations
 *   - Template-based design for type flexibility
 *   - Comprehensive documentation and examples
 */

#include<iostream>
#include<thread>
#include<condition_variable>
#include<queue>
#include<mutex>
#include<chrono>

using namespace std;
template <typename T>
class sync_queue {
public:
	sync_queue(int size) :capacity(size) {
	}
	void push(T &data) {
		unique_lock<mutex> lock(m);
		cond_not_full.wait(lock, [&] {return q.size() < capacity;});
		q.push(data);
		cond_not_empty.notify_one();
	}

	T pop() {
		unique_lock<mutex> lock(m);
		cond_not_empty.wait(lock, [&] { return !q.empty();});
		T item = q.front();
		q.pop();
		cond_not_full.notify_one();
		return item;
	}
private:
	condition_variable cond_not_full, cond_not_empty;
	mutex m;
	queue<T> q;
	size_t capacity;
};

void producerthread(sync_queue<int>& q, int count) {
	for (int i = 0;i < count;i++) {
		q.push(i);
		this_thread::sleep_for(chrono::milliseconds(10));
		cout << "produced ..item"<<i<<endl;
	}
}

void consumerthread(sync_queue<int>& q, int count) {
	for (int i = 0;i < count;i++) {
		int data = q.pop();
		this_thread::sleep_for(chrono::milliseconds(10));
		cout << "consumed ..item"<<data<<endl;
	}
}

int main() {
	sync_queue<int> q(10);
	thread Produce(producerthread, ref(q), 10);
	thread Consume(consumerthread, ref(q), 10);
	Produce.join();
	Consume.join();
	return 0;
}


