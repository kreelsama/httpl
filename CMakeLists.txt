cmake_minimum_required(VERSION 3.23)
project(http)

set(CMAKE_CXX_STANDARD 14)

add_library(http SHARED httpl.cpp httpl_memory.h httpl_memory.cpp content_provider.h content_provider.cpp HTTPResponse.cpp HTTPResponse.h HTTPRequest.cpp HTTPRequest.h StreamIO.cpp StreamIO.h httpl_network.cpp httpl_network.h httpl_err.h)