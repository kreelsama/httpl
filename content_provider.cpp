//
// Created by kreel on 8/23/22.
//
#include "content_provider.h"
#include <cstdlib>

static int init_static_map(static_map *map, const string &uri, const string &dir){
    char* path;

    map->next = nullptr;
    path = realpath(dir.c_str(), nullptr);
    if (!path) {
        return 1;
    }

    map->dir = string(path);
    map->uri = uri;

    free(path);

    return 0;
}

static_map *Content_Provider::create_static_map(const std::string &uri, const std::string &dir){
    auto *map = new static_map;
    if (init_static_map(map, uri, dir)){
        free(map);
        return nullptr;
    }
    return map;
}

void Content_Provider::add_static_map(const string &uri, const string &dir){

    auto *ptr = static_mapping;

    if(!ptr)
        return;
    for(;;){
        if (ptr->uri == uri){
            auto path = realpath(dir.c_str(), nullptr);
            ptr->dir = path;
            free(path);
            return;
        }
        if(ptr->next)
            ptr = ptr->next;
        else
            break;
    }

    auto *new_map = create_static_map(uri, dir);

    ptr->next = (static_map*) new_map;
}

void Content_Provider::clear_static_map(){
    auto *cur = static_mapping;
    static_map *next;

    while(cur){
        next = cur->next;
        delete cur;
        cur = next;
    }
    static_mapping = nullptr;
}

Content_Provider::Content_Provider() {
    static_mapping = create_static_map("/", ".");
}

Content_Provider::Content_Provider(const string& dir) {
    static_mapping = create_static_map("/", dir);
}

Content_Provider::Content_Provider(const string& uri, const string& dir) {
    static_mapping = create_static_map(uri, dir);
}

int Content_Provider::set_web_base(const string &dir) {
    add_static_map("/", dir);
}

Content_Provider::~Content_Provider() {
    clear_static_map();
}
