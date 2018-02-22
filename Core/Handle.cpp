//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/Handle.h>

Index GetHandleIndex(Handle handle) {
    return ((Index *) &handle)[0];
}

Generation GetHandleGeneration(Handle handle) {
    return ((Generation *) &handle)[1];
}

Handle GetHandle(Index index, Generation generation) {
    u32 data[2];
    data[0] = index;
    data[1] = generation;

    return *((Handle *) &data);
}
