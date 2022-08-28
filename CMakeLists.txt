cmake_minimum_required(VERSION 3.23)
project(http)

set(CMAKE_CXX_STANDARD 14)

add_library(http SHARED httpl_wrapper.cpp httpl_memory.h httpl_memory.cpp content_provider.h content_provider.cpp HTTPResponse.cpp HTTPRequest.cpp StreamIO.cpp StreamIO.h httpl_network.cpp httpl_network.h httpl_err.h TaskQueue.cpp TaskQueue.h HTTPConnection.cpp HTTPConnection.h HTTPContent.h httpl_common.h HTTPHeader.cpp)