//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_TEXTURE_H
#define PLAZA_TEXTURE_H

#include <Core/NativeUtils.h>

Unit(Texture)
    Enum(TextureFormat)
    Enum(TextureFlag)

    Component(Texture)
        Property(u16, TextureFormat)
        Property(u32, TextureFlag)
        Property(bool, TextureDynamic)
        Property(u8, TextureMipLevels)

    Component(TextureReader)
        ReferenceProperty(Texture, TextureReaderSource)
        ChildProperty(Texture, TextureReaderBlitTexture)
        ChildProperty(Stream, TextureReaderTarget)

#define TextureFlag_NONE                UINT32_C(0x00000000) //!<

#define TextureFlag_U_MIRROR            UINT32_C(0x00000001) //!< Wrap U mode: Mirror
#define TextureFlag_U_CLAMP             UINT32_C(0x00000002) //!< Wrap U mode: Clamp
#define TextureFlag_U_BORDER            UINT32_C(0x00000003) //!< Wrap U mode: Border
#define TextureFlag_V_MIRROR            UINT32_C(0x00000004) //!< Wrap V mode: Mirror
#define TextureFlag_V_CLAMP             UINT32_C(0x00000008) //!< Wrap V mode: Clamp
#define TextureFlag_V_BORDER            UINT32_C(0x0000000c) //!< Wrap V mode: Border
#define TextureFlag_W_MIRROR            UINT32_C(0x00000010) //!< Wrap W mode: Mirror
#define TextureFlag_W_CLAMP             UINT32_C(0x00000020) //!< Wrap W mode: Clamp
#define TextureFlag_W_BORDER            UINT32_C(0x00000030) //!< Wrap W mode: Border

#define TextureFlag_MIN_POINT           UINT32_C(0x00000040) //!< Min sampling mode: Point
#define TextureFlag_MIN_ANISOTROPIC     UINT32_C(0x00000080) //!< Min sampling mode: Anisotropic
#define TextureFlag_MAG_POINT           UINT32_C(0x00000100) //!< Mag sampling mode: Point
#define TextureFlag_MAG_ANISOTROPIC     UINT32_C(0x00000200) //!< Mag sampling mode: Anisotropic
#define TextureFlag_MIP_POINT           UINT32_C(0x00000400) //!< Mip sampling mode: Point

#define TextureFlag_MSAA_SAMPLE         UINT32_C(0x00000800) //!< Texture will be used for MSAA sampling.

#define TextureFlag_RT                  UINT32_C(0x00001000) //!<
#define TextureFlag_RT_MSAA_X2          UINT32_C(0x00002000) //!< Render target MSAAx2 mode.
#define TextureFlag_RT_MSAA_X4          UINT32_C(0x00003000) //!< Render target MSAAx4 mode.
#define TextureFlag_RT_MSAA_X8          UINT32_C(0x00004000) //!< Render target MSAAx8 mode.
#define TextureFlag_RT_MSAA_X16         UINT32_C(0x00005000) //!< Render target MSAAx16 mode.
#define TextureFlag_RT_WRITE_ONLY       UINT32_C(0x00008000) //!< Render target will be used for writing only.

#define TextureFlag_COMPARE_LESS        UINT32_C(0x00010000) //!< Compare when sampling depth texture: less.
#define TextureFlag_COMPARE_LEQUAL      UINT32_C(0x00020000) //!< Compare when sampling depth texture: less or equal.
#define TextureFlag_COMPARE_EQUAL       UINT32_C(0x00030000) //!< Compare when sampling depth texture: equal.
#define TextureFlag_COMPARE_GEQUAL      UINT32_C(0x00040000) //!< Compare when sampling depth texture: greater or equal.
#define TextureFlag_COMPARE_GREATER     UINT32_C(0x00050000) //!< Compare when sampling depth texture: greater.
#define TextureFlag_COMPARE_NOTEQUAL    UINT32_C(0x00060000) //!< Compare when sampling depth texture: not equal.
#define TextureFlag_COMPARE_NEVER       UINT32_C(0x00070000) //!< Compare when sampling depth texture: never.
#define TextureFlag_COMPARE_ALWAYS      UINT32_C(0x00080000) //!< Compare when sampling depth texture: always.

#define TextureFlag_COMPUTE_WRITE       UINT32_C(0x00100000) //!< Texture will be used for compute write.

#define TextureFlag_SRGB                UINT32_C(0x00200000) //!< Sample texture as sRGB.

#define TextureFlag_BLIT_DST            UINT32_C(0x00400000) //!< Texture will be used as blit destination.
#define TextureFlag_READ_BACK           UINT32_C(0x00800000) //!< Texture will be used for read back from GPU.

enum
{
    TextureFormat_BC1,          //!< DXT1
    TextureFormat_BC2,          //!< DXT3
    TextureFormat_BC3,          //!< DXT5
    TextureFormat_BC4,          //!< LATC1/ATI1
    TextureFormat_BC5,          //!< LATC2/ATI2
    TextureFormat_BC6H,         //!< BC6H
    TextureFormat_BC7,          //!< BC7
    TextureFormat_ETC1,         //!< ETC1 TextureFormat_RGB8
    TextureFormat_ETC2,         //!< ETC2 TextureFormat_RGB8
    TextureFormat_ETC2A,        //!< ETC2 TextureFormat_RGBA8
    TextureFormat_ETC2A1,       //!< ETC2 TextureFormat_RGB8A1
    TextureFormat_PTC12,        //!< PVRTC1 TextureFormat_RGB 2BPP
    TextureFormat_PTC14,        //!< PVRTC1 TextureFormat_RGB 4BPP
    TextureFormat_PTC12A,       //!< PVRTC1 TextureFormat_RGBA 2BPP
    TextureFormat_PTC14A,       //!< PVRTC1 TextureFormat_RGBA 4BPP
    TextureFormat_PTC22,        //!< PVRTC2 TextureFormat_RGBA 2BPP
    TextureFormat_PTC24,        //!< PVRTC2 TextureFormat_RGBA 4BPP

    TextureFormat_Unknown,      // Compressed formats above.

    TextureFormat_R1,
    TextureFormat_A8,
    TextureFormat_R8,
    TextureFormat_R8I,
    TextureFormat_R8U,
    TextureFormat_R8S,
    TextureFormat_R16,
    TextureFormat_R16I,
    TextureFormat_R16U,
    TextureFormat_R16F,
    TextureFormat_R16S,
    TextureFormat_R32I,
    TextureFormat_R32U,
    TextureFormat_R32F,
    TextureFormat_RG8,
    TextureFormat_RG8I,
    TextureFormat_RG8U,
    TextureFormat_RG8S,
    TextureFormat_RG16,
    TextureFormat_RG16I,
    TextureFormat_RG16U,
    TextureFormat_RG16F,
    TextureFormat_RG16S,
    TextureFormat_RG32I,
    TextureFormat_RG32U,
    TextureFormat_RG32F,
    TextureFormat_RGB8,
    TextureFormat_RGB8I,
    TextureFormat_RGB8U,
    TextureFormat_RGB8S,
    TextureFormat_RGB9E5F,
    TextureFormat_BGRA8,
    TextureFormat_RGBA8,
    TextureFormat_RGBA8I,
    TextureFormat_RGBA8U,
    TextureFormat_RGBA8S,
    TextureFormat_RGBA16,
    TextureFormat_RGBA16I,
    TextureFormat_RGBA16U,
    TextureFormat_RGBA16F,
    TextureFormat_RGBA16S,
    TextureFormat_RGBA32I,
    TextureFormat_RGBA32U,
    TextureFormat_RGBA32F,
    TextureFormat_R5G6B5,
    TextureFormat_RGBA4,
    TextureFormat_RGB5A1,
    TextureFormat_RGB10A2,
    TextureFormat_RG11B10F,

    TextureFormat_UnknownDepth, // Depth formats below.

    TextureFormat_D16,
    TextureFormat_D24,
    TextureFormat_D24S8,
    TextureFormat_D32,
    TextureFormat_D16F,
    TextureFormat_D24F,
    TextureFormat_D32F,
    TextureFormat_D0S8,

    TextureFormat_Count
};


#endif //PLAZA_TEXTURE_H
