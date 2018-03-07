//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_INDEXBUFFER_H
#define PLAZA_INDEXBUFFER_H

#include <Core/Entity.h>
#include <Core/Service.h>


    DeclareService(IndexBuffer)

    DeclareComponent(IndexBuffer)

    DeclareComponentProperty(IndexBuffer, bool, IndexBufferLong)
    DeclareComponentProperty(IndexBuffer, bool, IndexBufferDynamic)


#endif //PLAZA_INDEXBUFFER_H
