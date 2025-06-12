#pragma once
#include "core/resource/ResourceVariant.hpp"

// 静态多态实现任务(下面这个类不是纯虚类)
class BaseTask{
public:
    virtual ~BaseTask();
    virtual void execute(const ResourceVariant res)=0;
};

// CRTP模板类（仅声明）
template <typename Impl>
class Task: public BaseTask{
public:
    void execute(const ResourceVariant res) final override;
};

