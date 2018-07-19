cmake_minimum_required(VERSION 3.9)
project(JavaScript)

set(CMAKE_CXX_STANDARD 14)

add_library(JavaScript duktape-2.2.1/src/duktape.c JavaScriptModule.cpp JavaScriptModule.h JavaScriptContext.cpp JavaScriptContext.h JavaScriptPersistance.cpp JavaScriptPersistance.h)

target_link_libraries(JavaScript PRIVATE Core Foundation)
target_include_directories(JavaScript PUBLIC ".." "duktape-2.2.1/src")

install (TARGETS JavaScript RUNTIME DESTINATION bin)