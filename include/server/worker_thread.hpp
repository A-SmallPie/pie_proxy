class TaskQueue;

class WorkerThread{
private:
    int epoll_fd_;
    TaskQueue task_queue_;
    const int MAX_EVENT;

public:
    WorkerThread(size_t max_event);
    ~WorkerThread();
    bool add_task(Connection *connection);
    void modify_epoll_events(int fd, std::string events);
    int get_load();
    void run();
};