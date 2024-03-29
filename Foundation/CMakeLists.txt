cmake_minimum_required(VERSION 3.9)
project(Foundation)

set(CMAKE_CXX_STANDARD 14)

add_library(Foundation CommandLine.h CommandLine.cpp AppLoop.cpp AppLoop.h Visibility.cpp Visibility.h FoundationModule.cpp FoundationModule.h Stream.cpp Stream.h MemoryStream.cpp MemoryStream.h PersistancePoint.cpp PersistancePoint.h VirtualPath.cpp VirtualPath.h Task.cpp Task.h ValueConverter.cpp ValueConverter.h Invocation.cpp Invocation.h Timer.cpp Timer.h StopWatch.cpp StopWatch.h NativeUtils.h Win32/unistd.h Win32/dirent.h Invalidation.cpp Invalidation.h StdStream.cpp StdStream.h)

target_include_directories(Foundation PUBLIC ".." ".")

target_link_libraries(Foundation Core)

if(WIN32)
    target_link_libraries(Foundation shlwapi)
endif()

IF(UNIX)
    target_link_libraries(Foundation pthread)
ENDIF()

if(MSVC)
    target_sources(Foundation PRIVATE Foundation.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Foundation RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()