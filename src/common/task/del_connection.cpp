#include <unistd.h>
#include "server/thread_resource.hpp"
#include "common/task/del_connection.hpp"

DelConnection::DelConnection(int fd){
    client_socket_ = fd;
}

DelConnection::~DelConnection()=default;

void DelConnection::run(ThreadResource& res){
    res.epoll_mod(client_socket_, EPOLL_CTL_DEL, nullptr);
    res.remove_connection(client_socket_);
    close(client_socket_);
}