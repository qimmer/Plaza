cmake_minimum_required(VERSION 3.9)
project(DataModel)

set(CMAKE_CXX_STANDARD 14)

add_library(DataModel DataModelModule.cpp DataModelModule.h CppPersistance.cpp CppPersistance.h)

target_include_directories(DataModel PUBLIC "..")

target_link_libraries(DataModel Foundation Core)

if(BUILD_SHARED_LIBS)
install (TARGETS DataModel RUNTIME DESTINATION bin)
    endif()