class TaskQueue;
class ConnectionPool;

class WorkerThread{
private:
    int epoll_fd_;
    ConnectionPool& pool_;
    TaskQueue task_queue_;
    const int MAX_EVENT;

public:
    WorkerThread(size_t max_event, ConnectionPool& pool);
    ~WorkerThread();
    bool add_task(Connection *connection);
    void modify_epoll_events(int fd, std::string events);
    int get_load();
    void run();
};