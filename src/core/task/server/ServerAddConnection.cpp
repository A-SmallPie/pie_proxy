#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include "core/task/base/BaseTask.hpp"
#include "core/task/server/ServerAddConnection.hpp"
#include "core/connection/ServerConnection.hpp"
#include "core/resource/ServerThreadResource.hpp"
#include "core/resource/ResourceVariant.hpp"

ServerAddConnection::ServerAddConnection(int fd, std::string ip, size_t buffer_size)
:RECV_BUFFER_SIZE(buffer_size){
    client_socket_= fd;
    client_ip_= ip;
}

ServerAddConnection::~ServerAddConnection()=default;

void ServerAddConnection::run(const ResourceVariant resource){
    /* 给文件描述符设置非阻塞模式，如果不设置，且客户端数据没有就绪时，read会卡住线程
    虽然epoll只有在数据就绪的时候才会发送通知，但是如果返回的数据被其他线程取走了
    或者数据被内核协议栈丢弃（客户端发送rst包）时，有可能读不到数据*/
    std::visit([this](auto&& res){
        using T = std::decay_t<decltype(res)>;
        if constexpr(std::is_same_v<T, ServerThreadResource*>){
            int flags = fcntl(client_socket_, F_GETFL, 0);
            if(fcntl(client_socket_, F_SETFL, flags | O_NONBLOCK)==-1){
                std::cerr<<"[线程"<<res->get_id()<<"]: 为连接"<<client_ip_<<"设置非阻塞模式失败"<<std::endl; 
                close(client_socket_);
                return;
            }

            struct epoll_event event{};
            event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
            event.data.fd = client_socket_;
            std::shared_ptr<ServerConnection> connection = \
            std::make_shared<ServerConnection>(client_socket_, client_ip_, RECV_BUFFER_SIZE, std::move(event), *res);
            res->add_connection(connection);
            std::cerr<<"[线程"<<res->get_id()<<"]: 连接"<<client_ip_<<"添加成功"<<std::endl; 
        } else {
            std::cerr<<"不支持的资源类型"<<std::endl;
        }
    }, resource);
}
