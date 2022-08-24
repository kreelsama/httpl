//
// Created by kreel on 8/23/22.
//

#include <cstdlib>
#include <cstring>

void *httpl_zalloc(unsigned int size){
    void *ptr;
    ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
        return ptr;
    }
    return nullptr;
}

//void httpl_free(void *buffer){
//    if(!buffer)
//        free(buffer);
//}