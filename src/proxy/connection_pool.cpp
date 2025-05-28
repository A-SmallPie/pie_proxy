#include "proxy/connection.hpp"
#include "proxy/connection_pool.hpp"
#include <mutex>

/*
需要添加的功能：
    - 访问时加锁
    - 弹出连接的时候判空
    - 动态扩展
    - 通过 shared_ptr 或 RAII 自动归还。
    - 销毁时释放全部链接

*/

ConnectionPool::ConnectionPool(size_t initial_pool_capacity, size_t max_pool_capacity)
:INITIAL_POOL_CAPACITY(initial_pool_capacity), MAX_POOL_CAPACITY(max_pool_capacity){
    for(int i=0; i<INITIAL_POOL_CAPACITY; i++){
        Connection* connection = new Connection(1024);
        pool_.emplace(connection);
    }
}

bool ConnectionPool::acquire(int client_socket, std::string client_ip, Connection** connection){
    std::lock_guard<std::mutex> lock(mutex_);
    if(pool_.empty()){
        if(MAX_POOL_CAPACITY<=current_pool_capacity_)
            return false;
        else expand();
    }
    *connection = pool_.front();
    pool_.pop();
    (*connection)->set_socket(client_socket);
    (*connection)->set_ip(client_ip);
    return true;
}

void ConnectionPool::expand(){
    size_t capacity = 0;
    if(current_pool_capacity_*2<=MAX_POOL_CAPACITY)
        capacity = current_pool_capacity_*2;
    else capacity = MAX_POOL_CAPACITY;
    for(size_t i=0; i<capacity - current_pool_capacity_; i++){
        Connection* connection = new Connection(1024);
        pool_.emplace(connection);
    }
    current_pool_capacity_ = capacity;
}

void ConnectionPool::scale_out(){
    std::lock_guard<std::mutex> lock(mutex_);
    size_t capacity = current_pool_capacity_;
    if(current_pool_capacity_/2>=INITIAL_POOL_CAPACITY)
        capacity = current_pool_capacity_/2;
    else capacity = INITIAL_POOL_CAPACITY;
    for(size_t i=0; i<current_pool_capacity_-capacity; i++){
        Connection* connection = pool_.front();
        pool_.pop();
        delete(connection);
    }
    current_pool_capacity_ = capacity;
}

void ConnectionPool::release(Connection* connection){
    connection->reset();
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.emplace(connection);
}

size_t ConnectionPool::get_used_connection_num(){
    std::lock_guard<std::mutex> lock(mutex_);
    return current_pool_capacity_ - pool_.size();
}