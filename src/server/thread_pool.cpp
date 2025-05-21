#include <thread>
#include <sstream>
#include <iostream>
#include "server/task/base.hpp"
#include "server/thread_pool.hpp"
#include "server/worker_thread.hpp"


ThreadPool::ThreadPool(size_t thread_num, size_t max_event)
:WORKER_NUM(thread_num){
    current_ptr_=0;
    std::cout<<"[线程池] 创建了一个有"<<thread_num<<"个线程的线程池"<<std::endl;
    for(size_t i=0; i<thread_num; i++){
        workers_.push_back(std::make_unique<WorkerThread>(i, max_event));
        std::cout<<"[线程池] 创建第"<<i+1<<"个工作线程对象"<<std::endl;
    }
}

ThreadPool::~ThreadPool(){
    std::cout<<"[线程池] 正在销毁线程池"<<std::endl;
    for(auto& t:threads_){
        if(t.joinable()){
            t.join();
        }
    }
}

void ThreadPool::run_all(){
    // 不能用for(:)循环(这种循环需要数组里放对象)，因为容器里面存储的是unique指针
    std::cout<<"[线程池] 启动所有线程..."<<std::endl;
    for(size_t i=0; i<workers_.size(); i++){
        threads_.emplace_back( [this, i]{
                // 获取线程id
                std::stringstream ss;
                ss<<std::this_thread::get_id();
                std::string thread_id = ss.str();
                std::cout<<"[线程池] 启动工作线程"<<i<<"(线程ID: "<<thread_id<<")"<<std::endl;
                workers_[i]->run();
                std::cout<<"[线程池] 结束工作线程"<<i<<"(线程ID: "<<thread_id<<")"<<std::endl;
            }
        );
    }
}

void ThreadPool::dispatchTask(std::unique_ptr<BaseTask> task){
    size_t index = current_ptr_%WORKER_NUM;
    std::cout<<"[线程池] 分配连接任务至工作线程: "<<index<<"; 总连接任务数: "<<current_ptr_+1<<std::endl;
    workers_[index]->dispatch_task(std::move(task));
    current_ptr_++;
}
