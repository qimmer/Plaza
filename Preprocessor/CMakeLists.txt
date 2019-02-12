cmake_minimum_required(VERSION 3.9)
project(Preprocessor)

set(CMAKE_CXX_STANDARD 14)

add_library(Preprocessor PreprocessorModule.cpp PreprocessorModule.h Lexer.cpp Lexer.h CppParser.cpp CppParser.h)

target_include_directories(Preprocessor PUBLIC ".." ".")
target_link_libraries(Preprocessor Foundation Core)

if(MSVC)
    target_sources(Preprocessor PRIVATE Preprocessor.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Preprocessor RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()