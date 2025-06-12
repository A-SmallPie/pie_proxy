#include "core/task/base/BaseTask.hpp"
#include "core/task/server/ServerDelConnection.hpp"
#include "core/task/server/ServerAddConnection.hpp"
#include "core/resource/ResourceVariant.hpp"

BaseTask::~BaseTask()=default;

template <typename Impl>
void Task<Impl>::execute(const ResourceVariant res){
    static_cast<Impl*>(this)->run(res);
};

template class Task<ServerAddConnection>;
template class Task<ServerDelConnection>;