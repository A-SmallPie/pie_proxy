#include <unistd.h>
#include "server/thread_resource.hpp"
#include "server/task/del_connection.hpp"

DelConnection::DelConnection(int fd){
    fd_ = fd;
}

DelConnection::~DelConnection()=default;

void DelConnection::run(ThreadResource& res){
    res.epoll_mod(fd_, EPOLL_CTL_DEL, nullptr);
    res.remove_connection(fd_);
    close(fd_);
}