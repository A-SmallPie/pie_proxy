#pragma once
#include <vector>
#include <atomic>
#include <string>

class BaseConnection{
protected:
    std::atomic<bool> is_closed_{false};
public:
    virtual ~BaseConnection()=0;
    virtual void self_close()=0;
};
