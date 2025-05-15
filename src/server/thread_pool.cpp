#include "thread_pool.hpp"
#include "worker_thread.hpp"


ThreadPool::ThreadPool(size_t thread_num, size_t max_event)
:WORKER_NUM(thread_num){
    current_ptr_=0;
    for(size_t i=0; i<thread_num; i++){
        workers_.push_back(std::make_unique<WorkerThread>(max_event));
    }
}

void ThreadPool::dispatchTask(Task* task){
    current_ptr_ = (current_ptr_+1)%WORKER_NUM;
    workers_[current_ptr_-1]->add_task(task);
}