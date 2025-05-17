#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
class Connection;


class ConnnectionManager{
private:
    std::unordered_map<int, std::shared_ptr<Connection>> connection_map;

public:
    ConnnectionManager();
    ~ConnnectionManager();
    bool add_connection(int fd, Connection* connection);
    bool release_connection(int fd);
};