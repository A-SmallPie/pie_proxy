#include "common/task/base.hpp"
#include "server/thread_resource.hpp"

ThreadResource::ThreadResource(size_t id, size_t max_event):
ID_(id), MAX_EVENT(max_event), queue_(), manager_(){
    epoll_fd_ = epoll_create1(0);
    if(epoll_fd_<0){
        std::cerr<<"[线程"<<ID_<<"]: 创建epoll实例失败"<<std::endl;
        exit(EXIT_FAILURE);
    }
}

ThreadResource::~ThreadResource()=default;

size_t ThreadResource::get_id()const{
    return ID_;
}

// 实现操作epoll的接口
size_t ThreadResource::get_max_events()const{
    return MAX_EVENT;
}

int ThreadResource::get_epoll_fd() const{
    return epoll_fd_;
}

bool ThreadResource::epoll_add(int fd, epoll_event* event){
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, event)==-1){
        manager_.remove_connection(fd);
        std::cerr<<"[线程"<<ID_<<"]: 添加连接事件监听失败"<<std::endl;
        return false;
    }
    return true;
}

bool ThreadResource::epoll_mod(int fd, int op, epoll_event* event=nullptr){
    if(epoll_ctl(epoll_fd_, op, fd, event)==-1){
        std::cerr<<"[线程"<<ID_<<"]: 执行epoll操作"<<op<<"失败"<<std::endl;
        return false;
    }
    return true;
}


// 需要使用透传，因为其他类也可能需要添加任务
void ThreadResource::add_task(std::unique_ptr<BaseTask> task){
    queue_.push(std::move(task));
}

std::unique_ptr<BaseTask> ThreadResource::pop_task(){
    return queue_.pop();
}

bool ThreadResource::try_pop_task(std::unique_ptr<BaseTask>& task){
    return queue_.try_pop(task);
}

// 透传连接管理器方法
bool ThreadResource::add_connection(std::shared_ptr<ServerConn> connection){
    return manager_.add_connection(connection);
}

bool ThreadResource::remove_connection(int fd){
    return manager_.remove_connection(fd);
}

size_t ThreadResource::remove_time_out_connection(){
    return manager_.remove_time_out_connection();
}