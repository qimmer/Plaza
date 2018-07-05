//
// Created by Kim on 05/05/2018.
//

#ifndef PLAZA_UICAMERA_H
#define PLAZA_UICAMERA_H

#include <Core/Entity.h>

DeclareComponent(UICamera)
DeclareService(UICamera)

DeclareComponentPropertyReactive(UICamera, float, UICameraPixelsPerUnit)

#endif //PLAZA_UICAMERA_H
