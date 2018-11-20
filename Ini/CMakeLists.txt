cmake_minimum_required(VERSION 3.9)
project(Ini)

set(CMAKE_CXX_STANDARD 14)

add_library(Ini IniPersistance.h IniPersistance.cpp IniModule.cpp IniModule.h)

target_include_directories(Ini PUBLIC "..")

target_link_libraries(Ini Foundation Core Json)

if(MSVC)
    target_sources(Ini PRIVATE Ini.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Ini RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()