//
// Created by Creel Ji on 2022/8/25.
//

#ifndef HTTP_TASKQUEUE_H
#define HTTP_TASKQUEUE_H

#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <atomic>

using std::mutex;
using std::thread;
using std::function;
using std::vector;
using std::queue;
using function_type = std::function<void()>;

struct locked_int{
    int n;
    mutex lock;

    locked_int& operator=(int val){
       std::unique_lock<mutex> lck(lock);
       n = val;
       return *this;
    }

    int operator+=(int val){
        std::unique_lock<mutex> lck(lock);
        n += val;
        return n;
    }

    int operator-=(int val){
        std::unique_lock<mutex> lck(lock);
        n -= val;
        return n;
    }
};

template<typename T>
struct locked_queue : queue<T>{
    mutex lock;

    void locked_push(T task){
        std::unique_lock<mutex> lck(lock);
        this->push(task);
    }

    void locked_pop(){
        std::unique_lock<mutex> lck(lock);
        this->pop();
    }

    T locked_front(){
        std::unique_lock<mutex> lck(lock);
        return this->front();
    }

    bool locked_empty(){
        std::unique_lock<mutex> lck(lock);
        return this->empty();
    }

    mutex& acquire_lock(){
        return lock;
    }
};

class TaskQueue {
public:
    explicit TaskQueue(int nThreads=0);
    void add_to_task_queue(function_type& task);
    ~TaskQueue();

protected:
    unsigned int total_threads;
    vector<thread> threads;
    std::atomic_uint16_t available_threads{};
    queue<function_type> task_queue;
    mutex queue_lock;
    std::condition_variable condition;
    int terminate=0;

    void thread_loop();
};


#endif //HTTP_TASKQUEUE_H
