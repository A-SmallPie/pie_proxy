#include <queue>
class Connection;

class ConnectionPool{
private:
    std::queue<Connection*> pool;
public:
    ConnectionPool();
    ~ConnectionPool();
    Connection* acquire();
    void release(Connection* connection);
};