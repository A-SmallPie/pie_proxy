#include "core/connection/ServerConnection.hpp"
#include "server/ConnectionManager.hpp"

ConnectionManager::ConnectionManager() = default;

ConnectionManager::~ConnectionManager() = default;

bool ConnectionManager::add_connection(std::shared_ptr<ServerConnection> connection){
    connection_map_.emplace(connection->get_socket(), std::move(connection));
    return true;
}

bool ConnectionManager::remove_connection(int fd){
    connection_map_.erase(fd);
    return true;
}

size_t ConnectionManager::remove_time_out_connection(){
    for (const auto& entry : connection_map_) {
        int fd = entry.first;
        auto connection = entry.second;
        std::cout << "FD: " << fd << ", shared_ptr count: " << connection.use_count() << std::endl;
        std::cout << "FD: " << fd << ", IP" << connection->get_ip() << std::endl;
    }
    return 1;
}