#pragma once
#include <atomic>
#include <string>
#include <functional>
#include "common/base_conn.hpp"
#include "thread_resource.hpp"

class ServerConn:public BaseConn{
private:
    struct epoll_event event_;
    ThreadResource& resource_;
    std::function<void(Connection*, uint32_t)> event_callback_;
public:
    ServerConn(int sock_fd, std::string peer_ip, size_t recv_buff_size, epoll_event event, ThreadResource& resource);
    ~ServerConn();
    void set_event_callback(std::function<void(Connection*, uint32_t)> event_callback);
    void recv_message();
    void send_message();
    void self_close();
};
