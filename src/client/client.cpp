#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    if(argc<3){
        std::cerr<<"用法: "<<argv[0]<<" <服务器ip> <端口> [消息]"<<std::endl;
        return 1;
    }
    
    const char* server_ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string message = (argc > 3) ? argv[3] : "HELLO SERVER!";

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket<0){
        std::cerr<<"创建客户端套接字失败"<<std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 取地址其实就是获取一个指向这个变量的指针
    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr)<0){
        std::cerr<<"无效的IP地址"<<std::endl;
        close(server_socket);
        return 1;
    }

    if(connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        std::cerr<<"连接服务器失败"<<std::endl;
        close(server_socket);
        return 1;
    }

    int flags = fcntl(server_socket, F_GETFL, 0);
    if(fcntl(server_socket, F_SETFL, flags | O_NONBLOCK)==-1){
        std::cerr<<"设置非阻塞模式失败"<<std::endl; 
        close(server_socket);
        return 1;
    }

    std::cout<<"已连接服务器"<<server_ip<<std::endl;
    while(std::cin>>message){
        char buffer[BUFFER_SIZE];
        ssize_t bytes_sent=send(server_socket, message.c_str(), message.size(), 0);
        if(bytes_sent<0){
            std::cerr<<"消息发送失败"<<std::endl;
            close(server_socket);
            return 1;
        }
        else{
            std::cout<<"已发送"<<bytes_sent<<"字节消息:"<<message<<std::endl;
        }

        ssize_t bytes_recv = recv(server_socket, &buffer, BUFFER_SIZE-1, 0);
        if(bytes_recv>0){
            buffer[bytes_recv] = '\0';
            std::cout<<"收到服务器回信"<<buffer<<std::endl;
        }
    }

    close(server_socket);
    return 0;

}

