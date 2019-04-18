//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_MAINFRAME_H
#define PLAZA_MAINFRAME_H

#include <Core/NativeUtils.h>

struct MainFrame {
    Entity MainFrameRenderer, MainFrameCamera, MainFrameController;
};

struct MainFrameStyle {
    Entity MainFrameStyleMesh;
};

Unit(MainFrame)
    Component(MainFrame)
        ChildProperty(SceneRenderer, MainFrameRenderer)
        ChildProperty(Camera, MainFrameCamera)
        ChildProperty(GuiController, MainFrameController)

    Component(MainFrameStyle)
        ChildProperty(WidgetMesh, MainFrameStyleMesh)

    Prefab(MainFrame)

#endif //PLAZA_MAINFRAME_H
