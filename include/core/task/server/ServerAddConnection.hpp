#include <string>
#include <memory>
#include "../base/BaseTask.hpp"
#include "core/resource/ServerThreadResource.hpp"

class ServerAddConnection: public Task<ServerAddConnection>{
private:
    int client_socket_;
    std::string client_ip_;
    const size_t RECV_BUFFER_SIZE;
public:
    explicit ServerAddConnection(int fd, std::string ip, size_t buffer_size);
    ~ServerAddConnection();
    void run(ServerThreadResource& res);
};