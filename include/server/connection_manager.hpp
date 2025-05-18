#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
class Connection;


class ConnectionManager{
private:
    std::unordered_map<int, std::shared_ptr<Connection>> connection_map_;

public:
    ConnectionManager();
    ~ConnectionManager();
    bool add_connection(std::shared_ptr<Connection> connection);
    bool remove_connection(int fd);
    size_t remove_time_out_connection();
};