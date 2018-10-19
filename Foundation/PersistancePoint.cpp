
#include "PersistancePoint.h"
#include "Stream.h"
#include "Task.h"
#include "Invocation.h"
#include "FoundationModule.h"

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <Core/Debug.h>
#include <Core/Identification.h>

using namespace eastl;

struct PersistancePoint {
    bool PersistancePointLoading, PersistancePointSaving, PersistancePointAsync, PersistancePointLoaded;
};

struct UnresolvedReference {
    Entity UnresolvedReferenceProperty;
    StringRef UnresolvedReferenceUuid;
};

struct UnresolvedEntity {
};

LocalFunction(OnStreamContentChanged, void, Entity persistancePoint) {
    // If serialized content has changed, re-deserialize (load) it!
    if(HasComponent(persistancePoint, ComponentOf_PersistancePoint()) && !GetPersistancePointLoading(persistancePoint) && !GetPersistancePointSaving(persistancePoint) && GetPersistancePointLoaded(persistancePoint)) {
        SetPersistancePointLoading(persistancePoint, false);
        SetPersistancePointLoading(persistancePoint, true);
    }
}

LocalFunction(OnStreamPathChanged, void, Entity persistancePoint, StringRef oldValue, StringRef newValue) {
    OnStreamContentChanged(persistancePoint);
}

LocalFunction(Load, void, Entity persistancePoint) {
    auto fileType = GetStreamFileType(persistancePoint);
    if(!IsEntityValid(fileType)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. File type is not found for '%s'.", GetStreamResolvedPath(persistancePoint));
        SetPersistancePointLoading(persistancePoint, false);
        return;
    }

    auto mimeType = GetFileTypeMimeType(fileType);
    if(strlen(mimeType) == 0) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. Unable to determine mime type for '%s'.", GetStreamResolvedPath(persistancePoint));
        SetPersistancePointLoading(persistancePoint, false);
        return;
    }

    auto path = GetStreamPath(persistancePoint);

    // Find serializer
    Entity serializer = 0;
    for_entity(serializerEntity, serializerData, Serializer, {
        if(mimeType == serializerData->SerializerMimeType) {
            serializer = serializerEntity;
            break;
        }
    });

    if(!IsEntityValid(serializer)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. No compatible serializer for mime type '%s' found when deserializing '%s'.", mimeType, GetStreamResolvedPath(persistancePoint));
        SetPersistancePointLoading(persistancePoint, false);
        return;
    }
    if(!StreamOpen(persistancePoint, StreamMode_Read)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. Could not open '%s' for reading.", GetStreamResolvedPath(persistancePoint));
        SetPersistancePointLoading(persistancePoint, false);
        return;
    }

    Log(persistancePoint, LogSeverity_Info, "Loading %s ...", path);

    auto result = GetSerializerData(serializer)->DeserializeHandler(persistancePoint);

    SetPersistancePointLoading(persistancePoint, false);

    if(result) {
        SetPersistancePointLoaded(persistancePoint, true);
    }

    StreamClose(persistancePoint);
}

LocalFunction(OnPersistancePointLoadingChanged, void, Entity persistancePoint, bool oldValue, bool newValue) {
    if(newValue) {
        if(GetPersistancePointAsync(persistancePoint)) {
            char taskName[64];
            snprintf(taskName, 64, "LoadTask_%llu", persistancePoint);

            auto task = AddQueuedTasks(ModuleOf_Foundation());
            SetName(task, taskName);
            SetInvocationFunction(task, FunctionOf_Load());
            TaskSchedule(task);
        } else {
            Load(persistancePoint);
        }
    }
}

LocalFunction(OnPersistancePointSavingChanged, void, Entity persistancePoint, bool oldValue, bool newValue) {
    if(newValue) {
        auto fileType = GetStreamFileType(persistancePoint);
        if(!IsEntityValid(fileType)) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. File type is not found for '%s'.", GetStreamResolvedPath(persistancePoint));
            SetPersistancePointSaving(persistancePoint, false);
            return;
        }

        auto mimeType = GetFileTypeMimeType(fileType);
        if(strlen(mimeType) == 0) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. Unable to determine serializer, as content mime type is not given for '%s'.", GetStreamResolvedPath(persistancePoint));
            SetPersistancePointSaving(persistancePoint, false);
            return;
        }

        Entity serializer = 0;
        for_entity(serializerEntity, serializerData, Serializer, {
            if(mimeType == serializerData->SerializerMimeType) {
                serializer = serializerEntity;
                break;
            }
        });

        if(!IsEntityValid(serializer)) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. No compatible serialiser for mime type '%s' found when serializing '%s'.", mimeType, GetStreamResolvedPath(persistancePoint));
            SetPersistancePointSaving(persistancePoint, false);
            return;
        }

        if(!StreamOpen(persistancePoint, StreamMode_Write)) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. Could not open '%s' for writing", GetStreamResolvedPath(persistancePoint));
            SetPersistancePointSaving(persistancePoint, false);
            return;
        }

        if(!GetSerializerData(serializer)->SerializeHandler(persistancePoint)) {
            Log(persistancePoint, LogSeverity_Error, "Serialization failed on trying to save '%s'.", GetStreamResolvedPath(persistancePoint));
        }

        StreamClose(persistancePoint);

        SetPersistancePointSaving(persistancePoint, false);
    }

}

// Tries to load all persistance point entities in the given path
API_EXPORT bool LoadEntityPath(StringRef entityPath) {
    return false;
}

API_EXPORT bool ResolveReferences() {
    bool areAllResolved = true;
    Entity unresolvedReference = 0;
    UnresolvedReference *data = NULL;

    for_entity(unresolvedReference, data, UnresolvedReference, {
        auto reference = FindEntityByUuid(data->UnresolvedReferenceUuid);
        if(!IsEntityValid(reference)) {
            areAllResolved = false;
            continue;
        }

        auto entity = GetOwner(unresolvedReference);

        SetPropertyValue(data->UnresolvedReferenceProperty, entity, &reference);

        auto index = GetUnresolvedReferencesIndex(entity, unresolvedReference);
        RemoveUnresolvedReferences(entity, index);
    });

    return areAllResolved;
}

BeginUnit(PersistancePoint)
    BeginComponent(PersistancePoint)
        RegisterProperty(bool, PersistancePointAsync)
        RegisterProperty(bool, PersistancePointLoading)
        RegisterProperty(bool, PersistancePointLoaded)
        RegisterProperty(bool, PersistancePointSaving)

        RegisterBase(Stream)
    EndComponent()

    BeginComponent(UnresolvedReference)
        RegisterReferenceProperty(Property, UnresolvedReferenceProperty)
        RegisterProperty(StringRef, UnresolvedReferenceUuid)
    EndComponent()

    BeginComponent(UnresolvedEntity)
        RegisterArrayProperty(UnresolvedReference, UnresolvedReferences)
    EndComponent()

    RegisterSubscription(EventOf_StreamContentChanged(), OnStreamContentChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_StreamPath()), OnStreamPathChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PersistancePointLoading()), OnPersistancePointLoadingChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PersistancePointSaving()), OnPersistancePointSavingChanged, 0)
EndUnit()
