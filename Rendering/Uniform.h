//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_UNIFORM_H
#define PLAZA_UNIFORM_H

#include <Core/Entity.h>

DeclareComponent(Uniform)

DeclareComponentPropertyReactive(Uniform, StringRef, UniformName)
DeclareComponentPropertyReactive(Uniform, u32, UniformArrayCount)
DeclareComponentPropertyReactive(Uniform, Type, UniformType)

#endif //PLAZA_UNIFORM_H
