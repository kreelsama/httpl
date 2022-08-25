#include "httpl_wrapper.h"
#include "httpl_network.h"
#include "TaskQueue.h"

#include <iostream>
#include <unistd.h>

TaskQueue *tasks;

HTTPServer::HTTPServer(const std::string& addr, unsigned int port, http_version version)
    : version_used(version) {
    PROTOCOL proto;
    string ip;
    if(addr == "localhost"){
        ip = "127.0.0.1";
    }
    if (version == HTTP_1_1) {
        main_sockfd = bind_to_socket(ip.c_str(), port, TCP);
    } else
        throw std::exception();

    if(!tasks){
        tasks = new TaskQueue;
    }
}

int HTTPServer::serve(int forever) {

    do {
        int clientfd = accept_one(main_sockfd);

    }while(forever);
    return 1;
}

HTTPServer::~HTTPServer() {
    if (main_sockfd > 0){
        close(main_sockfd);
    }

}

int HTTPServer::add_static_map(const std::string& uri, const std::string& dir) {
    content_provider.add_static_map(uri, dir);
    return 0;
}

int HTTPServer::launch_HTTP_connection(const int &fd) {
    return 0;
}