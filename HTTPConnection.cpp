//
// Created by kreel on 8/28/22.
//

#include "HTTPConnection.h"
#include "StreamIO.h"
#include "HTTPContent.h"

class ServerHTTPConnection {
public:
    ServerHTTPConnection(int conn, const Content_Provider& cp);
    ~ServerHTTPConnection();

    void serve();

private:
    string receive_or_die();
    HTTPRequest receive_request();
    string send_response();
    int fd;
    int time_out = 10;
    Content_Provider provider;
};

void handle_http_connection(int io, const Content_Provider& provider){
    ServerHTTPConnection connection(io, provider);
    connection.serve();
}

void ServerHTTPConnection::serve() {

}

ServerHTTPConnection::~ServerHTTPConnection() {

}

HTTPRequest ServerHTTPConnection::receive_request() {

}

ServerHTTPConnection::ServerHTTPConnection(int conn, const Content_Provider& cp){
    fd = conn;
    provider = cp;
}

string ServerHTTPConnection::receive_or_die() {
    return std::string();
}
