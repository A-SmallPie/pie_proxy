#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include "server/connection.hpp"
#include "server/thread_resource.hpp"
#include "server/task/add_connection.hpp"

AddConnection::AddConnection(int fd, std::string ip, size_t buffer_size)
:BUFFER_SIZE(buffer_size){
    fd_= fd;
    ip_= ip;
}

AddConnection::~AddConnection()=default;

void AddConnection::run(ThreadResource& res){
    // 补全连接类对象
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(fd_, ip_, BUFFER_SIZE, res);
    /* 给文件描述符设置非阻塞模式，如果不设置，且客户端数据没有就绪时，read会卡住线程
    虽然epoll只有在数据就绪的时候才会发送通知，但是如果返回的数据被其他线程取走了
    或者数据被内核协议栈丢弃（客户端发送rst包）时，有可能读不到数据*/
    int flags = fcntl(fd_, F_GETFL, 0);
    if(fcntl(fd_, F_SETFL, flags | O_NONBLOCK)==-1){
        std::cerr<<"线程"<<res.get_id()<<" 为连接"<<ip_<<"设置非阻塞模式失败"<<std::endl; 
        close(fd_);
        return;
    }
    /*添加事件监听， 不应该默认监听可写事件，因为这样会导致持续返回可写事件
    如果要监听可写事件，可以等缓冲区满了之后再监听可写事件
    设置epollrdhup是需要检测对方是否半关闭连接，对端不再会写数据*/
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    event.data.fd = fd_;
    // 这里不能存原始指针，因为shared可能被释放，导致epoll里面是悬空指针
    event.data.ptr = connection.get();
    res.epoll_add(fd_, &event);
    res.add_connection(connection);
}