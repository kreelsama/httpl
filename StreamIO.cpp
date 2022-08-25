//
// Created by kreel on 8/24/22.
//

#include "StreamIO.h"

StreamIO::StreamIO(const std::string& addr) {
    if(addr.length() <= 7){
        iotype = INVALID_IO;
        valid = false;
        io_fd = -1;
        return;
    }
    if(addr.rfind("file://", 0) == 0){

    }
}
