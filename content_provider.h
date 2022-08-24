//
// Created by kreel on 8/23/22.
//
#ifndef HTTP_CONTENT_PROVIDER_H
#define HTTP_CONTENT_PROVIDER_H

#include "httpl.h"

using std::string;

typedef struct mapping {
    string uri;
    string dir;
    struct mapping *next;
}static_map;

class Content_Provider {
public:
    Content_Provider();
    explicit Content_Provider(const string& dir);
    Content_Provider(const string& uri, const string& dir);

    void add_static_map(const string& uri,const string& dir);
    int set_web_base(const string& dir);

    ~Content_Provider();
protected:
    static_map *static_mapping;
    static static_map *create_static_map(const string& uri, const string& dir);
    void clear_static_map();
};

#endif //HTTP_CONTENT_PROVIDER_H
