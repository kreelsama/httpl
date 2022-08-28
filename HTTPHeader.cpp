//
// Created by kreel on 8/28/22.
//
#include "HTTPContent.h"
#include "httpl_err.h"

HTTPHeader::HTTPHeader(const header_t& init_header){
    update(init_header);
}

HTTPHeader::HTTPHeader(const std::string &header_string) {
    string attr;
    string value;
    string line;
    header_t parsed;
    HTTPMethods meth;
    unsigned long line_pos, space_pos;

    line_pos = header_string.find(CRLF);
    line = header_string.substr(0, line_pos);
    // TODO: space_pos points to the end
    if(line.find("HTTP/") == 0){ // A server response without HTTP Method and URI
        set_http_version(line);
        space_pos = line.find(' ');
        line = line.substr(space_pos + 1);
        space_pos = line.find(' ');
        int code;
        code = strtol(line.c_str(), nullptr, 10);
        set_status_code(code);
    } else { // Client request with HTTP Method and requested URI
        meth = get_http_method(line);
        method = meth;
        space_pos = line.find(' ');
        line = line.substr(space_pos + 1);
        space_pos = line.find(' ');
        uri = line.substr(0,space_pos);
        line = line.substr(space_pos+1);
        set_http_version(line);
    }

    line_pos = header_string.find(CRLF);
    line = header_string.substr(line_pos + sizeof(CRLF));

    // "ATTR: VALUE"
    while(line.find(CRLF) != 0){ // consecutive CRLF indicate the end of a header string
        space_pos = line.find(' ');
        // before the space is a ":"
        if(line.at(space_pos-1) != ':'){
            ERR_WITH_FORMAT("Confused header line: %s\n", line.c_str());
            continue;
        }
        attr = line.substr(0, space_pos-1);
        value = line.substr(space_pos + 1);
        parsed[attr] = value;
        line_pos = line.find(CRLF);
    }

    update(parsed);
}

void HTTPHeader::update(const header_t &to_append) {
    for(const auto & it : to_append){
        auto attr = it.first;
        auto value = it.second;
        header[attr] = value;
    }
}

string& HTTPHeader::operator[](const std::string &attr) {
    return header[attr];
}

void HTTPHeader::set_http_version(http_version version) {
    switch (version) {
        case HTTP_1_1: version_served = "HTTP/1.1"; break;
        case HTTP_2:   version_served = "HTTP/2";   break;
        case HTTP_3:   version_served = "HTTP/3";   break;
        default: version_served = "HTTP/1.1";
    }
}

void HTTPHeader::set_http_version(const string& version) {
    if(version.find("HTTP/1.1") == 0 ||
       version.find("HTTP/2") == 0   ||
       version.find("HTTP/3") == 0)
        version_served = version;
    else
        version_served = "HTTP/1.1";
}


void HTTPHeader::set_status_code(int code) {
    if(status_desc.find(code) == status_desc.end()){
        ERR_WITH_ERRNO("Unexpected status code")
        code = 500; // return Internal Server Error for undefined status code
    }
    status_string = std::to_string(code) + " " + status_desc[code];
}

HTTPMethods HTTPHeader::get_http_method() {
    return method;
}

HTTPMethods HTTPHeader::get_http_method(const string &meth) {
    if(meth.find("GET") == 0){
        return GET;
    }
    if(meth.find("POST") == 0){
        return POST;
    }
    if(meth.find("HEAD") == 0){
        return HEAD;
    }
    return OTHERS;
}
