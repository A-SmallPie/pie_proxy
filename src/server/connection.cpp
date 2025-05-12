#include "connection.hpp"
#include <sys/epoll.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(size_t buffer_size)
:BUFFER_SIZE(buffer_size){
    client_socket_ = -1;
    client_ip_ = "";
}

Connection::Connection(int client_socket, std::string client_ip, size_t buffer_size)
:BUFFER_SIZE(buffer_size){
    client_socket_ = client_socket;
    client_ip_ = client_ip;
}

void Connection::set_socket(int client_socket){
    client_socket_ = client_socket;
}

void Connection::set_ip(std::string client_ip){
    client_ip_ = client_ip;
}

void Connection::set_event_callback(std::function<void(int, std::string)> event_callback){
    event_callback_ = std::move(event_callback);
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
            std::cout<<"客户端IP: "<<client_ip_<<"关闭"<<std::endl;
            event_callback_(client_socket_, "CLOSE_CONNECTION");
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
            event_callback_(client_socket_, "CLOSE_CONNECTION");
            break;
        }
    }
}

void Connection::send_message(){
    if(send(client_socket_, send_buffer_.data(), send_buffer_.size(), 0)<0)
        std::cerr<<"发送客户端回信失败"<<std::endl;
    std::cout<<"无数据可读"<<std::endl;
}

void Connection::reset(){
    client_socket_ = -1;
    client_ip_ = "";
    send_buffer_.erase();
    recv_buffer_.erase();
}

int Connection::get_socket(){
    return client_socket_;
}
std::string Connection::get_ip(){
    return client_ip_;
}