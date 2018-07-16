cmake_minimum_required(VERSION 3.9)
project(Rest)

set(CMAKE_CXX_STANDARD 14)

add_library(Rest RestModule.cpp RestModule.h RestEntityRouting.cpp RestEntityRouting.h RestResourceRouting.cpp RestResourceRouting.h RestRouting.cpp RestRouting.h HttpStream.cpp HttpStream.h)

target_link_libraries(Rest PRIVATE Core Foundation Json sws)
target_compile_options(Rest PRIVATE "-fexceptions")
target_include_directories(Rest PUBLIC ".." "asio/include" "SimpleWebServer")
