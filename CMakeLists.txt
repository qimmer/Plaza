cmake_minimum_required(VERSION 3.9)
project(Plaza)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS ON)

# Plaza Libraries
add_subdirectory(Core)
add_subdirectory(Foundation)
add_subdirectory(File)
add_subdirectory(Zip)
add_subdirectory(Json)

# Tests
add_subdirectory(Test/TestCore)
add_subdirectory(Test/TestFoundation)

# Tools
add_subdirectory(BuildTool)