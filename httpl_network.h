//
// Created by kreel on 8/24/22.
//

#ifndef HTTP_HTTPL_NETWORK_H
#define HTTP_HTTPL_NETWORK_H

#define BACKLOG_LISTENERS 10
enum PROTOCOL{
    TCP,
    UDP,
    TCP_UDP
};


int bind_to_socket(const char *addr, unsigned int port, PROTOCOL proto);
int accept_one(const int& fd);

#endif //HTTP_HTTPL_NETWORK_H
