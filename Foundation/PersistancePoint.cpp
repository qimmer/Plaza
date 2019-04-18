
#include "PersistancePoint.h"
#include "Stream.h"
#include "Task.h"
#include "Invocation.h"
#include "FoundationModule.h"

#include <Core/Binding.h>
#include <Core/Instance.h>

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <Core/Debug.h>
#include <Core/Identification.h>

static void Load(Entity persistancePoint) {
    auto streamData = GetStream(persistancePoint);
    auto persistancePointData = GetPersistancePoint(persistancePoint);

    auto fileType = streamData.StreamFileType;
    if(!IsEntityValid(fileType)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. File type is not found for '%s'.", streamData.StreamResolvedPath);

        persistancePointData.PersistancePointLoading = false;
        SetPersistancePoint(persistancePoint, persistancePointData);
        return;
    }

    auto mimeType = GetFileType(fileType).FileTypeMimeType;
    if(strlen(mimeType) == 0) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. Unable to determine mime type for '%s'.", streamData.StreamResolvedPath);

        persistancePointData.PersistancePointLoading = false;
        SetPersistancePoint(persistancePoint, persistancePointData);
        return;
    }

    auto path = streamData.StreamPath;

    // Find serializer
    Entity serializer = 0;
    Serializer serializerData;
    for_entity_data(serializerEntity, ComponentOf_Serializer(), &serializerData) {
        if(mimeType == serializerData.SerializerMimeType) {
            serializer = serializerEntity;
            break;
        }
    }

    if(!IsEntityValid(serializer)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. No compatible serializer for mime type '%s' found when deserializing '%s'.", mimeType, streamData.StreamResolvedPath);

        persistancePointData.PersistancePointLoading = false;
        SetPersistancePoint(persistancePoint, persistancePointData);
        return;
    }
    if(!StreamOpen(persistancePoint, StreamMode_Read)) {
        Log(persistancePoint, LogSeverity_Error, "Load failed. Could not open '%s' for reading.", streamData.StreamResolvedPath);

        persistancePointData.PersistancePointLoading = false;
        SetPersistancePoint(persistancePoint, persistancePointData);
        return;
    }

    auto result = serializerData.DeserializeHandler(persistancePoint);

    if(result) {
        persistancePointData.PersistancePointLoaded = true;
    }

    persistancePointData.PersistancePointLoading = false;
    SetPersistancePoint(persistancePoint, persistancePointData);

    u32 numLoading = 0;
    for_entity_data(persistancePoint, ComponentOf_PersistancePoint(), &persistancePointData) {
        if(persistancePointData.PersistancePointLoading) {
            numLoading++;
        }
    }

    if(numLoading == 0) {
        UnresolvedReference unresolvedReferenceData;
        for_entity_data(unresolvedReference, ComponentOf_UnresolvedReference(), &unresolvedReferenceData) {
            Error(unresolvedReference, "Property '%s' of entity '%s' has an unresolved uuid '%s'.",
                  GetIdentification(unresolvedReferenceData.UnresolvedReferenceProperty).Uuid,
                  GetIdentification(GetOwnership(unresolvedReference).Owner).Uuid,
                  unresolvedReferenceData.UnresolvedReferenceUuid);
        }
    }

    StreamClose(persistancePoint);
}

static void OnPersistancePointChanged(Entity persistancePoint, const PersistancePoint& oldValue, const PersistancePoint& newValue) {
    auto persistancePointData = newValue;

    if(!oldValue.PersistancePointLoading && newValue.PersistancePointLoading) {
        if(newValue.PersistancePointAsync) {
            /*char taskName[64];
            snprintf(taskName, 64, "LoadTask_%llu", persistancePoint);

            auto task = CreateEntity();
            auto functionInvocationData = GetFunctionInvocation(task);
            functionInvocationData.InvocationFunction = FunctionOf_Load();

            auto argument = CreateEntity();

            SetFunctionInvocation(task, functionInvocationData);

            AddQueuedTasks(ModuleOf_Foundation(), task);
            TaskSchedule(task);*/
        } else {
            Load(persistancePoint);
        }
    }

    if(!oldValue.PersistancePointSaving && newValue.PersistancePointSaving) {
        auto streamData = GetStream(persistancePoint);

        auto fileType = streamData.StreamFileType;
        if(!IsEntityValid(fileType)) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. File type is not found for '%s'.", streamData.StreamResolvedPath);

            persistancePointData.PersistancePointSaving = false;
            SetPersistancePoint(persistancePoint, persistancePointData);
            return;
        }

        auto mimeType = GetFileType(fileType).FileTypeMimeType;
        if(strlen(mimeType) == 0) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. Unable to determine mime type for '%s'.", streamData.StreamResolvedPath);

            persistancePointData.PersistancePointSaving = false;
            SetPersistancePoint(persistancePoint, persistancePointData);
            return;
        }

        auto path = streamData.StreamPath;

        // Find serializer
        Entity serializer = 0;
        Serializer serializerData;
        for_entity_data(serializerEntity, ComponentOf_Serializer(), &serializerData) {
            if(mimeType == serializerData.SerializerMimeType) {
                serializer = serializerEntity;
                break;
            }
        }

        if(!IsEntityValid(serializer)) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. No compatible serializer for mime type '%s' found when serializing '%s'.", mimeType, streamData.StreamResolvedPath);
            persistancePointData.PersistancePointSaving = false;
            SetPersistancePoint(persistancePoint, persistancePointData);
            return;
        }

        if(!StreamOpen(persistancePoint, StreamMode_Write)) {
            Log(persistancePoint, LogSeverity_Error, "Save failed. Could not open '%s' for writing", streamData.StreamResolvedPath);
            persistancePointData.PersistancePointSaving = false;
            SetPersistancePoint(persistancePoint, persistancePointData);
            return;
        }

        if(!GetSerializer(serializer).SerializeHandler(persistancePoint)) {
            Log(persistancePoint, LogSeverity_Error, "Serialization failed on trying to save '%s'.", streamData.StreamResolvedPath);
        }

        StreamClose(persistancePoint);

        persistancePointData.PersistancePointSaving = false;
        SetPersistancePoint(persistancePoint, persistancePointData);
    }

    if(!oldValue.PersistancePointLoaded && newValue.PersistancePointLoaded && !newValue.PersistancePointLoading) {
        persistancePointData.PersistancePointLoading = true;
        persistancePointData.PersistancePointLoaded = false;
        SetPersistancePoint(persistancePoint, persistancePointData);
    }
}

BeginUnit(PersistancePoint)
    BeginComponent(PersistancePoint)
        RegisterProperty(bool, PersistancePointAsync)
        RegisterProperty(bool, PersistancePointLoading)
        RegisterProperty(bool, PersistancePointLoaded)
        RegisterProperty(bool, PersistancePointSaving)

        RegisterBase(Stream)
    EndComponent()

    RegisterSystem(OnPersistancePointChanged, ComponentOf_PersistancePoint())
EndUnit()
