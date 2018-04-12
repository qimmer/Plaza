cmake_minimum_required(VERSION 3.9)
project(Json)

set(CMAKE_CXX_STANDARD 14)

add_library(Logic State.cpp State.h MultiState.cpp MultiState.h ConditionalState.cpp ConditionalState.h ToggleState.cpp ToggleState.h LogicModule.cpp LogicModule.h InvertState.cpp InvertState.h PropertyBinding.cpp PropertyBinding.h)

target_include_directories(Logic PUBLIC "..")

target_link_libraries(Logic Foundation Core)
