//
// Created by Kim Johannsen on 08/01/2018.
//

#ifndef PLAZA_RENDERINGMODULE_H
#define PLAZA_RENDERINGMODULE_H

#include <Core/Module.h>

Module(Rendering)
    Unit(Rendering)
        Component(Rendering)
            ChildProperty(Uniform, RenderingUvOffsetScaleUniform)

        Declare(Texture, White)

#define AppLoopOrder_ResourceSubmission (AppLoopOrder_Rendering * 0.5f)
#define AppLoopOrder_BatchSubmission (AppLoopOrder_Rendering * 1.5f)
#define AppLoopOrder_ResourceDownload (AppLoopOrder_Rendering * 1.75f)
#define AppLoopOrder_Present (AppLoopOrder_Rendering * 2.0f)

#endif //PLAZA_RENDERINGMODULE_H
