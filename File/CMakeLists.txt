cmake_minimum_required(VERSION 3.9)
project(File)

set(CMAKE_CXX_STANDARD 14)

add_library(File FileStream.cpp FileStream.h Folder.cpp Folder.h FileModule.cpp FileModule.h FileWatcher/FileWatcher.cpp FileWatcher/FileWatcherWin32.cpp FileWatcher/FileWatcherOSX.cpp)

if(WIN32)
    target_link_libraries(File shlwapi)
endif()
target_include_directories(File PUBLIC ".." ".")

target_link_libraries(File Foundation Core)

if(MSVC)
    target_sources(File PRIVATE File.def)
endif()

if(BUILD_SHARED_LIBS)
install (TARGETS File RUNTIME DESTINATION bin)
    endif()