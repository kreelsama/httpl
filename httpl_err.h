//
// Created by kreel on 8/24/22.
//

#ifndef HTTP_HTTPL_ERR_H
#define HTTP_HTTPL_ERR_H

#include <cstdio>
#include <cerrno>
#include <cstring>

#define ERR_WITH_ERRNO(msg) \
    fprintf(stderr, "%s:line %d:%s:%s\n", __FILE_NAME__, __LINE__, msg, strerror(errno));


#define ERR_WITH_FORMAT(msg, ...) \
    fprintf(stderr, "%s:line %d:%s", __FILE_NAME__, __LINE__, msg); \
    fprintf(stderr, msg, __VA_ARGS__);

// Requested Resource Error Code
enum ERRCODE {
    E_PERMISSION,
    E_FILE,
    E_REMOTE_SOCKET,
    E_HTTP_CONNECTION,
};

#endif //HTTP_HTTPL_ERR_H
