cmake_minimum_required(VERSION 3.9)
project(Debug)

set(CMAKE_CXX_STANDARD 14)

add_library(Debug DebugModule.cpp DebugModule.h FlowNode.cpp FlowNode.h ModuleProfiling.cpp ModuleProfiling.h)

target_include_directories(Debug PUBLIC "..")

target_link_libraries(Debug Core Foundation Networking Json File Rest)

if(MSVC)
    target_sources(Debug PRIVATE Debug.def)
endif()


if(BUILD_SHARED_LIBS)
    install (TARGETS Debug RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()

INSTALL(FILES Debug.json DESTINATION bin )

add_executable(Debugger Debugger.cpp)
target_include_directories(Debugger PUBLIC "..")
target_link_libraries(Debugger Core Foundation Networking Json Rest Debug)
install (TARGETS Debugger RUNTIME DESTINATION bin)
