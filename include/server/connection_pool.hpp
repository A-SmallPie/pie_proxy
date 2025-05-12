#include <queue>
class Connection;

class ConnectionPool{
private:
    std::queue<Connection*> pool;
    const int INITIAL_CONNECTION_NUM;
    int current_connection_num;
public:
    ConnectionPool(int intital_connection_num);
    ~ConnectionPool();
    Connection* acquire(int client_socket, std::string client_ip);
    void release(Connection* connection);
};