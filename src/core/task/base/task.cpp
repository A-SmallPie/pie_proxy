#include "core/task/base/BaseTask.hpp"
#include "core/task/server/ServerDelConnection.hpp"
#include "core/task/server/ServerAddConnection.hpp"
#include "core/resource/ServerThreadResource.hpp"

BaseTask::~BaseTask()=default;

void BaseTask::execute(ServerThreadResource& resource){
    std::cout<<"[线程"<<resource.get_id()<<"]: 尝试使用基础Task"<<std::endl;
    throw std::runtime_error("未实现execute方法");
}

template <typename Impl>
void Task<Impl>::execute(ServerThreadResource& res){
    static_cast<Impl*>(this)->run(res);
};

template class Task<ServerAddConnection>;
template class Task<ServerDelConnection>;