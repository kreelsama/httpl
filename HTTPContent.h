//
// Created by kreel on 8/28/22.
//

#ifndef HTTP_HTTPCONTENT_H
#define HTTP_HTTPCONTENT_H

#include <unordered_map>
#include <string>
#include "httpl_common.h"

using std::string;
using header_t = std::unordered_map<std::string, std::string>;

enum HTTPMethods{
    GET,
    POST,
    HEAD,
    OTHERS // unimplemented
}; // only these three supported

std::unordered_map<int, string> status_desc = {
        {100, "Continue"}, {103, "Early Hints"},

        {200, "OK"}, {201, "Created"}, {202, "Accepted"},
        {204, "No Content"}, {206, "Partial Content"},

        {301, "Moved Permanently"}, {302, "Found"}, {303, "See Other"}, {304, "Not Modified"},
        {307, "Temporary Redirect"},{308, "Permanent Redirect"},

        {400, "Bad Request"}, {401, "Unauthorized"}, {403, "Forbidden"}, {404, "Not Found"},
        {405, "Method Not Allowed"}, {406, "Not Acceptable"}, {407, "Proxy Authentication Required"},
        {408, "Request Timeout"}, {409, "Conflict"}, {410, "Gone"},
        {412, "Precondition Failed"}, {413, "Payload Too Large"},
        {414, "URI Too Long"}, {415, "Unsupported Media Type"}, {416, "Range Not Satisfiable"},
        {471, "Expectation Failed"}, {418, "I'm a teapot"}, {425, "Too Early"},
        {426, "Upgrade Required"}, {429, "Too Many Requests"}, {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},

        {500, "Internal Server Error"},{501, "Not Implemented"},{502, "Bad Gateway"},
        {503, "Service Unavailable"}, {504, "Gateway Timeout"},{505, "HTTP Version Not Supported"}
};

class HTTPRequest {
    HTTPRequest(string req_body);
};

class HTTPResponse {
    HTTPResponse();
};

class HTTPHeader {
    HTTPHeader();
    explicit HTTPHeader(const header_t& init_header);
    explicit HTTPHeader(const string& header_string);

    void update(const header_t& to_append);
    void update(const string& attr, const string& value);

    // support header["attr"] = "val";
    string& operator[](const string& attr);
    HTTPHeader& operator+(const header_t& to_append);
    HTTPHeader& operator+=(const header_t& to_append);

    string serialize();
    void set_status_code(int code);
    void set_http_version(http_version version);
    void set_http_version(const string& version);
    HTTPMethods get_http_method(const string& meth);
    void set_http_method(HTTPMethods meth);
    HTTPMethods get_http_method();

private:
    enum {ClientReq, ServerResp} request_type;
    header_t header;
    HTTPMethods method;
    string version_served;
    string status_string;
    string uri; // for requested resource
};
#endif //HTTP_HTTPCONTENT_H
