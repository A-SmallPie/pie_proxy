#include <string>
#include <memory>
#include "base.hpp"
#include "server/thread_resource.hpp"

class AddConnection: public Task<AddConnection>{
private:
    int fd_;
    std::string ip_;
    const size_t BUFFER_SIZE;
public:
    explicit AddConnection(int fd, std::string ip, size_t buffer_size);
    ~AddConnection();
    void run(ThreadResource& res);
};