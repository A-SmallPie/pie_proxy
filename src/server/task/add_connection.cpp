#include "server/task/add_connection.hpp"

AddConnection::AddConnection(int fd){
    socket_fd_=fd;
}
void AddConnection::run(ThreadResource &res){
    //执行逻辑
}