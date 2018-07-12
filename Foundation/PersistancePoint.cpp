
#include "PersistancePoint.h"
#include "Stream.h"
#include "Task.h"
#include "Invocation.h"

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <Core/Debug.h>

using namespace eastl;

struct PersistancePoint {
    Entity PersistancePointSerializer;
    bool PersistancePointLoaded, PersistancePointLoading, PersistancePointSaving, PersistancePointAsync;
};

LocalFunction(OnPersistancePointAdded, void, Entity persistancePoint) {
    // If children exist already, they are referenced to stores entities at this persistance point.
    // Therefore load these to keep them up to date with the ones on disk.
    if(IsEntityValid(GetFirstChild(persistancePoint))) {
        SetPersistancePointLoaded(persistancePoint, true);
    }
}

LocalFunction(OnStreamContentChanged, void, Entity persistancePoint) {
    // If serialized content has changed, re-deserialize (load) it!
    if(HasComponent(persistancePoint, ComponentOf_PersistancePoint()) && IsEntityValid(GetFirstChild(persistancePoint)) && GetPersistancePointLoaded(persistancePoint)) {
        SetPersistancePointLoaded(persistancePoint, false);
        SetPersistancePointLoaded(persistancePoint, true);
    }
}

LocalFunction(OnStreamPathChanged, void, Entity persistancePoint, StringRef oldValue, StringRef newValue) {
    OnStreamContentChanged(persistancePoint);
}

LocalFunction(Load, bool, Entity persistancePoint) {
    if(!StreamOpen(persistancePoint, StreamMode_Read)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. Could not open '%s' for reading.", GetStreamResolvedPath(persistancePoint));
        SetPersistancePointLoaded(persistancePoint, false);
        return false;
    }

    auto serializer = GetPersistancePointSerializer(persistancePoint);
    auto result = GetSerializerData(serializer)->DeserializeHandler(persistancePoint);

    SetPersistancePointLoading(persistancePoint, false);

    return result;
}

LocalFunction(OnPersistancePointLoadedChanged, void, Entity persistancePoint, bool oldValue, bool newValue) {
    if(newValue) {
        // Load
        auto fileType = GetStreamFileType(persistancePoint);
        if(!IsEntityValid(fileType)) {
            Log(persistancePoint, LogSeverity_Error, "Load failed. File type is not found for '%s'.", GetStreamResolvedPath(persistancePoint));
            SetPersistancePointLoaded(persistancePoint, false);
            return;
        }

        auto mimeType = GetFileTypeMimeType(fileType);
        if(strlen(mimeType) == 0) {
            Log(persistancePoint, LogSeverity_Error, "Load failed. Unable to determine serializer, as content mime type is not given for '%s'.", GetStreamResolvedPath(persistancePoint));
            SetPersistancePointLoaded(persistancePoint, false);
            return;
        }

        auto serializer = GetPersistancePointSerializer(persistancePoint);
        if(!IsEntityValid(serializer)) {
            Log(persistancePoint, LogSeverity_Error, "Load failed. No compatible serialiser for mime type '%s' found when deserializing '%s'.", mimeType, GetStreamResolvedPath(persistancePoint));
            SetPersistancePointLoaded(persistancePoint, false);
            return;
        }

        SetPersistancePointLoading(persistancePoint, true);

        if(GetPersistancePointAsync(persistancePoint)) {
            char taskName[64];
            snprintf(taskName, 64, "LoadTask_%llu", persistancePoint);

            auto task = CreateEntityFromName(NodeOf_TaskQueue(), taskName);
            SetInvocationFunction(task, FunctionOf_Load());
            TaskSchedule(task);
        } else {
            Load(persistancePoint);
        }
    } else {
        // Unload
        for_children(child, persistancePoint) {
            DestroyEntity(child);
        }
    }
}

LocalFunction(OnParentChanged, void, Entity entity, Entity oldParent, Entity newParent) {
    if(IsEntityValid(newParent) && HasComponent(newParent, ComponentOf_PersistancePoint())) {
        auto data = GetPersistancePointData(newParent);
        if(!data->PersistancePointLoaded && !data->PersistancePointLoading) Load(newParent);
    }
}

API_EXPORT bool Save(Entity persistancePoint) {
    auto fileType = GetStreamFileType(persistancePoint);
    if(!IsEntityValid(fileType)) {
        Log(persistancePoint, LogSeverity_Error, "Save failed. File type is not found for '%s'.", GetStreamResolvedPath(persistancePoint));
        return false;
    }

    auto mimeType = GetFileTypeMimeType(fileType);
    if(strlen(mimeType) == 0) {
        Log(persistancePoint, LogSeverity_Error, "Save failed. Unable to determine serializer, as content mime type is not given for '%s'.", GetStreamResolvedPath(persistancePoint));
        return false;
    }

    auto serializer = GetPersistancePointSerializer(persistancePoint);
    if(!IsEntityValid(serializer)) {
        Log(persistancePoint, LogSeverity_Error, "Save failed. No compatible serialiser for mime type '%s' found when serializing '%s'.", mimeType, GetStreamResolvedPath(persistancePoint));
        return false;
    }

    if(!StreamOpen(persistancePoint, StreamMode_Write)) {
        Log(persistancePoint, LogSeverity_Error, "Save failed. Could not open '%s' for writing", GetStreamResolvedPath(persistancePoint));
        return false;
    }

    SetPersistancePointSaving(persistancePoint, true);

    auto result = GetSerializerData(serializer)->SerializeHandler(persistancePoint);

    SetPersistancePointSaving(persistancePoint, false);

    return result;
}

BeginUnit(PersistancePoint)
    BeginComponent(PersistancePoint)
        RegisterProperty(bool, PersistancePointAsync)
        RegisterProperty(bool, PersistancePointLoaded)
        RegisterProperty(bool, PersistancePointLoading)
        RegisterProperty(bool, PersistancePointSaving)
        RegisterProperty(Entity, PersistancePointSerializer)

        RegisterBase(Stream)
    EndComponent()

    BeginComponent(Serializer)
        RegisterProperty(StringRef, SerializerMimeType)
    EndComponent()

    RegisterFunction(Save)

    RegisterSubscription(PersistancePointAdded, OnPersistancePointAdded, 0)
    RegisterSubscription(StreamContentChanged, OnStreamContentChanged, 0)
    RegisterSubscription(StreamPathChanged, OnStreamPathChanged, 0)
    RegisterSubscription(PersistancePointLoadedChanged, OnPersistancePointLoadedChanged, 0)
    RegisterSubscription(ParentChanged, OnParentChanged, 0)
EndUnit()
