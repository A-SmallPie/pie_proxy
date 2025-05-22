#pragma once
class ThreadResource;

// 静态多态实现任务(下面这个类不是纯虚类)
class BaseTask{
public:
    virtual ~BaseTask();
    virtual void execute(ThreadResource& res);
};

// CRTP模板类（仅声明）
template <typename Impl>
class Task: public BaseTask{
public:
    void execute(ThreadResource& res) final override;
};

