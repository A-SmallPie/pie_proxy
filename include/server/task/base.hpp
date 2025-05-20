#pragma once
class Connection;
class ThreadResource;

// 静态多态实现任务
class BaseTask{
public:
    virtual ~BaseTask();
    virtual void execute(ThreadResource* res);
};

// CRTP模板类（仅声明）
template <typename Impl>
class Task: public BaseTask{
public:
    void execute(ThreadResource* res) final override;
};

