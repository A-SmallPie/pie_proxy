#include "connection.hpp"
#include "connection_pool.hpp"

/*
需要添加的功能：
    - 访问时加锁
    - 弹出连接的时候判空
    - 动态扩展
    - 通过 shared_ptr 或 RAII 自动归还。
    - 销毁时释放全部链接

*/

ConnectionPool::ConnectionPool(int initial_connection_num)
:INITIAL_CONNECTION_NUM(initial_connection_num){
    for(int i=0; i<INITIAL_CONNECTION_NUM; i++){
        Connection* connection = new Connection(1024);
        pool.emplace(connection);
    }
}

Connection* ConnectionPool::acquire(int client_socket, std::string client_ip){
    Connection *connection = pool.front();
    pool.pop();
    connection->set_socket(client_socket);
    connection->set_ip(client_ip);
    return connection;
}

void ConnectionPool::release(Connection* connection){
    connection->reset();
    pool.emplace(connection);
}