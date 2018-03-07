
#include <Core/Dictionary.h>
#include <Core/String.h>
#include "PersistancePoint.h"
#include "Persistance.h"
#include "Stream.h"

struct PersistancePoint {
    bool Loaded, IsLoading, IsSaving;
};

static Dictionary<String, Serializer> Serializers;

DefineComponent(PersistancePoint)
    Dependency(Stream)

    DefineMethod(void, Load, Entity entity)
    DefineMethod(void, Unload, Entity entity)
    DefineMethod(void, Save, Entity entity)
EndComponent()

DefineService(PersistancePoint)
EndService()

DefineComponentProperty(PersistancePoint, bool, Loaded)

DefineEvent(LoadStarted, EntityHandler)
DefineEvent(SaveStarted, EntityHandler)
DefineEvent(UnloadFinished, EntityHandler)
DefineEvent(LoadFinished, EntityHandler)
DefineEvent(SaveFinished, EntityHandler)

void Unload(Entity persistancePoint) {
    for(auto entity = GetNextEntityThat(0, HasPersistance); IsEntityValid(entity); entity = GetNextEntityThat(entity, HasPersistance)) {
        if(GetEntityPersistancePoint(entity) == persistancePoint) {
            DestroyEntity(entity);
        }
    }
    GetPersistancePoint(persistancePoint)->Loaded = false;
}

static void LoadedChanged(Entity persistancePoint, bool oldValue, bool newValue) {
    if(oldValue != newValue && newValue) {
        Load(persistancePoint);
    }

    if(oldValue != newValue && !newValue) {
        Unload(persistancePoint);
    }
}

void Load(Entity persistancePoint) {
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
    FireEvent(LoadStarted, persistancePoint);
    if(!serializerIt->second.DeserializeHandler(persistancePoint)) {
        SetLoaded(persistancePoint, false);
    }
    GetPersistancePoint(persistancePoint)->IsLoading = false;
    FireEvent(LoadFinished, persistancePoint);
}

void Save(Entity persistancePoint) {
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
    FireEvent(SaveStarted, persistancePoint);

    serializerIt->second.SerializeHandler(persistancePoint);

    GetPersistancePoint(persistancePoint)->IsSaving = false;
    FireEvent(SaveFinished, persistancePoint);
}

bool IsLoading(Entity persistancePoint) {
    return GetPersistancePoint(persistancePoint)->IsLoading;
}

void AddSerializer(StringRef mimeType, struct Serializer *serializer) {
    Serializers[mimeType] = *serializer;
}

void RemoveSerializer(StringRef mimeType) {
    Serializers.erase(mimeType);
}


static bool ServiceStart() {
    SubscribeLoadedChanged(LoadedChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeLoadedChanged(LoadedChanged);
    return true;
}
