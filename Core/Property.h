//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/Handle.h>
#include <Core/Type.h>
#include <Core/Enum.h>

DeclareHandle(Property)
DeclareType(Property)

typedef void(*PropertyTransferFunc)(Handle src, Handle dst);

Type GetPropertyOwner(Property property);
Type GetPropertyType(Property property);
void* GetPropertyGetter(Property property);
void* GetPropertySetter(Property property);
const char * GetPropertyName(Property property);
PropertyTransferFunc GetPropertyTransferFunc(Property property);
Enum GetPropertyEnum(Property property);
void* GetPropertyChangedSubscribeFunc(Property property);
void* GetPropertyChangedUnsubscribeFunc(Property property);

Property FindPropertyByName(StringRef name);

void SetPropertyOwner(Property property, Type type);
void SetPropertyType(Property property, Type type);
void SetPropertyGetter(Property property, void* getter);
void SetPropertySetter(Property property, void *setter);
void SetPropertyName(Property property, const char * name);
void SetPropertyTransferFunc(Property property, PropertyTransferFunc func);
void SetPropertyChangedSubscribeFunc(Property property, void* func);
void SetPropertyChangedUnsubscribeFunc(Property property, void* func);
void SetPropertyEnum(Property property, Enum e);

#define DefineProperty(PROPERTYTYPE, PROPERTYNAME) \
        Property prop_ ## PROPERTYNAME = CreateProperty(); \
        SetPropertyOwner(prop_ ## PROPERTYNAME, type); \
        SetPropertyType(prop_ ## PROPERTYNAME, TypeOf_ ## PROPERTYTYPE ()); \
        SetPropertyName(prop_ ## PROPERTYNAME, #PROPERTYNAME ); \
        SetPropertyGetter(prop_ ## PROPERTYNAME, (void*)& Get ## PROPERTYNAME); \
        SetPropertySetter(prop_ ## PROPERTYNAME, (void*)& Set ## PROPERTYNAME); \
        SetPropertyTransferFunc(prop_ ## PROPERTYNAME, Copy ## PROPERTYNAME); \

#define DefinePropertyReactive(PROPERTYTYPE, PROPERTYNAME) \
        DefineProperty(PROPERTYTYPE, PROPERTYNAME) \
        SetPropertyChangedSubscribeFunc(prop_ ## PROPERTYNAME, (void*)Subscribe ## PROPERTYNAME ## Changed); \
        SetPropertyChangedUnsubscribeFunc(prop_ ## PROPERTYNAME, (void*)Unsubscribe ## PROPERTYNAME ## Changed);

#define DefinePropertyEnum(PROPERTYTYPE, PROPERTYNAME, ENUM) \
        DefineProperty(PROPERTYTYPE, PROPERTYNAME)\
        SetPropertyEnum(prop_ ## PROPERTYNAME, EnumOf_ ## ENUM ());

#define DefinePropertyReactiveEnum(PROPERTYTYPE, PROPERTYNAME, ENUM) \
        DefinePropertyReactive(PROPERTYTYPE, PROPERTYNAME)\
        SetPropertyEnum(prop_ ## PROPERTYNAME, EnumOf_ ## ENUM ());

#endif //PLAZA_PROPERTY_H
