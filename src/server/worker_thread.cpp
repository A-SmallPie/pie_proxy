#include <vector>
#include <memory>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include "common/task/base.hpp"
#include "common/task_queue.hpp"
#include "server/server_conn.hpp"
#include "server/worker_thread.hpp"
#include "server/connection_manager.hpp"

WorkerThread::WorkerThread(size_t id, size_t max_event=64)
:ID_(id){
    resource_=std::make_unique<ThreadResource>(id, max_event);
}

WorkerThread::~WorkerThread() = default;

void WorkerThread::run(){
    size_t MAX_EVENT = resource_->get_max_events();
    struct epoll_event events[MAX_EVENT];
    while(true){
        std::unique_ptr<BaseTask> task;
        while(resource_->try_pop_task(task)){
            task->execute(*resource_.get());
        }
        // 如果等待不到就绪的事件就去偷任务还是在任务队列里没有任务就直接去偷任务
        // 设置等待时间=0，如果没有就绪的时间就立刻去干别的
        int n = epoll_wait(resource_->get_epoll_fd(), events, MAX_EVENT, 5);
        for(int i=0; i<n; i++){
            ServerConn* connection = (ServerConn*)events[i].data.ptr;
            // 客户端是否已经关闭或者发生了错误
            // 如果客户端只是暂时传完了数据怎么办
            // 非阻塞模式下不应循环调用 recv()，而是依赖 epoll_wait 的事件驱动。
            // 不能只靠这两个事件(EPOLLHUP,EPOLLERR)监听关闭
            if(events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)){
                std::cerr<<"[线程"<<ID_<<"]: 客户端出错，关闭连接"<<std::endl;
                int fd = events->data.fd;
                resource_->epoll_mod(fd, EPOLL_CTL_DEL,  nullptr);
                resource_->remove_connection(fd);
                close(fd);
                continue;
            }
            else if(events[i].events & EPOLLIN){
                resource_->remove_time_out_connection();
                connection->recv_message();
                std::cout<<"bbbbbb"<<std::endl;
                // connection->send_message();
            }
            else if(events[i].events & EPOLLOUT){
                connection->send_message();
            }
        }
    }
}

void WorkerThread::dispatch_task(std::unique_ptr<BaseTask> task){
    resource_->add_task(std::move(task));
}

void WorkerThread::modify_epoll_events(ServerConn* connection, uint32_t op){
    // 处理各种事务， 暂不实现
    connection->get_ip();
    std::cout<<op<<std::endl;
    return;
}

size_t WorkerThread::get_load() const{
    return 1;
}

size_t WorkerThread::get_id() const{
    return ID_;
}