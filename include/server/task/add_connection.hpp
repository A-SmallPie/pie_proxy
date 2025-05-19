#include "base.hpp"

class AddConnection: public Task<AddConnection>{
private:
    int socket_fd_;
public:
    explicit AddConnection(int fd);
    void run(ThreadResource &res);
};