#include <netdb.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "core/task/base/BaseTask.hpp"
#include "server/ThreadPool.hpp"
#include "core/connection/ServerConnection.hpp"
#include "core/task/server/ServerAddConnection.hpp"

#define BACK_LOG 5
#define BUFFER_SIZE 1024

void listAllServerIPs() {
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("[主线程]: 获取网络接口失败");
        return;
    }

    std::cout <<"[主线程]: 服务器所有可用IP地址:" << std::endl;
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        sockaddr_in* addr = (sockaddr_in*)ifa->ifa_addr;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
        
        std::cout << "   - " << ifa->ifa_name << ": " << ip << std::endl;
    }

    freeifaddrs(ifaddr);
}


int main(int argc, char *argv[])
{
    int port;
    if(argc<2){
        std::cerr<<"[主线程]: 用法: "<<argv[0]<<" <端口>"<<std::endl;
        return 1;
    }
    else{
        port = std::atoi(argv[1]);
    }
    ThreadPool* thread_pool = new ThreadPool(2, 64);
    thread_pool->run_all();
    /*
    第一个参数为地址族，在这里表示用ipv4进行通信
    第二个参数表示建立可靠的连接，即使用tcp进行通信
    第三个参数为协议参数，0是指自动选择协议参数
    定义的socket不会自带一个端口，必须手动制定
    */
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket<0){
        std::cerr<<"[主线程]: 无法创套接字!"<<std::endl;
        return 1;
    }
    /*
    设置允许地址重用，适配频繁重启服务器的情况
    否则接口关闭后，会进入TIME_WAIT阶段，此时端口被操作系统接管，没有办法被绑定
    设置了允许重用后，就可以绑定到处于TIME_WAIT状态的端口，也可以实现多个套接字绑定到一个端口
    */
    int opt=1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        std::cerr<<"[主线程]: 设置套接子选项失败"<<std::endl;
        close(server_socket);
        return 1;
    }

    sockaddr_in server_addr{};                  // 用于存储ipv4地址和端口信息的结构体
    server_addr.sin_family = AF_INET;           // 使用ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY;   // 允许任何ip地址连接(0.0.0.0)(bind的时候，这个字段决定了服务器的套接子会被绑定到哪个ip地址)
    server_addr.sin_port = htons(port);         // 设置网络端口号（转为网络字节序：大端）

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cerr<<"[主线程]: 绑定端口失败"<<std::endl;
        close(server_socket);
        return 1;
    }
    /*
    为什么要监听端口？将socket从主动模式变成被动模式
    主动模式下的socket不能接受连接（可能默认是客户端的端口？）
    backlog是指最多有多少个用户可以排队等accept（）
    绑定的端口和监听的端口是同一个，listen只是让socket开始监听这个端口
    bind告诉操作操作系统这个socket要使用这个端口
    listen会让操作系统创建一个等待连接队列，socket开始鉴定这个端口上的syn包
    */
    if(listen(server_socket, BACK_LOG) < 0){
        std::cerr<<"[主线程]: 监听端口失败"<<std::endl;
        close(server_socket);
        return 1;
    }

    std::cout<<"[主线程]: 开始监听端口："<<port<<std::endl;
    listAllServerIPs();

    while(true){

        sockaddr_in client_addr{};
        socklen_t client_addr_len = sizeof(client_addr);
        /*
        三个参数：第一个是指定从哪个监听套接字上接受连接，第二个参数用于存储客户端地址，第三个是告诉内核地址的长度
        因为不同协议的地址长度可能不一样，所以需要制定地址长度
        如果不关心连接的来源，后面两个可以都填null
        */
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if(client_socket < 0){
            std::cerr<<"[主线程]: 接受连接失败"<<std::endl;
        }
        
        // 输出建立连接的客户端信息
        char client_IP[INET_ADDRSTRLEN];
        // 将二进制ip地址转化为人类可读的ip地址
        inet_ntop(AF_INET, &client_addr.sin_addr, client_IP, INET_ADDRSTRLEN);
        std::cout<<"[主线程]: 接受端连接："<<client_IP<<": "<<ntohs(client_addr.sin_addr.s_addr)<<std::endl;
        
        thread_pool->dispatchTask(std::make_unique<ServerAddConnection>(client_socket, client_IP, 4096));

    }
    close(server_socket);
    std::cout<<"[主线程]: 服务端"<<port<<"已关闭"<<std::endl;

    return 0;

}

