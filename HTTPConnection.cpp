//
// Created by kreel on 8/28/22.
//

#include "HTTPConnection.h"
#include "StreamIO.h"
#include "HTTPContent.h"

class HTTPConnection {
public:
    HTTPConnection(int conn, const Content_Provider& cp);
    ~HTTPConnection();

    void start();

private:
    string receive_or_die();
    string send_response();
    int fd;
    Content_Provider provider;
};


void handle_http_connection(int io, const Content_Provider& provider){
    HTTPConnection connection(io, provider);
    connection.start();
}

HTTPConnection::HTTPConnection(int conn, const Content_Provider& cp){
    fd = conn;
    provider = cp;
}