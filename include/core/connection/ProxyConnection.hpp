#include <vector>
#include <string>
#include <sys/epoll.h>

class ProxyConn{
private:
    int client_socket_;
    int server_socket_;
    const std::string client_ip_;
    const std::string server_ip_;
    struct epoll_event client_event_;
    struct epoll_event server_event_;
    std::vector<char> recv_buff_;
    std::vector<char> send_buff_;

public:
    ProxyConn();
    ~ProxyConn();
    

};