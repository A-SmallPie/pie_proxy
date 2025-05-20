#pragma once
#include <string>
#include <functional>
#include <atomic>
class ThreadResource;

class Connection{
private:
    int client_socket_;
    const int BUFFER_SIZE;
    std::atomic<bool> is_closed_{false};
    std::string client_ip_;
    std::string send_buffer_;
    std::string recv_buffer_;
    std::function<void(Connection*, uint32_t)> event_callback_;
public:
    Connection(int client_socket, std::string client_ip, size_t buffer_size);
    ~Connection();
    void set_task_queue(TaskQueue* task_queue);
    void set_event_callback(std::function<void(Connection*, uint32_t)> event_callback);
    void recv_message();
    void send_message();
    int get_socket();
    std::string get_ip();
};
