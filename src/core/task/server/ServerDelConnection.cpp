#include <unistd.h>
#include "core/resource/ResourceVariant.hpp"
#include "core/resource/ServerThreadResource.hpp"
#include "core/task/server/ServerDelConnection.hpp"

ServerDelConnection::ServerDelConnection(int fd){
    client_socket_ = fd;
}

ServerDelConnection::~ServerDelConnection()=default;

void ServerDelConnection::run(const ResourceVariant resource){
    std::visit([this](auto&& res){
        using T = std::decay_t<decltype(res)>;
        if constexpr(std::is_same_v<T, ServerThreadResource*>){
            res->epoll_mod(client_socket_, EPOLL_CTL_DEL, nullptr);
            res->remove_connection(client_socket_);
            close(client_socket_);
        }else {
            std::cerr<<"不支持的资源类型"<<std::endl;
        }
    }, resource);
}