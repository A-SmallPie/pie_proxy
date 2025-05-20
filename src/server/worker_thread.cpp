#include "server/task/base.hpp"
#include "server/task_queue.hpp"
#include "server/connection.hpp"
#include "server/worker_thread.hpp"
#include "server/connection_manager.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>

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
            task->execute(resource_.get());
        }
        // 如果等待不到就绪的事件就去偷任务还是在任务队列里没有任务就直接去偷任务
        // 设置等待时间=0，如果没有就绪的时间就立刻去干别的
        int n = epoll_wait(resource_->get_epoll_fd(), events, MAX_EVENT, 0);
        for(int i=0; i<n; i++){
            Connection* connection = (Connection*)events[i].data.ptr;

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
    // 补全连接类对象
    connection->set_task_queue(task_queue_.get());
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
}

void WorkerThread::handle_del_connection(Connection* connection){
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, connection->get_socket(), nullptr)==-1)
        std::cerr<<"[工作线程] 线程"<<ID_<<": 移除监听失败:"<<errno<<std::endl;
    close(connection->get_socket());
    std::cout<<"[工作线程] 线程"<<ID_<<": 已关闭来自"<<connection->get_ip()<<"的连接"<<std::endl;
    delete connection;
}

void WorkerThread::modify_epoll_events(Connection* connection, uint32_t op){
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