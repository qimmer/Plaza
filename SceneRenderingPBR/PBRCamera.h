//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_PBRCAMERA_H
#define PLAZA_PBRCAMERA_H

Unit(PBRCamera)
    Component(PBRModelBatch)
        ReferenceProperty(Model, PBRModelBatchModel)

    Component(PBRPointLightBatch)
        ReferenceProperty(Light, PBRPointLightBatchLight)

    Component(PBRCamera)
        ChildComponent(OffscreenRenderTarget, PBRCameraGBuffer)
        ChildComponent(CommandList, PBRCameraSceneCommandList)
        ChildComponent(CommandList, PBRCameraLightingCommandList)

#endif //PLAZA_PBRCAMERA_H
