cmake_minimum_required(VERSION 3.9)
project(Test)

set(CMAKE_CXX_STANDARD 14)

add_library(Test Test.h Test.cpp TestModule.cpp TestModule.h)
target_include_directories(Test PUBLIC "..")
target_link_libraries(Test Core)

add_executable(TestRunner TestRunner.h TestRunner.cpp TestRunner.cpp TestRunner.h)
target_include_directories(TestRunner PUBLIC "..")
target_link_libraries(TestRunner Core Test)

if(BUILD_SHARED_LIBS)
    install (TARGETS Test RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()

install (TARGETS TestRunner RUNTIME DESTINATION bin)