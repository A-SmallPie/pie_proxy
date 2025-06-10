#include <queue>
class ServerConnection;

class ConnectionPool{
private:
    std::queue<ServerConnection*> pool_;
    const size_t MAX_POOL_CAPACITY;
    const size_t INITIAL_POOL_CAPACITY;
    size_t current_pool_capacity_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
public:
    ConnectionPool(size_t intital_pool_capacity, size_t max_pool_capacity);
    ~ConnectionPool();
    bool acquire(int client_socket, std::string client_ip, ServerConnection** connection);
    size_t get_used_connection_num();
    void release(ServerConnection* connection);
    void expand();
    void scale_out();
    bool is_empty();
};