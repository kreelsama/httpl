//
// Created by kreel on 8/28/22.
//
#include "HTTPContent.h"
#include "httpl_err.h"

#include <vector>
using std::vector;

static vector<string> split(const string& raw, const char* delimiter){
    vector<string> lines;
    string line;
    const unsigned step_size = strlen(delimiter), maxlen = raw.length();
    string::size_type prev_pos = 0, pos = 0;

    while(prev_pos < maxlen){
        pos = raw.substr(prev_pos).find(delimiter);
        if(pos != string::npos)
            pos += prev_pos;
        else // End of String
            pos = maxlen;
        if(pos != prev_pos) {
            line = raw.substr(prev_pos, pos - prev_pos);
            lines.push_back(line);
        }
        prev_pos = pos + step_size;
    }
    return lines;
}

HTTPHeader::HTTPHeader(const header_t& init_header){
    update(init_header);
}

HTTPHeader::HTTPHeader(const std::string &header_string) {
    string attr;
    string value;
    string line;
    header_t parsed;
    HTTPMethods meth;
    unsigned long space_pos;

    vector<string> lines = split(header_string, CRLF);

    line = lines[0];
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
        set_http_method(get_http_method(line));
        space_pos = line.find(' ');
        line = line.substr(space_pos + 1);
        space_pos = line.find(' ');
        uri = line.substr(0,space_pos);
        line = line.substr(space_pos+1);
        set_http_version(line);
    }

    if(lines.size() == 1) // Header only contains a single line
        return;

    for(int i = 1; i < lines.size(); ++i){
        line = lines[i];
        space_pos = line.find(' ');
        // before the space is a ":"
        if(line.at(space_pos-1) != ':'){
            ERR_WITH_FORMAT("Confused header line: %s\n", line.c_str());
            continue;
        }
        attr = line.substr(0, space_pos - 1);
        value = line.substr(space_pos + 1);
        parsed[attr] = value;
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

void HTTPHeader::set_http_method(HTTPMethods meth) {
    method = meth;
    switch (method) {
        case GET : method_string="GET"; break;
        case POST: method_string="POST"; break;
        case HEAD: method_string="HEAD"; break;
        default: method_string="";
    }
}

void HTTPHeader::update(const std::string &attr, const std::string &value) {
    header[attr] = value;
}

HTTPHeader& HTTPHeader::operator+(const header_t &to_append) {
    update(to_append);
    return *this;
}

void HTTPHeader::operator+=(const header_t &to_append) {
    update(to_append);
}

string HTTPHeader::serialize() {
    string http_header;
    if(method == UNSET){ // A response header
        http_header += version_served;
        http_header += " ";
        http_header += status_string;
    } else { //  A request header
        http_header += method_string;
        http_header += " ";
        http_header += uri;
        if(!version_served.empty()){
            http_header += " ";
            http_header += version_served;
        }
    }

    http_header += CRLF;

    for(const auto & it : header){
        auto attr = it.first;
        auto value = it.second;
        http_header += attr;
        http_header += ": ";
        http_header += value;
        http_header += CRLF;
    }

    http_header += CRLF;

    return http_header;

}