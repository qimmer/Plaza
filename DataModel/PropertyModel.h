
#ifndef PropertyModel_H
#define PropertyModel_H

#include <Core/Entity.h>


    DeclareComponent(PropertyModel)
    DeclareService(PropertyModel)

    DeclareComponentProperty(PropertyModel, StringRef, PropertyModelType)
    DeclareComponentProperty(PropertyModel, bool, PropertyModelPublic)

#endif //PropertyModel_H
