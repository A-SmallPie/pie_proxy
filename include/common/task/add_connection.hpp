#include <string>
#include <memory>
#include "base.hpp"
#include "server/thread_resource.hpp"

class AddConnection: public Task<AddConnection>{
private:
    int client_socket_;
    std::string client_ip_;
    const size_t RECV_BUFFER_SIZE;
public:
    explicit AddConnection(int fd, std::string ip, size_t buffer_size);
    ~AddConnection();
    void run(ThreadResource& res);
};