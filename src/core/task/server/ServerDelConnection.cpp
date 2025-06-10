#include <unistd.h>
#include "core/resource/ServerThreadResource.hpp"
#include "core/task/server/ServerDelConnection.hpp"

ServerDelConnection::ServerDelConnection(int fd){
    client_socket_ = fd;
}

ServerDelConnection::~ServerDelConnection()=default;

void ServerDelConnection::run(ServerThreadResource& res){
    res.epoll_mod(client_socket_, EPOLL_CTL_DEL, nullptr);
    res.remove_connection(client_socket_);
    close(client_socket_);
}