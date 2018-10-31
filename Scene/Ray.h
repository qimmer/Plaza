//
// Created by Kim on 31-10-2018.
//

#ifndef PLAZA_RAY_H
#define PLAZA_RAY_H

#include <Core/NativeUtils.h>

struct Ray {
};

struct TraceRay {
    Entity TraceRayRenderable;
};

struct DetailedTraceRay {
    v3f DetailedTraceRayPoint, DetailedTraceRayNormal;
    u32 DetailedTraceRayPrimitiveIndex;
};

struct PickRay {
    Entity PickRaySceneRenderer;
};

Unit(Ray)
    Component(Ray)

    Component(TraceRay)
        ReferenceProperty(Renderable, TraceRayRenderable)

    Component(DetailedTraceRay)
        Property(v3f, DetailedTraceRayPoint)
        Property(v3f, DetailedTraceRayNormal)
        Property(u32, DetailedTraceRayPrimitiveIndex)

    Component(PickRay)
        ReferenceProperty(SceneRenderer, PickRaySceneRenderer)

#endif //PLAZA_RAY_H
