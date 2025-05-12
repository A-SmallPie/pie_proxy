#include <vector>
#include <memory>

class WorkerThread;

class ThreadPool{
private:
    std::vector<std::unique_ptr<WorkerThread>> workers_;
public:
    // 禁止隐式类型转换，避免逻辑错误
    // size_t是一种无符号整形，专门用于表示索引下标，循环技术，内存大小等非负值
    explicit ThreadPool(size_t thread_count);
    ~ThreadPool();
    void dispatchTask(int client_socket);
};