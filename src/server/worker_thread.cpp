#include "task.hpp"
#include "connection_pool.hpp"
#include "connection.hpp"
#include "worker_thread.hpp"
#include "task_queue.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>

WorkerThread::WorkerThread(size_t max_event=64)
    :MAX_EVENT(max_event){
    epoll_fd_ = epoll_create1(0);
    if(epoll_fd_<0){
        std::cerr<<"创建epoll实例失败"<<std::endl;
        exit(EXIT_FAILURE);
    }
}

void WorkerThread::run(){
    struct epoll_event events[MAX_EVENT];
    while(true){
        Task* task;
        if(task_queue_.try_pop(&task)){
            switch(task->type){
                case TaskType::ADD_CONNECTION: handle_add_connection(task->connection); break;
                case TaskType::DEL_CONNECTION: handle_del_connection(task->connection); break;
            }
        }
        // 如果等待不到就绪的事件就去偷任务还是在任务队列里没有任务就直接去偷任务
        // 设置等待时间=0，如果没有就绪的时间就立刻去干别的
        int n = epoll_wait(epoll_fd_, events, MAX_EVENT, 0);
        for(int i=0; i<n; i++){
            Connection* connection = (Connection*)events[i].data.ptr;
            int client_socket = events[i].data.fd;

            // 客户端是否已经关闭或者发生了错误
            // 如果客户端只是暂时传完了数据怎么办
            // 非阻塞模式下不应循环调用 recv()，而是依赖 epoll_wait 的事件驱动。
            // 不能只靠这两个事件(EPOLLHUP,EPOLLERR)监听关闭
            if(events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)){
                handle_del_connection(connection);
                continue;
            }
            else if(events[i].events & EPOLLIN){
                connection->recv_message();
            }
        }
    }
}

void WorkerThread::handle_add_connection(Connection* connection){
    /* 给文件描述符设置非阻塞模式，如果不设置，且客户端数据没有就绪时，read会卡住线程
    虽然epoll只有在数据就绪的时候才会发送通知，但是如果返回的数据被其他线程取走了
    或者数据被内核协议栈丢弃（客户端发送rst包）时，有可能读不到数据*/
    int new_client_socket = connection->get_socket();
    int flags = fcntl(new_client_socket, F_GETFL, 0);
    fcntl(new_client_socket, F_SETFL, flags | O_NONBLOCK);

    /*添加事件监听， 不应该默认监听可写事件，因为这样会导致持续返回可写事件
    如果要监听可写事件，可以等缓冲区满了之后再监听可写事件
    设置epollrdhup是需要检测对方是否半关闭连接，对端不再会写数据*/
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    event.data.fd = new_client_socket;
    event.data.ptr = connection;
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, new_client_socket, &event)==-1){
        handle_del_connection(connection);
        std::cerr<<"添加连接事件监听失败"<<std::endl;
    }
}

void WorkerThread::handle_del_connection(Connection* connection){
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, connection->get_socket(), nullptr)==-1)
        std::cerr<<"移除监听失败:"<<errno<<std::endl;
    close(connection->get_socket());
    std::cout<<"已关闭来自"<<connection->get_ip()<<"的连接"<<std::endl;
    delete connection;
}

void WorkerThread::modify_epoll_events(Connection* connection, uint32_t op){
    // 处理各种事务， 暂不实现
    switch(op){
        case 1:
            add_task(new Task(connection, TaskType::DEL_CONNECTION));
            break;
        default:
            break;
    }
}

void WorkerThread::add_task(Task* task){
    task_queue_.push(task);
}

size_t WorkerThread::get_load(){
    return 1;
}

