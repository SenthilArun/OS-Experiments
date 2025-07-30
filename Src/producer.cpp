




#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include<chrono>

using namespace std;
queue<int> q;
mutex mtx;
condition_variable cv;
#define Q_MAX_SIZE 10;

void  producer() {
 
    for (int i = 0;i < 10;i++) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] {return q.size() < Q_MAX_SIZE});
        q.push(i);
        cout << "Producer pushed" << i << endl;
        cv.notify_all();
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));

    }
}

void consumer() {
    while (true) {

        unique_lock<mutex> lock(mtx);
        
        cv.wait(lock, [] {return !q.empty();});
        int item = q.front();
        cout << "Consmer popped" << item << endl;
        q.pop();
        cv.notify_all();
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(150));

    }

}

int main() {
    thread thread_prod(producer);
    thread thread_cons(consumer);

    thread_prod.join();
    thread_cons.join();

    return 0;
}