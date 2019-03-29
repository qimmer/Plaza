//
// Created by Kim on 18-07-2018.
//

#include <Core/NativeUtils.h>
#include "Identification.h"
#include <Core/Entity.h>

#include "Component.h"
#include "Debug.h"
#include "Property.h"
#include "Vector.h"
#include "Algorithms.h"
#include "Hashing.h"
#include "Std.h"
#include "Strings.h"

#include <climits>
#include <map>
#include <EASTL/unordered_map.h>
#include <EASTL/fixed_string.h>

#ifdef WIN32
#undef Enum
#include <combaseapi.h>
#else
#include <uuid/uuid.h>
#endif

static std::map<const void*, Entity> uuidTable;

API_EXPORT Entity FindEntityByUuid(StringRef uuid) {
    if(!uuid || strlen(uuid) == 0) {
        return 0;
    }

    uuid = Intern(uuid);

    auto it = uuidTable.find(uuid);
    if(it == uuidTable.end()) {
        return 0;
    }

    return it->second;
}

static void OnIdentificationChanged(Entity entity, const Identification& value, const Identification& oldValue) {
    if(oldValue.Uuid) {
        auto oldIt = uuidTable.find(oldValue.Uuid);
        if(oldIt != uuidTable.end()) {
            uuidTable.erase(oldIt);
        }
    }

    if(value.Uuid) {
        auto newUuidIt = uuidTable.find(value.Uuid);
        if(newUuidIt != uuidTable.end() && newUuidIt->second != entity) {
            Log(entity, LogSeverity_Error, "Uuid '%s' is already occupied by '%s'.", value, GetDebugName(newUuidIt->second));
            return;
        }

        uuidTable[value.Uuid] = entity;
    }
}

API_EXPORT bool GetParentPath(StringRef childPath, u32 bufLen, char *parentPath) {
    strncpy(parentPath, childPath, bufLen);

    auto len = strlen(childPath);
    while(len > 0 && parentPath[len] != '/') {
        len--;
    }

    if(parentPath[len] != '/' && len > 0) {
        return false;
    }

    parentPath[len] = '\0';

    return true;
}

API_EXPORT StringRef CreateGuid() {
    char buf[128];
    memset(buf, 0, sizeof(buf));

#ifdef WIN32
    GUID guid;
    CoCreateGuid(&guid);

    snprintf(buf, 128, "%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
           guid.Data1, guid.Data2, guid.Data3,
           guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
           guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
#endif

    return Intern(buf);
}

API_EXPORT StringRef CreateGuidFromPath(StringRef path) {

    return nullptr;
}

BeginUnit(Identification)
    BeginComponent(Identification)
        RegisterProperty(StringRef, Uuid)
    EndComponent()

    RegisterSystem(OnIdentificationChanged, ComponentOf_Identification())
EndUnit()
