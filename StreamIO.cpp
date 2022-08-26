//
// Created by kreel on 8/24/22.
//

#include "StreamIO.h"
#include "httpl_network.h"
#include "httpl_err.h"

#include <unistd.h>
#include <fcntl.h>

static int fileopen(const char* filepath){
    int fd;
    fd = open(filepath, O_RDONLY);
    if(fd < 0){
        ERR_WITH_ERRNO("Open file failed");
    }
    return fd;
}

static bool fileeof(int fd){
    off_t cur_offset, end_offset;

    cur_offset = lseek(fd, 0, SEEK_CUR);
    end_offset = lseek(fd, 0, SEEK_END);

    if(cur_offset == end_offset){
        return true;
    }

    lseek(fd, cur_offset, SEEK_SET);
    return false;
}

StreamIO::StreamIO(const std::string& addr) {
    if(addr.length() <= 7){
        iotype = INVALID_IO;
        valid = false;
        io_fd = -1;
        return;
    }

    httpl_addrinfo *addrinfo;
    addrinfo = get_addr_info(addr.c_str());

    if(addrinfo->type == AF_TYPE_INVALID){
        valid = false;
        return;
    }

    if(addrinfo->type == AF_FILE){
        io_fd = fileopen(addr.c_str());
        iotype = FILE_IO;
    }else{
        io_fd = -1;
    }

    if(io_fd < 0){
        valid = false;
    }
}

bool StreamIO::is_valid() {
    return valid;
}

StreamIO::~StreamIO() {
    if(valid && io_fd > 0){
        if(::close(io_fd) < 0){
            ERR_WITH_ERRNO("Close failed, ignoring");
        }
    }
}

bool StreamIO::eof() {
    if(iotype == FILE_IO && valid){
        return fileeof(io_fd);
    }
    else{
        return true;
    }
}

std::string StreamIO::read(int n){
    char *buf;
    long cur_offset, end_offset, bytes_to_read=n;
    if(n == -1 && iotype == FILE_IO){
        cur_offset = lseek(io_fd, 0, SEEK_CUR);
        end_offset = lseek(io_fd, 0 ,SEEK_END);
        bytes_to_read = end_offset - cur_offset;
        lseek(io_fd, 0, SEEK_SET);
    }

    buf = new char[bytes_to_read];
    ::read(io_fd, buf, bytes_to_read);

    std::string s(buf);
    delete []buf;
    return s;
}

int StreamIO::fileseek(long offset) {
    if(valid && iotype == FILE_IO){
        lseek(io_fd, offset, SEEK_SET);
    }
    return 0;
}
