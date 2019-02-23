//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_MAINFRAME_H
#define PLAZA_MAINFRAME_H

#include <Core/NativeUtils.h>

struct MainFrame {
    Entity MainFrameRenderer, MainFrameCamera, MainFramePicker;
};

struct MainFrameStyle {
    Entity MainFrameStyleMesh;
};

Unit(MainFrame)
    Component(MainFrame)
        ChildProperty(SceneRenderer, MainFrameRenderer)
        ChildProperty(Camera, MainFrameCamera)
        ChildProperty(GuiPickRay, MainFramePicker)

    Component(MainFrameStyle)
        ChildProperty(WidgetMesh, MainFrameStyleMesh)

#endif //PLAZA_MAINFRAME_H
