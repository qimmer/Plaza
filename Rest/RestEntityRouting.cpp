//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Json/JsonPersistance.h>
#include <Core/Identification.h>
#include <Core/Debug.h>
#include "RestEntityRouting.h"
#include "RestRouting.h"

struct RestEntityRouting {
    Entity RestEntityRoutingRoot;
};

static Entity handleGet(StringRef entityPath, Entity request, Entity response) {
    auto responseStream = GetHttpResponseContentStream(response);

    auto requestedEntity = FindEntityByPath(entityPath);

	if (!IsEntityValid(requestedEntity)) {
		Log(request, LogSeverity_Error, "GET: Entity not found: %s", entityPath);
		return FindResponseCode(404);
	}

    SetStreamPath(responseStream, "memory://response.json");

    if(!SerializeJson(responseStream, requestedEntity)) {
		Log(request, LogSeverity_Error, "GET: Error serializing %s", entityPath);

        return FindResponseCode(500);
    }

    return FindResponseCode(200);
}

static Entity handlePut(StringRef entityPath, Entity request, Entity response) {
    auto requestStream = GetHttpRequestContentStream(request);

    auto requestedEntity = FindEntityByPath(entityPath);

	if (!IsEntityValid(requestedEntity)) {
		Log(request, LogSeverity_Error, "PUT: Entity not found: %s", entityPath);
		return FindResponseCode(404);
	}

    if(!DeserializeJson(requestStream, requestedEntity)) {
		Log(request, LogSeverity_Error, "PUT: Error serializing %s", entityPath);
        return FindResponseCode(500);
    }

    return FindResponseCode(200);
}

static Entity handlePost(StringRef entityPath, Entity request, Entity response) {
    auto existing = FindEntityByPath(entityPath);

    if(IsEntityValid(existing)) {
		Log(request, LogSeverity_Error, "POST: Entity already exists: %s", entityPath);
        return FindResponseCode(409); // Conflict
    }

    char parentPath[PathMax];
    char propertyName[PathMax];
    char childName[PathMax];

    if(!GetParentPath(entityPath, PathMax, propertyName)) return FindResponseCode(404);
    if(!GetParentPath(propertyName, PathMax, parentPath)) return FindResponseCode(404);
    strcpy(childName, entityPath + strlen(propertyName) + 1);

    // Remove parent path from property path so the property name remains
    memmove(propertyName, propertyName + strlen(parentPath) + 1, strlen(entityPath) - strlen(parentPath) + 1);

    auto parent = FindEntityByPath(parentPath);
    if(!IsEntityValid(parent)) return FindResponseCode(404);

    auto property = FindEntityByName(ComponentOf_Property(), propertyName);
    if(!IsEntityValid(property)) return FindResponseCode(404);

    if(GetPropertyKind(property) != PropertyKind_Array) {
        return FindResponseCode(404);
    }

    u32 newIndex = AddArrayPropertyElement(property, parent);
    auto newEntity = GetArrayPropertyElement(property, parent, newIndex);
    SetName(newEntity, childName);

    auto requestStream = GetHttpRequestContentStream(request);
    auto responseStream = GetHttpResponseContentStream(response);

    if(HasComponent(requestStream, ComponentOf_Stream())) {
        auto fileType = GetStreamFileType(requestStream);
        if(IsEntityValid(fileType) && strcmp(GetFileTypeMimeType(fileType), "application/json") == 0) {
            DeserializeJson(requestStream, newEntity);
        }
    }

    SetStreamPath(responseStream, "memory://response.json");

    if(!SerializeJson(responseStream, newEntity, 100, 0)) {
        return FindResponseCode(500);
    }

    return FindResponseCode(201);
}

static Entity handleDelete(StringRef entityPath, Entity request, Entity response) {
    auto existing = FindEntityByPath(entityPath);

	if (!IsEntityValid(existing)) {
		Log(request, LogSeverity_Error, "DELETE: Entity not found: %s", entityPath);
		return FindResponseCode(404);
	}

    char parentPath[PathMax];
    char propertyName[PathMax];
    char childName[PathMax];

    if(!GetParentPath(entityPath, PathMax, propertyName)) return FindResponseCode(404);
    if(!GetParentPath(propertyName, PathMax, parentPath)) return FindResponseCode(404);

    // Remove parent path from property path so the property name remains
    memmove(propertyName, propertyName + strlen(parentPath) + 1, strlen(entityPath) - strlen(parentPath) + 1);

    auto parent = FindEntityByPath(parentPath);
    if(!IsEntityValid(parent)) return FindResponseCode(500);

    auto property = FindEntityByName(ComponentOf_Property(), propertyName);
    if(!IsEntityValid(property)) return FindResponseCode(500);

    if(GetPropertyKind(property) != PropertyKind_Array) {
		Log(request, LogSeverity_Error, "DELETE: Entity is not a removable array element: %s", entityPath);
        return FindResponseCode(403);
    }

    auto children = GetArrayPropertyElements(property, parent);
    auto numChildren = GetArrayPropertyCount(property, parent);
    for(auto i = 0; i < numChildren; ++i) {
        if(children[i] == existing) {
			if (!RemoveArrayPropertyElement(property, parent, i)) {
				return FindResponseCode(500);
			}

            return FindResponseCode(200);
        }
    }

    return FindResponseCode(500);
}

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request, Entity response) {
    auto data = GetRestEntityRoutingData(routing);

    if(data && IsEntityValid(data->RestEntityRoutingRoot)) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));

        char entityPath[PathMax];
        CalculateEntityPath(entityPath, PathMax, data->RestEntityRoutingRoot);

        snprintf(completeRoute, 1024, "%s%s", entityPath, relativeUrl);

        auto method = GetHttpRequestMethod(request);
        auto responseCode = FindResponseCode(500);
		if (strcmp(method, "GET") == 0) {
			responseCode = handleGet(completeRoute, request, response);
		} else if (strcmp(method, "PUT") == 0) {
			responseCode = handlePut(completeRoute, request, response);
		} else if (strcmp(method, "POST") == 0) {
			responseCode = handlePost(completeRoute, request, response);
		} else if (strcmp(method, "DELETE") == 0) {
			responseCode = handleDelete(completeRoute, request, response);
		} else {
			responseCode = FindResponseCode(405);
		}

        SetHttpResponseCode(response, responseCode);
    }
}

BeginUnit(RestEntityRouting)
    BeginComponent(RestEntityRouting)
        RegisterBase(RestRouting)
        RegisterProperty(Entity, RestEntityRoutingRoot)
    EndComponent()

    RegisterSubscription(RestRoutingRequest, OnRestRoutingRequest, 0)
EndUnit()