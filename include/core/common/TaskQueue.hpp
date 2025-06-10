#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

/*如果是多个工作线程共享一个任务队列，那么这个任务队列应该定义到哪里
定义在线程池，还是定义在一个共有文件里面？
定义在线程池，那么在初始化工作线程的时候应该向工作线程提供队列的地址
队列里面应该存只能指针还是int？
资源描述符本质上就是一个int，所以可以直接赋值，但是必须保证只有一个线程关闭他
如果存智能指针，那么在将连接从epoll中移除的时候就不用手动关闭了*/

class BaseTask;
class Connection;

class TaskQueue{
private:
    std::queue<std::unique_ptr<BaseTask>> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
public:
    TaskQueue();
    ~TaskQueue();
    void push(std::unique_ptr<BaseTask> task);
    std::unique_ptr<BaseTask> pop();
    bool try_pop(std::unique_ptr<BaseTask>& task);
    bool is_empty()const;
    size_t size()const;
};