#pragma once
#include <thread>
#include <vector>
#include <memory>
class WorkerThread;

class ThreadPool{
private:
    const size_t WORKER_NUM;
    size_t current_ptr_;
    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<WorkerThread>> workers_;
public:
    // 禁止隐式类型转换，避免逻辑错误
    // size_t是一种无符号整形，专门用于表示索引下标，循环技术，内存大小等非负值
    explicit ThreadPool(size_t thread_num, size_t max_event);
    ~ThreadPool();
    void run_all();
    void dispatchTask(Task* Task);
};