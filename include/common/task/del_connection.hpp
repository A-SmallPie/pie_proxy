#include "base.hpp"
#include "server/thread_resource.hpp"

class DelConnection: public Task<DelConnection>{
private:
    int client_socket_;
public:
    explicit DelConnection(int fd);
    ~DelConnection();
    void run(ThreadResource& res);
};