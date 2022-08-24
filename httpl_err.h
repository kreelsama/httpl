//
// Created by kreel on 8/24/22.
//

#ifndef HTTP_HTTPL_ERR_H
#define HTTP_HTTPL_ERR_H

#include <cstdio>
#include <cerrno>

#define ERR_WITH_ERRNO(msg) \
    fprintf(stderr, "%s:line %d:%s:%s\n", __FILE_NAME__, __LINE__, msg, strerror(errno));

#endif //HTTP_HTTPL_ERR_H
