#pragma once
#include <atomic>
#include <string>
#include <functional>
#include <sys/epoll.h>
#include "BaseConnection.hpp"
class ServerThreadResource;

class ServerConnection:public BaseConnection{
private:
    int peer_fd_;
    const std::string peer_ip_;
    const size_t RECV_BUFFER_SIZE;
    std::vector<char> recv_buffer_;
    std::vector<char> send_buffer_;
    size_t send_ptr_;
    struct epoll_event event_;
    ServerThreadResource& resource_;
    std::function<void(BaseConnection*, uint32_t)> event_callback_;
public:
    ServerConnection(int peer_fd, std::string peer_ip, size_t recv_buff_size, epoll_event event, ServerThreadResource& resource);
    ~ServerConnection();
    void set_event_callback(std::function<void(BaseConnection*, uint32_t)> event_callback);
    void recv_message();
    void send_message();
    void self_close();
    int get_fd();
    std::string get_ip();
    
};
