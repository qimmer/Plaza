# Install script for directory: C:/Users/Kim/Documents/Plaza

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Kim/Documents/Plaza/build/Core/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Foundation/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/File/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Networking/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Rest/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Test/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Input/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Animation/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Rendering/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Scene/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Gui/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Sprite/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Json/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Ini/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/StbDecoders/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/Debug/cmake_install.cmake")
  include("C:/Users/Kim/Documents/Plaza/build/BgfxRendering/cmake_install.cmake")

endif()

