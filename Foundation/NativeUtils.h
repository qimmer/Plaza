//
// Created by Kim on 14/07/2018.
//

#ifndef PLAZA_FOUNDATION_NATIVEUTILS_H
#define PLAZA_FOUNDATION_NATIVEUTILS_H

#include <Core/Debug.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>

#define RegisterStreamProtocol(COMPONENT, IDENTIFIER) \
    {\
        auto protocol = GetUniqueEntity("Protocol " #COMPONENT, NULL );\
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        __InjectArrayPropertyElement(PropertyOf_ModuleStreamProtocols(), module, protocol); \
        SetName(protocol, #COMPONENT "Protocol");\
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
        auto compressor = GetUniqueEntity("Compressor " #COMPONENT, NULL );\
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        __InjectArrayPropertyElement(PropertyOf_ModuleStreamCompressors(), module, compressor); \
        SetName(compressor, #COMPONENT "Compressor");\
        SetStreamCompressor(compressor, MIMETYPE); \
        auto compressorData = GetStreamCompressorData(compressor); \
        compressorData->CompressHandler = Compress; \
        compressorData->DecompressHandler = Decompress; \
    }

#define RegisterSerializer(NAME, MIMETYPE) \
    {\
        auto serializer = GetUniqueEntity("Serializer " #NAME, NULL);\
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        __InjectArrayPropertyElement(PropertyOf_ModuleSerializers(), module, serializer); \
        SetName(serializer, #NAME "Serializer");\
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
