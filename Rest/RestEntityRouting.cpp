//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Json/JsonPersistance.h>
#include <Core/Identification.h>
#include <Core/Debug.h>
#include "RestEntityRouting.h"

#define Verbose_Rest "rest"

struct RestEntityRouting {
    Entity RestEntityRoutingJsonSettings;
};

static void ParseQuery(Entity jsonSettings, Entity request) {
    auto requestData = GetHttpRequest(request);
    for(auto queryParam : requestData.HttpRequestParameters) {
        auto propertyUuid = StringFormatV("Property.%s", GetHttpParameter(queryParam).HttpParameterName);
        auto property = FindEntityByUuid(propertyUuid);

        if(!property) {
            Log(request, LogSeverity_Warning, "Unknown JSON settings property when trying to parse query parameter: %s", propertyUuid);
        }

        auto value = GetHttpParameter(queryParam).HttpParameterValue;

        SetPropertyValue(property, jsonSettings, MakeVariant(StringRef, value));
    }
}

API_EXPORT Entity EntityRouteGet(Entity route, Entity request, Entity response, StringRef uuid) {
    auto responseStream = GetHttpResponse(response).HttpResponseContentStream;
    auto entityRoutingData = GetRestEntityRouting(route);

    ParseQuery(entityRoutingData.RestEntityRoutingJsonSettings, request);

    auto requestedEntity = FindEntityByUuid(uuid);

	if (!IsEntityValid(requestedEntity)) {
        requestedEntity = strtoull(uuid, NULL, 10);
	    if(!IsEntityValid(requestedEntity)) {
            Verbose(Verbose_Rest, "GET: Entity not found: %s", uuid);
            return FindResponseCode(404);
	    }
	}

	auto streamData = GetStream(responseStream);
    streamData.StreamPath = "memory://response.json";
    SetStream(responseStream, streamData);

    if(!SerializeJson(responseStream, requestedEntity, entityRoutingData.RestEntityRoutingJsonSettings)) {
		Log(request, LogSeverity_Error, "GET: Error serializing %s", uuid);

        return FindResponseCode(500);
    }

    return FindResponseCode(200);
}
API_EXPORT Entity EntityRoutePut(Entity route, Entity request, Entity response, StringRef uuid) {
    auto requestStream = GetHttpRequest(request).HttpRequestContentStream;

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
API_EXPORT Entity EntityRoutePost(Entity route, Entity request, Entity response, StringRef uuid, StringRef propertyName) {
    auto parent = FindEntityByUuid(uuid);
    auto entityRoutingData = GetRestEntityRouting(route);

    if(!IsEntityValid(parent)) {
        parent = strtoull(uuid, NULL, 10);
        if(!IsEntityValid(parent)) {
            Verbose(Verbose_Rest, "POST: Entity not found: %s", uuid);
            return FindResponseCode(404);
        }
    }

    auto property = FindEntityByUuid(StringFormatV("Property.%s", propertyName));
    if(!IsEntityValid(property)) return FindResponseCode(404);

    if(GetProperty(property).PropertyType != TypeOf_ChildArray) {
        return FindResponseCode(404);
    }

    auto newEntity = CreateEntity();
    auto arr = GetPropertyValue(property, parent);
    arr.as_ChildArray.Add(newEntity);
    SetPropertyValue(property, parent, arr);

    auto requestStream = GetHttpRequest(request).HttpRequestContentStream;
    auto responseStream = GetHttpResponse(response).HttpResponseContentStream;

    if(HasComponent(requestStream, ComponentOf_Stream())) {
        auto fileType = GetStream(requestStream).StreamFileType;
        if(IsEntityValid(fileType) && strcmp(GetFileType(fileType).FileTypeMimeType, "application/json") == 0) {
            DeserializeJson(requestStream, newEntity);
        }
    }

    auto streamData = GetStream(responseStream);
    streamData.StreamPath = "memory://response.json";
    SetStream(responseStream, streamData);

    if(!SerializeJson(responseStream, newEntity, entityRoutingData.RestEntityRoutingJsonSettings)) {
        return FindResponseCode(500);
    }

    return FindResponseCode(201);
}
API_EXPORT Entity EntityRouteDelete(Entity route, Entity request, Entity response, StringRef uuid) {
    auto existing = FindEntityByUuid(uuid);

	if (!IsEntityValid(existing)) {
        existing = strtoull(uuid, NULL, 10);
        if(!IsEntityValid(existing)) {
            Verbose(Verbose_Rest, "DELETE: Entity not found: %s", uuid);
            return FindResponseCode(404);
        }
	}

	auto parent = GetOwnership(existing).Owner;
	auto property = GetOwnership(existing).OwnerProperty;

    if(!IsEntityValid(parent) || !IsEntityValid(property) || GetProperty(property).PropertyType != TypeOf_ChildArray) {
		Log(request, LogSeverity_Error, "DELETE: Entity is not a removable array element: %s", uuid);
        return FindResponseCode(403);
    }

    auto arr = GetPropertyValue(property, parent);
    for(auto i = 0; i < arr.as_ChildArray.GetSize(); ++i) {
        if(arr.as_ChildArray[i] == existing) {
            arr.as_ChildArray.Remove(i);
            SetPropertyValue(property, parent, arr);

            return FindResponseCode(200);
        }
    }

    return FindResponseCode(500);
}

Prefab(RestEntityRoutingJsonSettings)

BeginUnit(RestEntityRouting)
    BeginPrefab(RestEntityRoutingJsonSettings)
        SetJsonSettings(prefab, {

        });
    EndPrefab()

    BeginComponent(RestEntityRouting)
        BeginChildProperty(RestEntityRoutingJsonSettings)
            BeginChildComponent(JsonSettings)
                data.JsonSettingsMaxRecursiveLevels = 3;
            EndChildComponent()
        EndChildProperty()
    EndComponent()

    RegisterFunction(EntityRouteGet)
    RegisterFunction(EntityRoutePut)
    RegisterFunction(EntityRoutePost)
    RegisterFunction(EntityRouteDelete)
EndUnit()