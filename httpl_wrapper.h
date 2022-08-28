#ifndef HTTP_HTTPL_WRAPPER_H
#define HTTP_HTTPL_WRAPPER_H

#include <iostream>
#include "content_provider.h"
#include "httpl_common.h"


class HTTPServer {
public:
    HTTPServer(const std::string& ip, unsigned int port, http_version version=HTTP_1_1);
    int add_static_map(const std::string& uri, const std::string& dir);

    int serve(int forever=true);
    int enable_ssl(const std::string& priv_certpath, const std::string& public_certpath);
    virtual ~HTTPServer() = 0;
protected:
    http_version version_used;
    int main_sockfd;
    int ssl_enabled=0;

    Content_Provider content_provider;

    int launch_HTTP_connection(const int fd);
    int launch_HTTPS_connection(const int fd);
};

#endif //HTTP_HTTPL_WRAPPER_H
