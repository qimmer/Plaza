cmake_minimum_required(VERSION 3.9)
project(Networking)

set(CMAKE_CXX_STANDARD 14)

add_library(Networking NetworkingModule.h NetworkingModule.cpp Server.cpp Server.h HttpServer.cpp HttpServer.h HttpRequest.cpp HttpRequest.h)

target_link_libraries(Networking Core Foundation)
target_include_directories(Networking PUBLIC ".." "asio/include")

if(WIN32)
    target_link_libraries(Networking ws2_32 wsock32)
endif()