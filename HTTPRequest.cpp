//
// Created by kreel on 8/23/22.
//

#include "HTTPContent.h"

enum compression_type{
    GZIP,
    DEFLATE
};

HTTPRequest::HTTPRequest(string req_body) {
    unsigned int pos;

    pos = req_body.find(DELIMITER);
    header = HTTPHeader(req_body.substr(0, pos));

    pos += string(DELIMITER).length();
    body = req_body.substr(pos);

    method = header.get_http_method();


}

HTTPRequest::operator bool() {
    return method == UNSET;
}
