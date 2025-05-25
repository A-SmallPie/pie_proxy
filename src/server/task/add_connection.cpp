#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include "server/connection.hpp"
#include "server/thread_resource.hpp"
#include "server/task/add_connection.hpp"

AddConnection::AddConnection(int fd, std::string ip, size_t buffer_size)
:RECV_BUFFER_SIZE(buffer_size){
    client_socket_= fd;
    client_ip_= ip;
}

AddConnection::~AddConnection()=default;

void AddConnection::run(ThreadResource& res){
    /* 给文件描述符设置非阻塞模式，如果不设置，且客户端数据没有就绪时，read会卡住线程
    虽然epoll只有在数据就绪的时候才会发送通知，但是如果返回的数据被其他线程取走了
    或者数据被内核协议栈丢弃（客户端发送rst包）时，有可能读不到数据*/
    int flags = fcntl(client_socket_, F_GETFL, 0);
    if(fcntl(client_socket_, F_SETFL, flags | O_NONBLOCK)==-1){
        std::cerr<<"[线程"<<res.get_id()<<"]: 为连接"<<client_ip_<<"设置非阻塞模式失败"<<std::endl; 
        close(client_socket_);
        return;
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    event.data.fd = client_socket_;
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(client_socket_, client_ip_, RECV_BUFFER_SIZE, std::move(event), res);
    res.add_connection(connection);
    std::cerr<<"[线程"<<res.get_id()<<"]: 连接"<<client_ip_<<"添加成功"<<std::endl; 
}