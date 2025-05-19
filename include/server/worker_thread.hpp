#pragma once
class TaskQueue;
class ConnectionManager;

class WorkerThread{
private:
    const size_t ID_;
    const size_t MAX_EVENT;
    int epoll_fd_;
    std::unique_ptr<TaskQueue> task_queue_;
    std::unique_ptr<ConnectionManager> manager_;

public:
    WorkerThread(size_t id, size_t max_event);
    ~WorkerThread();
    void modify_epoll_events(Connection* connection, uint32_t op);
    void handle_add_connection(Connection* connection);
    void handle_del_connection(Connection* connection);
    size_t get_load();
    size_t get_id();
    void run();
    // 资源操作接口

    // 透传任务队列方法
    void add_task(Task* task);
    // 透传连接管理器的方法
    bool add_connection(std::shared_ptr<Connection> connection);
    bool remove_connection(int fd);
    size_t remove_time_out_connection();

};