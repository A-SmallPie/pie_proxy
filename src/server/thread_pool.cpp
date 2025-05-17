#include <thread>
#include "server/task.hpp"
#include "server/thread_pool.hpp"
#include "server/worker_thread.hpp"


ThreadPool::ThreadPool(size_t thread_num, size_t max_event)
:WORKER_NUM(thread_num){
    current_ptr_=0;
    for(size_t i=0; i<thread_num; i++){
        workers_.push_back(std::make_unique<WorkerThread>(max_event));
    }
}

ThreadPool::~ThreadPool(){
    for(auto& t:threads_){
        if(t.joinable()){
            t.join();
        }
    }
}

void ThreadPool::run_all(){
    // 不能用for(:)循环，因为容器里面存储的是unique指针
    for(size_t i=0; i<workers_.size(); i++){
        threads_.emplace_back( [this, i]{
                workers_[i]->run();
            }
        );
    }
}

void ThreadPool::dispatchTask(Task* task){
    size_t index = current_ptr_%WORKER_NUM;
    workers_[index]->add_task(task);
    current_ptr_++;
}
