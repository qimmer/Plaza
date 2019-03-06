//
// Created by Kim on 14/07/2018.
//

#ifndef PLAZA_FOUNDATION_NATIVEUTILS_H
#define PLAZA_FOUNDATION_NATIVEUTILS_H

#include <Core/NativeUtils.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>

#define RegisterStreamProtocol(COMPONENT, IDENTIFIER) \
    {\
        auto protocol = GetUniqueEntity("Protocol." #COMPONENT, NULL );\
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        AddChild(PropertyOf_ModuleStreamProtocols(), module, protocol, true); \
        SetStreamProtocolIdentifier(protocol, IDENTIFIER); \
        SetStreamProtocolComponent(protocol, ComponentOf_ ## COMPONENT ()); \
        auto protocolData = GetStreamProtocolData(protocol); \
        Assert(protocol, protocolData);\
        protocolData->StreamSeekHandler = Seek; \
        protocolData->StreamTellHandler = Tell; \
        protocolData->StreamReadHandler = Read; \
        protocolData->StreamWriteHandler = Write; \
        protocolData->StreamIsOpenHandler = IsOpen; \
        protocolData->StreamOpenHandler = Open; \
        protocolData->StreamCloseHandler = Close; \
        protocolData->StreamDeleteHandler = Delete; \
    }

#define RegisterStreamCompressor(COMPONENT, MIMETYPE) \
    {\
        auto compressor = GetUniqueEntity("Compressor." #COMPONENT, NULL );\
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        AddChild(PropertyOf_ModuleStreamCompressors(), module, compressor, true); \
        SetStreamCompressorMimeType(compressor, MIMETYPE); \
        auto compressorData = GetStreamCompressorData(compressor); \
        compressorData->CompressHandler = Compress; \
        compressorData->DecompressHandler = Decompress; \
    }

#define RegisterSerializer(NAME, MIMETYPE) \
    {\
        auto serializer = GetUniqueEntity("Serializer." #NAME, NULL);\
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        AddChild(PropertyOf_ModuleSerializers(), module, serializer, true); \
        SetSerializerMimeType(serializer, MIMETYPE); \
        auto serializerData = GetSerializerData(serializer); \
        serializerData->SerializeHandler = Serialize; \
        serializerData->DeserializeHandler = Deserialize;\
    }

#define RegisterData(FILEPATH) \
    {\
        SetStreamPath(module, FILEPATH);\
        if(HasComponent(module, ComponentOf_PersistancePoint())) {\
            SetPersistancePointLoaded(module, true);\
        }\
    }

#endif //PLAZA_NATIVEUTILS_H
