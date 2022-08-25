//
// Created by kreel on 8/24/22.
//
#include "httpl_network.h"
#include "httpl_err.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <cctype>
#include <cstdlib>

#define UNIX_PATH_MAX 108


const addr_prefixes prefixes[] = {
        {"tcp://", TCP},
        {"udp://", UDP},
        {"file://", FILE_IO},
        {"http://", HTTP},
        {"https://", HTTPS},
};


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

// Assume to be: file://<location>/filename.ext
// tcp://ip:port OR tcp://<unix socket location>
// udp://ip:port OR udp://<unix socket location>
// http://ip:port or https://ip:port
httpl_addrinfo* get_addr_info(const char* addr){
    size_t len = strlen(addr);
    const char *s = addr;
    auto *addrinfo = new httpl_addrinfo{.port=-1};
    if(len <= 6){
        addrinfo->type = -1; // invalid address
        return addrinfo;
    }

    for(auto&& prefix : prefixes){
        if(!strncmp(addr, prefix.proto_prefix, strlen(prefix.proto_prefix))){
            addrinfo->type = prefix.type;
            s += strlen(prefix.proto_prefix);
            break;
        }
    }

    if (*s == '[' && addrinfo->type != FILE_IO){ // ipv6 start indicator
        for(len = 0; *(s+len)!=']'; ++len); // find end of ipv6 address
        addrinfo->domain = AF_INET6;
        addrinfo->addrlen = len-2;
        addrinfo->addr = new char[len-1];
        strncpy(addrinfo->addr, s+1, len-2);
        s += len;
    }
    else if (strstr(s, ":") && addrinfo->type != FILE_IO){ //existence of ":" while it's not IPv6, it is then must be IPv4
        for(len = 0; *(s+len)!=':'; ++len);
        addrinfo->domain = AF_INET;
        addrinfo->addrlen = len - 1;
        addrinfo->addr = new char[len];
        strncpy(addrinfo->addr, s, len-1);
        s += len;
    } else if (addrinfo->type == TCP || addrinfo->type == UDP){ // Unix socket without a port
        addrinfo->domain = AF_UNIX;
        for(len = 0; *s; ++len);
        addrinfo->addrlen = len;
        addrinfo->addr = new char[len+1];
        strncpy(addrinfo->addr, s, len+1);
        s += len;
    } else if (addrinfo->type == FILE_IO){ // Local file
        addrinfo->domain = AF_FILE;
        addrinfo->addrlen = len;
        addrinfo->addr = new char[len+1];
        strncpy(addrinfo->addr, s, len+1);
        s += len;
    } else{ // http or https
        addrinfo->domain = AF_UNSPEC; // we don't support http proxy yet
        goto invalid;
    }

    if (addrinfo->domain != AF_UNSPEC and addrinfo->domain != AF_UNIX){ // decide port
        char *endptr;
        if (*s != ':'){
            goto invalid;
        }
        if(tell_address(addrinfo->addr) != addrinfo->domain){
            goto invalid; //invalid ipv4 or ipv6 address
        }
        addrinfo->port = strtol(s+1, &endptr, 10);
        if(endptr == s+1){
            goto invalid;
        }

    }

    return addrinfo;
invalid:
    addrinfo->type = -1;
    return addrinfo;
}

void free_addrinfo(httpl_addrinfo *addrinfo){
    delete[] addrinfo->addr;
    delete addrinfo;
}