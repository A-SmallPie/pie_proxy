#include <queue>
#include <mutex>
#include <condition_variable>
#include "core/task/base/BaseTask.hpp"
#include "core/common/TaskQueue.hpp"

TaskQueue::TaskQueue() = default;

TaskQueue::~TaskQueue() = default;

void TaskQueue::push(std::unique_ptr<BaseTask> task){
    /*lock_guard自动加锁解锁，首先尝试获取锁，在mutex的作用域结束之后自动解锁
    lock_guard不支持手动解锁，所以pop函数只能手动加锁解锁
    lock_guard确实可以被unique_lock替代，但是lock_guard性能开销更小
    且lock_guard明确表示了全程持有锁的意图，unique_lock的灵活性是多余的
    要根据情景灵活选择更适合的工具*/
    std::lock_guard<std::mutex> lock(mutex_); 
    queue_.push(std::move(task));
    // 唤醒一个消费者线程
    cv_.notify_one();
}

std::unique_ptr<BaseTask> TaskQueue::pop(){
    /*必须使用wait等待队列不空，否则这就是一个自旋锁，会消耗cpu资源
    如果使用锁， 则wait的时候必须临时释放锁，如果使用lock_guard，中途就不能释放锁
    这个锁只能在上锁的线程拿到资源后才会释放，这样其他进程就不能再操作队列，也就不能让队列非空，这个进程就会永远等下去
    （等待这个资源的时候必须先释放这个资源上的锁，这样就能在一定程度上避免死锁）*/
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]{return !queue_.empty();});
    // wait结束后会自动获取这个锁，所以wait后面不需要手动重新加锁
    auto task = std::move(queue_.front());
    queue_.pop();
    // unique_lock也会在作用域结束时自动释放锁
    return task;
}

bool TaskQueue::try_pop(std::unique_ptr<BaseTask>& task){
    // 队列为空的时候返回false,必须在锁内部判断队列是不是空
    // 在这里不需要uniquelock（不需要中途解锁）
    std::lock_guard<std::mutex> lock(mutex_);
    if(queue_.empty()) return false;
    task = std::move(queue_.front());
    queue_.pop();
    return true;
}

bool TaskQueue::is_empty() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.empty();
}

size_t TaskQueue::size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
}