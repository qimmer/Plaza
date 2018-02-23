cmake_minimum_required(VERSION 3.9)
project(Core)

set(CMAKE_CXX_STANDARD 14)

add_library(Core Entity.cpp Entity.h Types.h Type.h Pool.h Handle.h Function.h Delegate.h Debug.cpp Debug.h Handle.cpp Property.h Property.cpp Type.cpp Vector.cpp Vector.h StackContainer.h Dictionary.cpp Dictionary.h String.cpp String.h Variant.h Module.cpp Module.h Service.cpp Service.h CoreModule.cpp CoreModule.h Hierarchy.cpp Hierarchy.h)
