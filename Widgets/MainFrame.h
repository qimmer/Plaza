//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_MAINFRAME_H
#define PLAZA_MAINFRAME_H

#include <Core/NativeUtils.h>

struct MainFrame {
    Entity MainFrameRenderer, MainFrameCamera;
};

struct MainFrameStyle {
    Entity MainFrameStyleMesh;
};

Unit(MainFrame)
    Component(MainFrame)
        ChildProperty(SceneRenderer, MainFrameRenderer)
        ChildProperty(Camera, MainFrameCamera)

    Component(MainFrameStyle)
        ChildProperty(WidgetMesh, MainFrameStyleMesh)

#endif //PLAZA_MAINFRAME_H
