//
// Created by kreel on 8/24/22.
//

#ifndef HTTP_HTTPL_NETWORK_H
#define HTTP_HTTPL_NETWORK_H

#define BACKLOG_LISTENERS 10
enum PROTOCOL{
    TCP,
    UDP,
    FILE_IO,
    HTTP,
    HTTPS
};

struct addr_prefixes{
    const char* proto_prefix;
    int type;
};

struct httpl_addrinfo{
    int domain;
    int type;
    char* addr;
    unsigned int addrlen;
    int port;
};


int bind_to_socket(const char *addr, unsigned int port, PROTOCOL proto);
int accept_one(const int& fd);
httpl_addrinfo* get_addr_info(const char* addr);

#endif //HTTP_HTTPL_NETWORK_H
