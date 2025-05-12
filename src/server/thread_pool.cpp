#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "thread_pool.hpp"
#include "worker_thread.hpp"


ThreadPool::ThreadPool(size_t thread_count){
    for(size_t i=0; i<thread_count; i++){
        
    }
}

void ThreadPool::dispatchTask(int client_socket){

}