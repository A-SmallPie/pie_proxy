#pragma once
#include <vector>
#include <atomic>
#include <string>

class BaseConnection{
protected:
    std::atomic<bool> is_closed_{false};
    int sock_fd_;
    const std::string peer_ip_;
    const size_t RECV_BUFFER_SIZE;
    std::vector<char> recv_buffer_;
    size_t send_ptr_;
    std::vector<char> send_buffer_;
public:
    BaseConnection(int sock_fd, std::string peer_ip, size_t recv_buff_size=4096);
    virtual ~BaseConnection()=0;
    virtual void recv_message()=0;
    virtual void send_message()=0;
    int get_socket();
    std::string get_ip();
    virtual void self_close()=0;
};
