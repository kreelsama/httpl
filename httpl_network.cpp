//
// Created by kreel on 8/24/22.
//
#include "httpl_network.h"
#include "httpl_err.h"

#include <linux/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdlib>
#include <unistd.h>
#include <netinet/tcp.h>

#define UNIX_PATH_MAX 108

int tell_address(const char* addr){
    struct addrinfo info{}, *res;
    int type = -1;
    info.ai_family = PF_UNSPEC;
    info.ai_flags = AI_NUMERICHOST;

    int ret = getaddrinfo(addr, nullptr, &info, &res);

    // If addr is a local address, which represents a unix domain socket
    if(ret)                               type = AF_UNIX;
    if(res && res->ai_family == AF_INET)  type = AF_INET;
    if(res && res->ai_family == AF_INET6) type = AF_INET6;
    if(UNIX_PATH_MAX < strlen(addr))   type = -1;

    if(res) freeaddrinfo(res);

    return type;
}

int bind_to_socket(const char *addr, unsigned int port, PROTOCOL proto){
    int sockfd;
    int domain_type = tell_address(addr);
    int len, ret, yes=1, no=0;

    if(domain_type < 0)
        return -1;
    if(proto == TCP){
        sockfd = socket(domain_type, SOCK_STREAM, 0);
    }else {
        return -1;
    }

    if(sockfd < 0){
        ERR_WITH_ERRNO("Create socket error");
        return sockfd;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,
               reinterpret_cast<void *>(&yes),sizeof(yes));
    if(domain_type == AF_INET){
        struct sockaddr_in sock_addr{};
        inet_pton(AF_INET, addr, &sock_addr.sin_addr.s_addr);
        sock_addr.sin_addr.s_addr = INADDR_ANY;
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(port);
        ret = bind(sockfd, (struct sockaddr*) &sock_addr, len);

    }
    else if(domain_type == AF_INET6){
        struct sockaddr_in6 sock_addr{};
        inet_pton(AF_INET6, addr, &sock_addr.sin6_addr);
        sock_addr.sin6_family = AF_INET6;
        sock_addr.sin6_port = htons(port);
        ret = bind(sockfd, (struct sockaddr*) &sock_addr, len);
        setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                   reinterpret_cast<char *>(&yes),sizeof(yes));
    }else { // domain_type == AF_UNIX
        struct sockaddr_un sock_addr{};
        sock_addr.sun_family = AF_UNIX;
        strcpy(sock_addr.sun_path, addr);
        len = sizeof(sock_addr);
        unlink(addr);
        ret = bind(sockfd, (struct sockaddr*) &sock_addr, len);
        setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY,
                   reinterpret_cast<char *>(&no),sizeof(no));
    }

    if (ret) {
        ERR_WITH_ERRNO("bind error");
        close(sockfd);
        return -1;
    }

    listen(sockfd, BACKLOG_LISTENERS);

    return sockfd;
}

int accept_one(const int& sockfd){
    struct sockaddr remote_addr{};
    int len = sizeof(remote_addr);
    int fd = accept(sockfd, &remote_addr, reinterpret_cast<socklen_t *>(&len));
    if(fd < 0){
        ERR_WITH_ERRNO("accept error");
    }
    return fd;
}