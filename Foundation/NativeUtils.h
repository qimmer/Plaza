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
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        auto protocol = AddModuleStreamProtocols(module); \
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
        AddComponent(module, ComponentOf_StreamExtensionModule());\
        auto compressor = AddModuleCompressors(module); \
        SetName(compressor, #COMPONENT "Compressor");\
        SetStreamCompressor(compressor, MIMETYPE); \
        auto compressorData = GetStreamCompressorData(compressor); \
        compressorData->CompressHandler = Compress; \
        compressorData->DecompressHandler = Decompress; \
    }

#define RegisterSerializer(NAME, MIMETYPE) \
    AddComponent(module, ComponentOf_StreamExtensionModule());\
    auto NAME ## serializer = AddModuleSerializers(module); \
    SetName(NAME ## serializer, #NAME "Serializer");\
    SetSerializerMimeType(NAME ## serializer, MIMETYPE); \
    auto NAME ## serializerData = GetSerializerData(NAME ## serializer); \
    NAME ## serializerData->SerializeHandler = Serialize; \
    NAME ## serializerData->DeserializeHandler = Deserialize;

#define RegisterData(FILEPATH) \
    {\
        SetStreamPath(module, FILEPATH);\
        if(HasComponent(module, ComponentOf_PersistancePoint())) {\
            SetPersistancePointLoaded(module, true);\
        }\
    }

#endif //PLAZA_NATIVEUTILS_H
