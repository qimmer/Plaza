cmake_minimum_required(VERSION 3.9)
project(DataModel)

set(CMAKE_CXX_STANDARD 14)

add_library(DataModel DataModelModule.cpp DataModelModule.h ModuleDefinition.cpp ModuleDefinition.h ServiceDefinition.cpp ServiceDefinition.h ServiceSubscriptionDefinition.cpp ServiceSubscriptionDefinition.h)

target_include_directories(DataModel PUBLIC "..")

target_link_libraries(DataModel Foundation Core)
