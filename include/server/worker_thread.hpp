#pragma once
#include "server/task_queue.hpp"
class TaskQueue;

class WorkerThread{
private:
    const size_t ID_;
    const size_t MAX_EVENT;
    int epoll_fd_;
    std::unique_ptr<TaskQueue> task_queue_;

public:
    WorkerThread(size_t id, size_t max_event);
    ~WorkerThread();
    void add_task(Task* task);
    void modify_epoll_events(Connection* connection, uint32_t op);
    void handle_add_connection(Connection* connection);
    void handle_del_connection(Connection* connection);
    size_t get_load();
    size_t get_id();
    void run();
};