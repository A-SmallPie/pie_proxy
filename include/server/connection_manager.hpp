#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
class ServerConn;

class ConnectionManager{
private:
    std::unordered_map<int, std::shared_ptr<ServerConn>> connection_map_;

public:
    ConnectionManager();
    ~ConnectionManager();
    bool add_connection(std::shared_ptr<ServerConn> connection);
    bool remove_connection(int fd);
    size_t remove_time_out_connection();
};