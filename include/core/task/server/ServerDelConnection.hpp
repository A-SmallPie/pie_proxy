#include "core/task/base/BaseTask.hpp"
#include "core/resource/ServerThreadResource.hpp"

class ServerDelConnection: public Task<ServerDelConnection>{
private:
    int client_socket_;
public:
    explicit ServerDelConnection(int fd);
    ~ServerDelConnection();
    void run(ServerThreadResource& res);
};