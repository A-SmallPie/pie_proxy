#include "connection.hpp"
#include "server/connection_manager.hpp"

ConnectionManager::ConnectionManager(){

}

ConnectionManager::~ConnectionManager(){

}

bool ConnectionManager::add_connection(std::shared_ptr<Connection> connection){
    connection_map_.emplace(connection->get_socket(), std::move(connection));
    return true;
}

bool ConnectionManager::remove_connection(int fd){
    connection_map_.erase(fd);
    return true;
}

size_t ConnectionManager::remove_time_out_connection(){
    return 1;
}