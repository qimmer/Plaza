
#ifndef SQLPersistancePoint_H
#define SQLPersistancePoint_H

#include <Core/Entity.h>


    DeclareComponent(SQLPersistancePoint)
    DeclareService(SQLPersistancePoint)

    DeclareComponentProperty(SQLPersistancePoint, StringRef, SelectCondition)

#endif //SQLPersistancePoint_H
