//
// Created by Kim on 14/07/2018.
//

#ifndef PLAZA_FOUNDATION_NATIVEUTILS_H
#define PLAZA_FOUNDATION_NATIVEUTILS_H

#include <Core/Debug.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>

#define RegisterStreamProtocol(COMPONENT, IDENTIFIER) \
    AddComponent(module, ComponentOf_StreamExtensionModule());\
    auto COMPONENT ## protocol = AddModuleStreamProtocols(module); \
    SetName(COMPONENT ## protocol, #COMPONENT "Protocol");\
    SetStreamProtocolIdentifier(COMPONENT ## protocol, IDENTIFIER); \
    SetStreamProtocolComponent(COMPONENT ## protocol, ComponentOf_ ## COMPONENT ()); \
    auto COMPONENT ## protocolData = GetStreamProtocolData(COMPONENT ## protocol); \
    Assert(COMPONENT ## protocol, COMPONENT ## protocolData);\
    COMPONENT ## protocolData->StreamSeekHandler = Seek; \
    COMPONENT ## protocolData->StreamTellHandler = Tell; \
    COMPONENT ## protocolData->StreamReadHandler = Read; \
    COMPONENT ## protocolData->StreamWriteHandler = Write; \
    COMPONENT ## protocolData->StreamIsOpenHandler = IsOpen; \
    COMPONENT ## protocolData->StreamOpenHandler = Open; \
    COMPONENT ## protocolData->StreamCloseHandler = Close;

#define RegisterStreamCompressor(COMPONENT, MIMETYPE) \
    AddComponent(module, ComponentOf_StreamExtensionModule());\
    auto COMPONENT ## compressor = AddModuleCompressors(module); \
    SetName(COMPONENT ## compressor, #COMPONENT "Compressor");\
    SetStreamCompressor(COMPONENT ## compressor, MIMETYPE); \
    auto COMPONENT ## compressorData = GetStreamCompressorData(COMPONENT ## compressor); \
    COMPONENT ## compressorData->CompressHandler = Compress; \
    COMPONENT ## compressorData->DecompressHandler = Decompress;

#define RegisterFileType(EXTENSION, MIMETYPE, COMPONENT) \
    AddComponent(module, ComponentOf_StreamExtensionModule());\
    node = AddModuleFileTypes(module); \
    SetName(node, "FileType" #EXTENSION);\
    SetFileTypeExtension(node, EXTENSION); \
    SetFileTypeMimeType(node, MIMETYPE); \
    SetFileTypeComponent(node, COMPONENT);

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
