cmake_minimum_required(VERSION 3.9)
project(Debug)

set(CMAKE_CXX_STANDARD 14)

add_library(Debug DebugModule.cpp DebugModule.h Debug.json)

target_include_directories(Debug PUBLIC "..")

target_link_libraries(Debug Core Foundation Networking Json File Rest)

install (TARGETS Debug RUNTIME DESTINATION bin)
INSTALL(FILES Debug.json DESTINATION bin )

add_executable(Debugger Debugger.cpp)
target_include_directories(Debugger PUBLIC "..")
target_link_libraries(Debugger Core Foundation Networking Json Rest Debug)
install (TARGETS Debugger RUNTIME DESTINATION bin)
