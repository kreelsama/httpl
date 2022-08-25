//
// Created by kreel on 8/24/22.
//

#ifndef HTTP_STREAMIO_H
#define HTTP_STREAMIO_H

#include <string>

enum StreamType{
    FILE_IO,
    NETWORK_IO,
    HTTP_IO,
    INVALID_IO
};

class StreamIO {

    StreamIO(const std::string& addr);

    std::string read(int n=-1);
    virtual int write(const std::string& buf)=0;

    bool is_valid();
    bool is_empty();
    int close();

    ~StreamIO();

protected:
    int io_fd;
    bool valid;
    StreamType iotype=INVALID_IO;
};


#endif //HTTP_STREAMIO_H
