//
// Created by Kim on 14/07/2018.
//

#ifndef PLAZA_FOUNDATION_NATIVEUTILS_H
#define PLAZA_FOUNDATION_NATIVEUTILS_H

#include <Core/NativeUtils.h>

#define StreamProtocol(NAME)\
    Declare(Protocol, NAME)

#define StreamCompressor(NAME)\
    Declare(Compressor, NAME)

#define Serializer(NAME)\
    Declare(Serializer, NAME)

#define FileType(NAME)\
    Declare(FileType, NAME)

#define RegisterStreamProtocol(NAME, IDENTIFIER) \
    {\
        auto protocol = ProtocolOf_ ## NAME ();\
        SetIdentification(protocol, {"Protocol." #NAME});\
        StreamProtocol protocolData;\
        protocolData.StreamProtocolIdentifier = IDENTIFIER; \
        protocolData.StreamSeekHandler = Seek; \
        protocolData.StreamTellHandler = Tell; \
        protocolData.StreamReadHandler = Read; \
        protocolData.StreamWriteHandler = Write; \
        protocolData.StreamIsOpenHandler = IsOpen; \
        protocolData.StreamOpenHandler = Open; \
        protocolData.StreamCloseHandler = Close; \
        protocolData.StreamDeleteHandler = Delete; \
        SetStreamProtocol(protocol, protocolData);\
        \
        auto streamModuleData = GetStreamExtensionModule(module);\
        streamModuleData.ModuleStreamProtocols.Add(protocol);\
        SetStreamExtensionModule(module, streamModuleData); \
    }

#define RegisterStreamCompressor(NAME, MIMETYPE) \
    {\
        auto compressor = CompressorOf_ ## NAME();\
        SetIdentification(compressor, {"Compressor." #NAME});\
        auto compressorData = GetStreamCompressor(compressor); \
        compressorData.StreamCompressorMimeType = MIMETYPE; \
        compressorData.CompressHandler = Compress; \
        compressorData.DecompressHandler = Decompress; \
        SetStreamCompressor(compressor, compressorData);\
        auto streamModuleData = GetStreamExtensionModule(module);\
        streamModuleData.ModuleStreamCompressors.Add(compressor);\
        SetStreamExtensionModule(module, streamModuleData); \
    }

#define RegisterSerializer(NAME, MIMETYPE) \
    {\
        auto serializer = SerializerOf_ ## NAME();\
        SetIdentification(serializer, {"Serializer." #NAME});\
        auto serializerData = GetSerializer(serializer); \
        serializerData.SerializerMimeType = MIMETYPE; \
        serializerData.SerializeHandler = Serialize; \
        serializerData.DeserializeHandler = Deserialize;\
        SetSerializer(serializer, serializerData);\
        auto streamModuleData = GetStreamExtensionModule(module);\
        streamModuleData.ModuleSerializers.Add(serializer);\
        SetStreamExtensionModule(module, streamModuleData); \
    }

#define RegisterFileType(NAME, MIMETYPE, EXTENSION) \
    {\
        auto fileType = FileTypeOf_ ## NAME();\
        SetIdentification(fileType, {"FileType." #NAME});\
        auto fileTypeData = GetFileType(fileType); \
        fileTypeData.FileTypeMimeType = MIMETYPE; \
        fileTypeData.FileTypeExtension = EXTENSION; \
        SetFileType(fileType, fileTypeData);\
        auto streamModuleData = GetStreamExtensionModule(module);\
        streamModuleData.ModuleFileTypes.Add(fileType);\
        SetStreamExtensionModule(module, streamModuleData); \
    }

#define RegisterData(FILEPATH) \
    {\
        SetStreamPath(module, FILEPATH);\
        if(HasComponent(module, ComponentOf_PersistancePoint())) {\
            SetPersistancePointLoaded(module, true);\
        }\
    }

#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>

#endif //PLAZA_NATIVEUTILS_H
