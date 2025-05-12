#include <string>
#include <functional>

class Connection{
private:
    int client_socket_;
    const int BUFFER_SIZE;
    std::string client_ip_;
    std::string send_buffer_;
    std::string recv_buffer_;
    std::function<void(int, std::string)> event_callback_;
public:
    Connection(size_t buffer_size);
    Connection(int client_socket, std::string client_ip, size_t buffer_size);
    ~Connection();
    void set_socket(int client_socket);
    void set_ip(std::string client_ip);
    void set_event_callback(std::function<void(int, std::string)> event_callback);
    void recv_message();
    void send_message();
    void reset();
    int get_socket();
    std::string get_ip();
};
