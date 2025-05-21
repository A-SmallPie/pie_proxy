#pragma once
#include <memory>
#include <sys/epoll.h>
#include "task_queue.hpp"
#include "connection_manager.hpp"
class BaseTask;
class Connection;

// 必须包含类完整的头文件，因为类做了成员变量，就必须知道这个类的完整定义
class ThreadResource{
private:
    const size_t ID_;
    int epoll_fd_;
    const size_t MAX_EVENT;
    TaskQueue queue_;
    ConnectionManager manager_;

public:
    ThreadResource(size_t id, size_t max_event);
    ~ThreadResource();
    size_t get_id()const;
    // 实现操纵epoll的接口
    size_t get_max_events()const;
    int get_epoll_fd()const;
    bool epoll_add(int fd, epoll_event* event);
    bool epoll_mod(int fd, int op, epoll_event* event);
    // 透传任务队列方法
    void add_task(std::unique_ptr<BaseTask> task);
    std::unique_ptr<BaseTask> pop_task();
    bool try_pop_task(std::unique_ptr<BaseTask>& task);
    // 透传连接管理器的方法
    bool add_connection(std::shared_ptr<Connection> connection);
    bool remove_connection(int fd);
    size_t remove_time_out_connection();
};