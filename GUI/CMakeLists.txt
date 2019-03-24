cmake_minimum_required(VERSION 3.9)
project(Gui)

set(CMAKE_CXX_STANDARD 14)

add_library(Gui GuiModule.cpp GuiModule.h Widget.cpp Widget.h Font.cpp Font.h GuiController.cpp GuiController.h DragHandle.cpp DragHandle.h TextWidget.cpp TextWidget.h Layout.cpp Layout.h)

target_include_directories(Gui PUBLIC ".." ".")

target_link_libraries(Gui Foundation Core Rendering Scene Animation)

if(MSVC)
    target_sources(Gui PRIVATE Gui.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Gui RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()