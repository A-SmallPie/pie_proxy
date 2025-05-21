#include "server/task/base.hpp"
#include "server/task/del_connection.hpp"
#include "server/task/add_connection.hpp"
#include "server/thread_resource.hpp"

BaseTask::~BaseTask()=default;

template <typename Impl>
void Task<Impl>::execute(ThreadResource& res){
    static_cast<Impl*>(this)->run(res);
};

template class Task<AddConnection>;

template class Task<DelConnection>;