#include <sys/epoll.h>
#include <string>

class Connection{
private:
    int client_socket_;
    int server_socket_;
    const std::string client_ip;
    const std::string server_ip;
    struct epoll_event client_event_;
    struct epoll_event server_event_;

}