//
// Created by kreel on 8/23/22.
//

#ifndef HTTP_HTTPL_MEMORY_H
#define HTTP_HTTPL_MEMORY_H

#define ZALLOC(type) (type*)httpl_zalloc(sizeof(type))
#define FREE(buffer) httpl_free(buffer)

void *httpl_zalloc(unsigned int size);

#endif //HTTP_HTTPL_MEMORY_H
