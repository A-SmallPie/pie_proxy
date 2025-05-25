#pragma once
#include <atomic>
#include <string>
#include <functional>
#include "thread_resource.hpp"

class Connection{
private:
    std::atomic<bool> is_closed_{false};
    int client_socket_;
    const std::string client_ip_;
    struct epoll_event event_;
    ThreadResource& resource_;
    const size_t RECV_BUFFER_SIZE;
    std::vector<char> recv_buffer_;
    size_t send_ptr_;
    std::vector<char> send_buffer_;
    std::function<void(Connection*, uint32_t)> event_callback_;
public:
    Connection(int client_socket, std::string client_ip, size_t recv_buff_size, epoll_event event, ThreadResource& resource);
    ~Connection();
    void set_event_callback(std::function<void(Connection*, uint32_t)> event_callback);
    void recv_message();
    void send_message();
    int get_socket();
    std::string get_ip();
    void self_close();
};
