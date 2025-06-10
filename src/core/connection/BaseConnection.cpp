#include "core/connection/BaseConnection.hpp"

BaseConnection::BaseConnection(int sock_fd, std::string peer_ip, size_t recv_buff_size)
:peer_ip_(std::move(peer_ip)), RECV_BUFFER_SIZE(recv_buff_size){
    sock_fd_ = sock_fd;
    send_ptr_ = 0;
}

BaseConnection::~BaseConnection() = default;

int BaseConnection::get_socket(){
    return sock_fd_;
}

std::string BaseConnection::get_ip(){
    return peer_ip_;
}