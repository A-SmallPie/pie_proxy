#pragma once
#include "thread_resource.hpp"

// 因为使用指针管理类对象，所以不需要包含头文件
class WorkerThread{
private:
    const size_t ID_;
    std::unique_ptr<ThreadResource> resource_;

public:
    WorkerThread(size_t id, size_t max_event);
    ~WorkerThread();
    void modify_epoll_events(ServerConn* server_conn, uint32_t op);
    void dispatch_task(std::unique_ptr<BaseTask> task);
    size_t get_load() const;
    size_t get_id() const;
    void run();
};