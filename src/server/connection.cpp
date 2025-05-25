#include <sys/epoll.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "server/task/base.hpp"
#include "server/task/del_connection.hpp"
#include "server/task_queue.hpp"
#include "server/connection.hpp"


Connection::Connection(int client_socket, std::string client_ip, size_t recv_buff_size, epoll_event event, ThreadResource& resource)
:client_ip_(std::move(client_ip)),event_(std::move(event)), resource_(resource), RECV_BUFFER_SIZE(recv_buff_size){
    event_.data.ptr = this;
    /*添加事件监听， 不应该默认监听可写事件，因为这样会导致持续返回可写事件
    如果要监听可写事件，可以等缓冲区满了之后再监听可写事件
    设置epollrdhup是需要检测对方是否半关闭连接，对端不再会写数据*/
    // 这里不能存原始指针，因为shared可能被释放，导致epoll里面是悬空指针
    client_socket_ = client_socket;
    resource_.epoll_add(client_socket_, &event_);
    send_ptr_ = 0;
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

// 当发现自己需要关闭的时候，或许可以不延迟关闭，直接使用关闭三件套，就不用工作线程再去做关闭连接的任务了
void Connection::recv_message(){
    char buffer[RECV_BUFFER_SIZE];
    while (true)
    {
        ssize_t bytes_read = recv(client_socket_, buffer, RECV_BUFFER_SIZE, 0);
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
            std::cout<<"[线程"<<resource_.get_id()<<"]: 收到客户端消息："<<recv_buffer_.data()<<std::endl;
            std::cout<<"[线程"<<resource_.get_id()<<"]: 已完成本次数据接收，无数据可读"<<std::endl;
            send_buffer_ = recv_buffer_;
            send_message();
            break;
        }
        else{
            std::cerr<<"[线程"<<resource_.get_id()<<"]: 客户端"<<client_ip_<<"发生错误:"<<errno<<std::endl;
            self_close();
            // event_callback_(this, 1);
            break;
        }
    }
}

void Connection::send_message(){
    // 如果缓冲区满，可以投递任务建立读数据的监听
    // 接收数据的时候必须使用缓冲区一点一点接收，因为tcp是流式协议，无法控制一次接收多少
    // 发送的时候最好直接一次性发送（内核会自动分片），需要处理部分发送的情况。
    ssize_t bytes_sent = send(client_socket_, send_buffer_.data()+send_ptr_, send_buffer_.size()-send_ptr_, 0);
    if(bytes_sent<0){
        std::cerr<<"[线程"<<resource_.get_id()<<"]: 发送客户端回信失败"<<std::endl;
        return;
    }
    send_ptr_+=bytes_sent;
    if(send_ptr_==send_buffer_.size()&&(EPOLLIN & event_.events)){
        // 容器的clear成本是ON，所以一发送完就清空缓冲区性能是没什么提升的
        // 缓冲区发送完毕后，如果监听了可写，就需要移除可写的监听, 保留默认的三个监听
        event_.events &= ~EPOLLOUT;
        resource_.epoll_mod(client_socket_, EPOLL_CTL_MOD, &event_);
    }
    else if(bytes_sent==0){
        // 客户端关闭（发送fin包）
        self_close();
        // event_callback_(this, 1);
        return;
    }
    else if(send_ptr_<send_buffer_.size()&&!(EPOLLIN & event_.events)){
        // 缓冲区没发送完，且未监听可写事件，需要添加监听
        event_.events |= EPOLLOUT;
        resource_.epoll_mod(client_socket_, EPOLL_CTL_MOD, &event_);
    }
    // 检查缓冲区大小是否超过指定大小，若超过则进行紧凑操作
    if(send_ptr_>4096){
        std::memmove(send_buffer_.data(), send_buffer_.data()+send_ptr_, send_buffer_.size()-send_ptr_);
        send_buffer_.resize(send_buffer_.size()-send_ptr_);
        send_ptr_=0;
    }
}

void Connection::self_close(){
    is_closed_.store(true, std::memory_order_release);
    std::cout<<"[线程"<<resource_.get_id()<<"]: 客户端IP: "<<client_ip_<<"关闭"<<std::endl;
    resource_.add_task(std::make_unique<DelConnection>(client_socket_));
}