#include "common/task/base.hpp"
#include "common/task/del_connection.hpp"
#include "common/task/add_connection.hpp"
#include "server/thread_resource.hpp"

BaseTask::~BaseTask()=default;

void BaseTask::execute(ThreadResource& resource){
    std::cout<<"[线程"<<resource.get_id()<<"]: 尝试使用基础Task"<<std::endl;
    throw std::runtime_error("未实现execute方法");
}

template <typename Impl>
void Task<Impl>::execute(ThreadResource& res){
    static_cast<Impl*>(this)->run(res);
};

template class Task<AddConnection>;

template class Task<DelConnection>;