//
// Created by kreel on 8/28/22.
//

#ifndef HTTP_HTTPL_COMMON_H
#define HTTP_HTTPL_COMMON_H

#define SERVER_DESC "httpl_static_server/0.0.1"

#define CRLF "\r\n"
#define DELIMITER CRLF CRLF

enum http_version {
    HTTP_1_1,
    HTTP_2,
    HTTP_3,
    HTTP_HYBRID
};

#endif //HTTP_HTTPL_COMMON_H
