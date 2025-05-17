#pragma once
#include "server/task_queue.hpp"
class TaskQueue;

class WorkerThread{
private:
    int epoll_fd_;
    std::unique_ptr<TaskQueue> task_queue_;
    const int MAX_EVENT;

public:
    WorkerThread(size_t max_event);
    ~WorkerThread();
    void add_task(Task* task);
    void modify_epoll_events(Connection* connection, uint32_t op);
    void handle_add_connection(Connection* connection);
    void handle_del_connection(Connection* connection);
    size_t get_load();
    void run();
};