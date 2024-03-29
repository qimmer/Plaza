//
// Created by Kim Johannsen on 16-03-2018.
//

#include "VoxPersistance.h"
#include "VoxelPalette.h"
#include "VoxelChunk.h"
#include "VoxelMesh.h"
#include "VoxelColor.h"
#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>
#include <memory.h>
#include <Rendering/Texture2D.h>
#include <Core/String.h>
#include <Rendering/Texture.h>
#include <Foundation/Persistance.h>
#include <Core/Types.h>
#include <algorithm>

const u32 default_palette[256] = {
        0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
        0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
        0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
        0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
        0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
        0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
        0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
        0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
        0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
        0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
        0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
        0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
        0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
        0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
        0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
        0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};

struct VoxHeader {
    char signature[4];
    s32 version;
};

struct ChunkHeader {
    char chunkType[4];
    s32 chunkSize;
    s32 numChildChunkBytes;
};

static bool ReadChunk(Entity stream, Entity parent, v3i* chunkSizeStore, u32 *chunkCnt, Entity *currentPalette) {
    auto startOffset = StreamTell(stream);

    ChunkHeader header;
    if(StreamRead(stream, sizeof(ChunkHeader), &header) != sizeof(ChunkHeader)) {
        Log(LogChannel_Core, LogSeverity_Error, "Cannot read VOX chunk header.");
        return false;
    }

    if(header.chunkSize > 0) {
        static Vector<char> chunkData(header.chunkSize);
        chunkData.resize(header.chunkSize);

        if(StreamRead(stream, header.chunkSize, chunkData.data()) != header.chunkSize) {
            Log(LogChannel_Core, LogSeverity_Error, "Cannot read VOX chunk data of %d bytes.", header.chunkSize);
            return false;
        }

        if(memcmp(header.chunkType, "SIZE", 4) == 0) {
            auto size = *(v3i*)chunkData.data();
            std::swap(size.y, size.z);
            *chunkSizeStore = size;
        } else if(memcmp(header.chunkType, "RGBA", 4) == 0) { //-V517
            *currentPalette = CreateVoxelPalette(parent, "Palette");

            rgba8 *colors = (rgba8*)chunkData.data();
            for(auto i = 0; i < 256; ++i) {
                char name[16];
                snprintf(name, 16, "%d", i);

                auto voxelColor = CreateVoxelColor(*currentPalette, name);
                SetVoxelColorColor(voxelColor, colors[i]);
                SetVoxelColorValue(voxelColor, i);
            }

        } else if(memcmp(header.chunkType, "XYZI", 4) == 0) {
            auto chunkPtr = chunkData.data();

            s32 numVoxels = *(s32*)chunkPtr;
            chunkPtr += 4;
            auto voxels = (rgba8*)chunkPtr;

            auto voxelChunkData = (Voxel*)calloc(chunkSizeStore->x * chunkSizeStore->y * chunkSizeStore->z, sizeof(Voxel));
			Assert(voxelChunkData);
            for(auto i = 0; i < numVoxels; ++i) {
                voxelChunkData[chunkSizeStore->x * chunkSizeStore->y * voxels[i].y + chunkSizeStore->x * voxels[i].z + voxels[i].x] = voxels[i].w;
            }

            char name[PathMax];
            snprintf(name, PathMax, "Chunk_%lu", (unsigned long)++(*chunkCnt));

            parent = CreateVoxelChunk(parent, name);
            SetVoxelChunkSize(parent, *chunkSizeStore);
            SetVoxelChunkRegion(parent, {0, 0, 0}, *chunkSizeStore, {0, 0, 0}, *chunkSizeStore, voxelChunkData);
            SetVoxelMeshChunk(parent, parent);
            SetVoxelMeshPalette(parent, *currentPalette);
            SetEntityPersistancePoint(parent, stream);

            free(voxelChunkData);
        } else if(memcmp(header.chunkType, "RGBA", 4) == 0) {

        } else {
            //Log(LogChannel_Core, LogSeverity_Warning, "Unknown VOX chunk '%c%c%c%c'. Skipping %d bytes.", header.chunkType[0], header.chunkType[1], header.chunkType[2], header.chunkType[3], header.chunkSize);
        }
    }

    while((StreamTell(stream) - startOffset) < header.numChildChunkBytes) {
        if(!ReadChunk(stream, parent, chunkSizeStore, chunkCnt, currentPalette)) return false;
    }

    return true;
}

static bool SerializeImage(Entity entity) {
    return false;
}

static bool DeserializeImage(Entity entity) {
    if(!StreamOpen(entity, StreamMode_Read)) return false;

    v3i sizeStore = {0, 0, 0};
    u32 chunkCnt = 0;
    Entity currentPalette = 0;
    VoxHeader header;
    if(StreamRead(entity, sizeof(VoxHeader), &header) != sizeof(VoxHeader) || memcmp(header.signature, "VOX", 3) != 0) {
        StreamClose(entity);
        Log(LogChannel_Core, LogSeverity_Error, "Cannot read VOX file signature. Maybe wrong file type?");
        return false;
    }

    auto result = ReadChunk(entity, entity, &sizeStore, &chunkCnt, &currentPalette);
    StreamClose(entity);

    return result;
}

LocalFunction(OnServiceStart, void, Service service) {
    Serializer s {
            SerializeImage,
            DeserializeImage
    };
    AddFileType(".vox", "voxel/vox");
    AddSerializer("voxel/vox", &s);
}

LocalFunction(OnServiceStop, void, Service service){
    RemoveFileType(".vox");
    RemoveSerializer("voxel/vox");
}

DefineService(VoxPersistance)
    RegisterSubscription(VoxPersistanceStarted, OnServiceStart, 0)
    RegisterSubscription(VoxPersistanceStopped, OnServiceStop, 0)
EndService()
