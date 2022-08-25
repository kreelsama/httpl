//
// Created by Creel Ji on 2022/8/25.
//

#include "TaskQueue.h"

TaskQueue::TaskQueue(int nThreads) {
    unsigned int nthreads = thread::hardware_concurrency();

    total_threads = nThreads == 0 ? nthreads : nThreads;

    available_threads = total_threads;

    threads.resize(nthreads);

    for(int t = 0; t < total_threads; ++t){
        threads.at(t) = thread(&TaskQueue::thread_loop, this);
    }
}

void TaskQueue::add_to_task_queue(function_type& task){
    task_queue.push(task);
    condition.notify_one();
}

void TaskQueue::thread_loop() {
    while(true) {
        function_type task;
        {
            std::unique_lock<mutex> lock(queue_lock);
            condition.wait(lock, [=] {
                return !task_queue.empty() || terminate;
            });
            if (terminate)
                return;
            task = task_queue.front();
            task_queue.pop();
        }
        available_threads -= 1;
        task();
        available_threads += 1;
    }
}

TaskQueue::~TaskQueue() {
    terminate = true;
    condition.notify_all();
    for(auto&& t:threads){
        t.join();
    }
    threads.clear();
}
