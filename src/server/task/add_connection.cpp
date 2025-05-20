#include <iostream>
#include <sys/epoll.h>
#include "server/thread_resource.hpp"
#include "server/task/add_connection.hpp"

AddConnection::AddConnection(int fd){
    socket_fd_=fd;
}
void AddConnection::run(ThreadResource* res){
    //执行逻辑
    if(epoll_ctl(res->get_epoll_fd(), EPOLL_CTL_DEL, socket_fd_, nullptr)==-1)
        std::cerr<<"[工作线程] 线程"<<ID_<<": 移除监听失败:"<<errno<<std::endl;
    close(connection->get_socket());
    std::cout<<"[工作线程] 线程"<<ID_<<": 已关闭来自"<<connection->get_ip()<<"的连接"<<std::endl;
    delete connection;
}