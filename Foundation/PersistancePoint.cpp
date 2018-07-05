
#include <Core/Dictionary.h>
#include <Core/String.h>
#include "PersistancePoint.h"
#include "Persistance.h"
#include "Stream.h"

struct PersistancePoint {
    bool Loaded, IsLoading, IsSaving;
};

static Dictionary<Serializer> Serializers;

DefineComponent(PersistancePoint)
    Dependency(Stream)

    DefineMethod(void, Load, Entity entity)
    DefineMethod(void, Unload, Entity entity)
    DefineMethod(void, Save, Entity entity)
EndComponent()

DefineComponentPropertyReactive(PersistancePoint, bool, Loaded)

DefineEvent(LoadStarted)
DefineEvent(SaveStarted)
DefineEvent(UnloadFinished)
DefineEvent(LoadFinished)
DefineEvent(SaveFinished)

void Unload(Entity persistancePoint) {
    for_entity(entity, Persistance) {
        if(GetEntityPersistancePoint(entity) == persistancePoint) {
            DestroyEntity(entity);
        }
    }
    GetPersistancePoint(persistancePoint)->Loaded = false;
}

static void OnPersistancePointAdded(Entity persistancePoint) {
    // If children exist already, they are referenced to stores entities at this persistance point.
    // Therefore load these to keep them up to date with the ones on disk.
    if(IsEntityValid(GetFirstChild(persistancePoint))) {
        Load(persistancePoint);
    }
}

static void OnStreamChanged(Entity persistancePoint) {
    if(HasPersistancePoint(persistancePoint) && IsEntityValid(GetFirstChild(persistancePoint)) && GetLoaded(persistancePoint)) {
        Load(persistancePoint);
    }
}

static void LoadedChanged(Entity persistancePoint, bool oldValue, bool newValue) {
    if(oldValue != newValue && newValue) {
        Load(persistancePoint);
    }

    if(oldValue != newValue && !newValue) {
        Unload(persistancePoint);
    }
}

static void OnParentChanged(Entity entity, Entity oldParent, Entity newParent) {
    if(IsEntityValid(newParent) && HasPersistancePoint(newParent)) {
        auto data = GetPersistancePoint(newParent);
        if(!data->Loaded && !data->IsLoading) Load(newParent);
    }
}

API_EXPORT void Load(Entity persistancePoint) {
    auto mimeType = GetStreamMimeType(persistancePoint);
    if(strlen(mimeType) == 0) {
        Log(LogChannel_Core, LogSeverity_Error, "Load failed. Unable to determine serializer, as content mime type is not given for '%s'.", GetStreamResolvedPath(persistancePoint));
        SetLoaded(persistancePoint, false);
        return;
    }
    auto serializerIt = Serializers.find(mimeType);

    if(serializerIt == Serializers.end()) {
        Log(LogChannel_Core, LogSeverity_Error, "Load failed. Mime type '%s' is unknown, which is required by '%s'.", mimeType, GetStreamResolvedPath(persistancePoint));
        SetLoaded(persistancePoint, false);
        return;
    }

    if(!StreamOpen(persistancePoint, StreamMode_Read)) {
        Log(LogChannel_Core, LogSeverity_Error, "Load failed. Could not open '%s' for reading", GetStreamResolvedPath(persistancePoint));
        SetLoaded(persistancePoint, false);
        return;
    }

    GetPersistancePoint(persistancePoint)->Loaded = true;

    GetPersistancePoint(persistancePoint)->IsLoading = true;
    FireNativeEvent(LoadStarted, persistancePoint);
    if(!serializerIt->second.DeserializeHandler(persistancePoint)) {
        SetLoaded(persistancePoint, false);
    }
    GetPersistancePoint(persistancePoint)->IsLoading = false;
    FireNativeEvent(LoadFinished, persistancePoint);
}

API_EXPORT void Save(Entity persistancePoint) {
    auto mimeType = GetStreamMimeType(persistancePoint);
    if(strlen(mimeType) == 0) {
        Log(LogChannel_Core, LogSeverity_Error, "Save failed. Unable to determine serializer, as content mime type is not given for '%s'.", GetStreamResolvedPath(persistancePoint));
        return;
    }
    auto serializerIt = Serializers.find(mimeType);

    if(serializerIt == Serializers.end()) {
        Log(LogChannel_Core, LogSeverity_Error, "Save failed. Mime type '%s' is unknown, which is required by '%s'.", mimeType, GetStreamResolvedPath(persistancePoint));
        return;
    }

    if(!StreamOpen(persistancePoint, StreamMode_Write)) {
        Log(LogChannel_Core, LogSeverity_Error, "Save failed. Could not open '%s' for writing", GetStreamResolvedPath(persistancePoint));
        return;
    }

    GetPersistancePoint(persistancePoint)->IsSaving = true;
    FireNativeEvent(SaveStarted, persistancePoint);

    serializerIt->second.SerializeHandler(persistancePoint);

    GetPersistancePoint(persistancePoint)->IsSaving = false;
    FireNativeEvent(SaveFinished, persistancePoint);
}

API_EXPORT bool IsLoading(Entity persistancePoint) {
    return GetPersistancePoint(persistancePoint)->IsLoading;
}

API_EXPORT void AddSerializer(StringRef mimeType, struct Serializer *serializer) {
    Serializers[mimeType] = *serializer;
}

API_EXPORT void RemoveSerializer(StringRef mimeType) {
    Serializers.erase(mimeType);
}

DefineService(PersistancePoint)
        Subscribe(PersistancePointAdded, OnPersistancePointAdded)
        Subscribe(StreamContentChanged, OnStreamChanged)
        Subscribe(StreamChanged, OnStreamChanged)
        Subscribe(LoadedChanged, LoadedChanged)
        Subscribe(ParentChanged, OnParentChanged)
EndService()
