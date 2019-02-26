cmake_minimum_required(VERSION 3.9)
project(Widgets)

set(CMAKE_CXX_STANDARD 14)

add_library(Widgets WidgetsModule.h WidgetsModule.cpp MainFrame.cpp MainFrame.h Dock.cpp Dock.h Button.cpp Button.h Checkbox.cpp Checkbox.h Style.cpp Style.h Panel.cpp Panel.h Menu.cpp Menu.h ListItem.cpp ListItem.h)

target_include_directories(Widgets PUBLIC ".." ".")
target_link_libraries(Widgets Foundation Core Gui Rendering)

if(MSVC)
    target_sources(Widgets PRIVATE Widgets.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Widgets RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()