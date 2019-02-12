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

#define Verbose_Rest "rest"

struct RestEntityRouting {
    Entity RestEntityRoutingRoot;
    u8 RestEntityRoutingDepth;
};

static Entity handleGet(StringRef uuid, Entity request, Entity response, u8 depth) {
    auto responseStream = GetHttpResponseContentStream(response);

    auto requestedEntity = FindEntityByUuid(uuid);

	if (!IsEntityValid(requestedEntity)) {
        requestedEntity = strtoull(uuid, NULL, 10);
	    if(!IsEntityValid(requestedEntity)) {
            Verbose(Verbose_Rest, "GET: Entity not found: %s", uuid);
            return FindResponseCode(404);
	    }
	}

    SetStreamPath(responseStream, "memory://response.json");

    if(!SerializeJson(responseStream, requestedEntity, depth, 0)) {
		Log(request, LogSeverity_Error, "GET: Error serializing %s", uuid);

        return FindResponseCode(500);
    }

    return FindResponseCode(200);
}

static Entity handlePut(StringRef uuid, Entity request, Entity response) {
    auto requestStream = GetHttpRequestContentStream(request);

    auto requestedEntity = FindEntityByUuid(uuid);

	if (!IsEntityValid(requestedEntity)) {
        requestedEntity = strtoull(uuid, NULL, 10);
        if(!IsEntityValid(requestedEntity)) {
            Verbose(Verbose_Rest, "PUT: Entity not found: %s", uuid);
            return FindResponseCode(404);
        }
	}

    if(!DeserializeJson(requestStream, requestedEntity)) {
		Log(request, LogSeverity_Error, "PUT: Error serializing %s", uuid);
        return FindResponseCode(500);
    }

    return FindResponseCode(200);
}

static Entity handlePost(StringRef uuid, StringRef propertyName, Entity request, Entity response) {
    auto parent = FindEntityByUuid(uuid);

    if(!IsEntityValid(parent)) {
        parent = strtoull(uuid, NULL, 10);
        if(!IsEntityValid(parent)) {
            Verbose(Verbose_Rest, "POST: Entity not found: %s", uuid);
            return FindResponseCode(404);
        }
    }

    auto property = FindEntityByName(ComponentOf_Property(), propertyName);
    if(!IsEntityValid(property)) return FindResponseCode(404);

    if(GetPropertyKind(property) != PropertyKind_Array) {
        return FindResponseCode(404);
    }

    u32 newIndex = AddArrayPropertyElement(property, parent);
    auto newEntity = GetArrayPropertyElement(property, parent, newIndex);

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

static Entity handleDelete(StringRef uuid, Entity request, Entity response) {
    auto existing = FindEntityByUuid(uuid);

	if (!IsEntityValid(existing)) {
        existing = strtoull(uuid, NULL, 10);
        if(!IsEntityValid(existing)) {
            Verbose(Verbose_Rest, "DELETE: Entity not found: %s", uuid);
            return FindResponseCode(404);
        }
	}

	auto parent = GetOwner(existing);
	auto property = GetOwnerProperty(existing);

    if(!IsEntityValid(parent) || !IsEntityValid(property) || GetPropertyKind(property) != PropertyKind_Array) {
		Log(request, LogSeverity_Error, "DELETE: Entity is not a removable array element: %s", uuid);
        return FindResponseCode(403);
    }

    u32 numChildren = 0;
    auto children = GetArrayPropertyElements(property, parent, &numChildren);
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
        char relativeUrl[1024];
        strcpy(relativeUrl, requestUrl + strlen(GetRestRoutingRoute(routing)) + 1);

        auto uuid = relativeUrl;
        auto propertyName = "";

        auto slashLoc = strchr(relativeUrl, '/');
        if(slashLoc) {
            *slashLoc = '\0';
            propertyName = slashLoc + 1;
        }

        auto method = GetHttpRequestMethod(request);
        auto responseCode = FindResponseCode(500);
		if (strcmp(method, "GET") == 0) {
			responseCode = handleGet(uuid, request, response, GetRestEntityRoutingDepth(routing));
		} else if (strcmp(method, "PUT") == 0) {
			responseCode = handlePut(uuid, request, response);
		} else if (strcmp(method, "POST") == 0) {
			responseCode = handlePost(uuid, propertyName, request, response);
		} else if (strcmp(method, "DELETE") == 0) {
			responseCode = handleDelete(uuid, request, response);
		} else {
			responseCode = FindResponseCode(405);
		}

        SetHttpResponseCode(response, responseCode);
    }
}

BeginUnit(RestEntityRouting)
    BeginComponent(RestEntityRouting)
        RegisterBase(RestRouting)
        RegisterReferenceProperty(Ownership, RestEntityRoutingRoot)
        RegisterProperty(u8, RestEntityRoutingDepth)
    EndComponent()

    RegisterSubscription(EventOf_RestRoutingRequest(), OnRestRoutingRequest, 0)
EndUnit()