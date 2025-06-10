#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
class ServerConnection;

class ConnectionManager{
private:
    std::unordered_map<int, std::shared_ptr<ServerConnection>> connection_map_;

public:
    ConnectionManager();
    ~ConnectionManager();
    bool add_connection(std::shared_ptr<ServerConnection> connection);
    bool remove_connection(int fd);
    size_t remove_time_out_connection();
};