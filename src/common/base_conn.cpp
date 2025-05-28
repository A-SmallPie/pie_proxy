#include "common/base_conn.hpp"

BaseConn::BaseConn(int sock_fd, std::string peer_ip, size_t recv_buff_size)
:peer_ip_(std::move(peer_ip)), RECV_BUFFER_SIZE(recv_buff_size){
    sock_fd_ = sock_fd;
    send_ptr_ = 0;
}

BaseConn::~BaseConn() = default;

int BaseConn::get_socket(){
    return sock_fd_;
}

std::string BaseConn::get_ip(){
    return peer_ip_;
}