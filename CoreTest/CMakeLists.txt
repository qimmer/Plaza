cmake_minimum_required(VERSION 3.9)
project(CoreTest)

set(CMAKE_CXX_STANDARD 14)

add_executable(CoreTest main.cpp TestEntity.h TestType.h Common.h)

target_link_libraries(CoreTest Core Test)