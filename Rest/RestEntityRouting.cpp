//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Json/JsonPersistance.h>
#include <Core/Identification.h>
#include "RestEntityRouting.h"
#include "RestRouting.h"

struct RestEntityRouting {
    Entity RestEntityRoutingRoot;
};

static u16 handleGet(StringRef entityPath, Entity request) {
    auto requestedEntity = FindEntityByPath(entityPath);

    if(!IsEntityValid(requestedEntity)) return 404;

    auto response = GetHttpRequestResponse(request);
    SetStreamPath(response, "memory://response.json");

    if(!SerializeJson(response, requestedEntity)) {
        return 500;
    }

    return 200;
}

static u16 handlePut(StringRef entityPath, Entity request) {
    auto requestedEntity = FindEntityByPath(entityPath);

    if(!IsEntityValid(requestedEntity)) return 404;

    auto response = GetHttpRequestResponse(request);

    if(!DeserializeJson(request, requestedEntity)) {
        return 500;
    }

    return 200;
}

static u16 handlePost(StringRef entityPath, Entity request) {
    auto existing = FindEntityByPath(entityPath);

    if(IsEntityValid(existing)) {
        return 409; // Conflict
    }

    char parentPath[PathMax];
    char propertyName[PathMax];
    char childName[PathMax];

    if(!GetParentPath(entityPath, PathMax, propertyName)) return 404;
    if(!GetParentPath(propertyName, PathMax, parentPath)) return 404;
    strcpy(childName, entityPath + strlen(propertyName) + 1);

    // Remove parent path from property path so the property name remains
    memmove(propertyName, propertyName + strlen(parentPath), strlen(entityPath) - strlen(parentPath) + 1);

    auto parent = FindEntityByPath(parentPath);
    if(!IsEntityValid(parent)) return 404;

    auto property = FindEntityByName(ComponentOf_Property(), propertyName);
    if(!IsEntityValid(property)) return 404;

    if(GetPropertyKind(property) != PropertyKind_Array) {
        return 404;
    }

    u32 newIndex = AddArrayPropertyElement(property, parent);
    auto newEntity = GetArrayPropertyElement(property, parent, newIndex);
    SetName(newEntity, childName);

    if(!DeserializeJson(request, newEntity)) {
        RemoveArrayPropertyElement(property, parent, newIndex);
        return 500;
    }

    return 201;
}

static u16 handleDelete(StringRef entityPath, Entity request) {
    auto existing = FindEntityByPath(entityPath);

    if(!IsEntityValid(existing)) {
        return 404;
    }

    char parentPath[PathMax];
    char propertyName[PathMax];
    char childName[PathMax];

    if(!GetParentPath(entityPath, PathMax, propertyName)) return 500;
    if(!GetParentPath(parentPath, PathMax, parentPath)) return 500;

    // Remove parent path from property path so the property name remains
    memmove(propertyName, propertyName + strlen(parentPath), strlen(entityPath) - strlen(parentPath) + 1);

    auto parent = FindEntityByPath(parentPath);
    if(!IsEntityValid(parent)) return 500;

    auto property = FindEntityByName(ComponentOf_Property(), propertyName);
    if(!IsEntityValid(property)) return 500;

    if(GetPropertyKind(property) != PropertyKind_Array) {
        return 500;
    }

    auto children = GetArrayPropertyElements(property, parent);
    auto numChildren = GetArrayPropertyCount(property, parent);
    for(auto i = 0; i < numChildren; ++i) {
        if(children[i] == existing) {
            RemoveArrayPropertyElement(property, parent, i);
            return 200;
        }
    }

    return 404;
}

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request) {
    auto data = GetRestEntityRoutingData(routing);

    if(data && IsEntityValid(data->RestEntityRoutingRoot)) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));

        char entityPath[PathMax];
        CalculateEntityPath(entityPath, PathMax, data->RestEntityRoutingRoot);

        snprintf(completeRoute, 1024, "%s%s", entityPath, relativeUrl);

        // 500 if handlers do not set any response code
        SetHttpResponseCode(GetHttpRequestResponse(request), 500);

        auto method = GetHttpRequestMethod(request);
        auto responseCode = 500;
        if(strcmp(method, "GET") == 0) responseCode = handleGet(completeRoute, request);
        else if(strcmp(method, "PUT") == 0) responseCode = handlePut(completeRoute, request);
        else if(strcmp(method, "POST") == 0) responseCode = handlePost(completeRoute, request);
        else if(strcmp(method, "DELETE") == 0) responseCode = handleDelete(completeRoute, request);
        else responseCode = 405;

        SetHttpResponseCode(GetHttpRequestResponse(request), responseCode);
    }
}

BeginUnit(RestEntityRouting)
    BeginComponent(RestEntityRouting)
        RegisterBase(RestRouting)
        RegisterProperty(Entity, RestEntityRoutingRoot)
    EndComponent()

    RegisterSubscription(RestRoutingRequest, OnRestRoutingRequest, 0)
EndUnit()