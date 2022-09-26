//
// Created by kreel on 8/28/22.
//
#include "HTTPContent.h"
#include "httpl_err.h"

#include <vector>
#include <ctime>
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

static string get_UTC_time(){
    time_t t = time(nullptr);
    string res = asctime(localtime(&t));
    res.pop_back(); // remove trailing '\n'
    res += " GMT";
    return res;
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
        request_type = ServerResp;
        set_http_version(line);
        space_pos = line.find(' ');
        line = line.substr(space_pos + 1);
        space_pos = line.find(' ');
        int code;
        code = strtol(line.c_str(), nullptr, 10);
        set_status_code(code);
    } else { // Client request with HTTP Method and requested URI
        request_type = ClientReq;
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
        update(attr, value);
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

// Only a Request Header need to be serialized
string HTTPHeader::serialize() {
    string http_header;
    set_date();
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

void HTTPHeader::set_request_uri(const string &req_uri) {
    uri = req_uri;
}

string HTTPHeader::get_request_uri() {
    return uri;
}

void HTTPHeader::set_GET_param(const std::string &name, const std::string &value) {
    GET_param[name] = value;
}

void HTTPHeader::update_GET_param_from_URI() {
    if(method != GET)
        return;

    int pos = 0;
    bool escaped = false;

    for(pos = 0; pos < uri.length(); ++pos){
        if(uri.at(pos) == '?' and not escaped){
            break;
        }
        if(uri.at(pos) == '\\'){
            if(escaped){
                escaped = false;
            } else escaped = true;
        }
    }
    if(pos == uri.length()) // last char is '?'
        GET_param.clear();

    string param = uri.substr(pos + 1);
    bool find_key = true;
    escaped = false;
    string key, value;

    auto append = [&](char c){
        if (find_key) {
            key += c;
        } else value += c;
    };

    for(pos = 0;pos < param.length(); pos ++){
        if(param.at(pos) == '&' and not escaped){
            find_key = true;
            if(not key.empty()){
                GET_param[key] = value;
            }
            key.clear();
            value.clear();
        }
        else if(param.at(pos) == '\\'){
            if(escaped){
                escaped = false;
                append('\\');
            } else escaped = true;
        }
        else if(param.at(pos) == '='){ // to find value
            if(escaped){
                escaped = false;
                append('=');
            } else find_key = false;
        }
        else {
            append(param.at(pos));
        }
    }

}

string HTTPHeader::GET_param2string() {
    string GET_string("?");

    auto append_string = [](string& to_append, const string& escaped_string){
        const string special_characters= R"(\?=&: )";
        auto l = special_characters.length();
        for(auto&& c:escaped_string){
            for(auto i = 0; i < l; ++i){
                if(special_characters[i] == c){
                    to_append += '\\';
                    break;
                }
            }
            to_append += c;
        }
    };

    for(const auto & it : GET_param){
        auto attr = it.first;
        auto value = it.second;
        append_string(GET_string, attr);
        GET_string += '=';
        append_string(GET_string, value);
        GET_string += '&';
    }

    GET_string.pop_back(); // remove trailing '&'
    // If the string is empty of parameters, the pop_back should pop '?'
    return GET_string;
}

void HTTPHeader::set_date() {
    if(header["Date"].empty()){
        update("Date", get_UTC_time());
    }
}

HTTPHeader::HTTPHeader() {
    set_date();
}
