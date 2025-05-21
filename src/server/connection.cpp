#include <sys/epoll.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include "server/task/base.hpp"
#include "server/task/del_connection.hpp"
#include "server/task_queue.hpp"
#include "server/connection.hpp"


Connection::Connection(int client_socket, std::string client_ip, size_t buffer_size, ThreadResource& resource)
:BUFFER_SIZE(buffer_size), client_ip_(std::move(client_ip)), resource_(resource){
    client_socket_ = client_socket;
}

Connection::~Connection() = default;

void Connection::set_event_callback(std::function<void(Connection*, uint32_t)> event_callback){
    event_callback_ = std::move(event_callback);
}

int Connection::get_socket(){
    return client_socket_;
}

std::string Connection::get_ip(){
    return client_ip_;
}

void Connection::recv_message(){
    char buffer[BUFFER_SIZE];
    while (true)
    {
        ssize_t bytes_read = recv(client_socket_, buffer, BUFFER_SIZE-1, 0);
        if(bytes_read>0){
            recv_buffer_.insert(recv_buffer_.end(), buffer, buffer+bytes_read);
        }
        else if(bytes_read==0){
            // 客户端关闭（发送fin包）
            self_close();
            // event_callback_(this, 1);
            break;
        }
        else if(errno == EAGAIN || errno == EWOULDBLOCK){
            std::cout<<"收到客户端消息："<<recv_buffer_.data()<<std::endl;
            send_buffer_ = recv_buffer_;
            send_message();
            break;
        }
        else{
            std::cerr<<"客户端"<<client_ip_<<"发生错误:"<<errno<<std::endl;
            self_close();
            // event_callback_(this, 1);
            break;
        }
    }
}

void Connection::send_message(){
    // 如果缓冲区满，可以投递任务建立读数据的监听
    if(send(client_socket_, send_buffer_.data(), send_buffer_.size(), 0)<0)
        std::cerr<<"发送客户端回信失败"<<std::endl;
    std::cout<<"无数据可读"<<std::endl;
}

void Connection::self_close(){
    is_closed_.store(true, std::memory_order_release);
    std::cout<<"客户端IP: "<<client_ip_<<"关闭"<<std::endl;
    resource_.add_task(std::make_unique<DelConnection>(client_socket_));
}