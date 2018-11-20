# Install script for directory: C:/Users/Kim/Documents/Plaza/BgfxRendering

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "c:/Users/Kim/Documents/Plaza/.install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Assets/Shaders" TYPE FILE FILES
    "C:/Users/Kim/Documents/Plaza/build/BgfxRendering/ex_bgfx-prefix/src/ex_bgfx/bgfx/examples/common/shaderlib.sh"
    "C:/Users/Kim/Documents/Plaza/build/BgfxRendering/ex_bgfx-prefix/src/ex_bgfx/bgfx/examples/common/common.sh"
    "C:/Users/Kim/Documents/Plaza/build/BgfxRendering/ex_bgfx-prefix/src/ex_bgfx/bgfx/src/bgfx_shader.sh"
    "C:/Users/Kim/Documents/Plaza/build/BgfxRendering/ex_bgfx-prefix/src/ex_bgfx/bgfx/src/bgfx_compute.sh"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Kim/Documents/Plaza/build/BgfxRendering.dll")
endif()

