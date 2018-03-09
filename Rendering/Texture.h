//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_TEXTURE_H
#define PLAZA_TEXTURE_H

#include <Core/Entity.h>

DeclareComponent(Texture)

DeclareComponentPropertyReactive(Texture, u16, TextureFormat)
DeclareComponentPropertyReactive(Texture, u32, TextureFlag)
DeclareComponentPropertyReactive(Texture, bool, TextureDynamic)
DeclareComponentPropertyReactive(Texture, u8, TextureMipLevels)

#define TextureFlag_NONE                UINT32_C(0x00000000) //!<
#define TextureFlag_U_MIRROR            UINT32_C(0x00000001) //!< Wrap U mode: Mirror
#define TextureFlag_U_CLAMP             UINT32_C(0x00000002) //!< Wrap U mode: Clamp
#define TextureFlag_U_BORDER            UINT32_C(0x00000003) //!< Wrap U mode: Border
#define TextureFlag_U_SHIFT             0                    //!<
#define TextureFlag_U_MASK              UINT32_C(0x00000003) //!<
#define TextureFlag_V_MIRROR            UINT32_C(0x00000004) //!< Wrap V mode: Mirror
#define TextureFlag_V_CLAMP             UINT32_C(0x00000008) //!< Wrap V mode: Clamp
#define TextureFlag_V_BORDER            UINT32_C(0x0000000c) //!< Wrap V mode: Border
#define TextureFlag_V_SHIFT             2                    //!<
#define TextureFlag_V_MASK              UINT32_C(0x0000000c) //!<
#define TextureFlag_W_MIRROR            UINT32_C(0x00000010) //!< Wrap W mode: Mirror
#define TextureFlag_W_CLAMP             UINT32_C(0x00000020) //!< Wrap W mode: Clamp
#define TextureFlag_W_BORDER            UINT32_C(0x00000030) //!< Wrap W mode: Border
#define TextureFlag_W_SHIFT             4                    //!<
#define TextureFlag_W_MASK              UINT32_C(0x00000030) //!<
#define TextureFlag_MIN_POINT           UINT32_C(0x00000040) //!< Min sampling mode: Point
#define TextureFlag_MIN_ANISOTROPIC     UINT32_C(0x00000080) //!< Min sampling mode: Anisotropic
#define TextureFlag_MIN_SHIFT           6                    //!<
#define TextureFlag_MIN_MASK            UINT32_C(0x000000c0) //!<
#define TextureFlag_MAG_POINT           UINT32_C(0x00000100) //!< Mag sampling mode: Point
#define TextureFlag_MAG_ANISOTROPIC     UINT32_C(0x00000200) //!< Mag sampling mode: Anisotropic
#define TextureFlag_MAG_SHIFT           8                    //!<
#define TextureFlag_MAG_MASK            UINT32_C(0x00000300) //!<
#define TextureFlag_MIP_POINT           UINT32_C(0x00000400) //!< Mip sampling mode: Point
#define TextureFlag_MIP_SHIFT           10                   //!<
#define TextureFlag_MIP_MASK            UINT32_C(0x00000400) //!<
#define TextureFlag_MSAA_SAMPLE         UINT32_C(0x00000800) //!< Texture will be used for MSAA sampling.
#define TextureFlag_RT                  UINT32_C(0x00001000) //!<
#define TextureFlag_RT_MSAA_X2          UINT32_C(0x00002000) //!< Render target MSAAx2 mode.
#define TextureFlag_RT_MSAA_X4          UINT32_C(0x00003000) //!< Render target MSAAx4 mode.
#define TextureFlag_RT_MSAA_X8          UINT32_C(0x00004000) //!< Render target MSAAx8 mode.
#define TextureFlag_RT_MSAA_X16         UINT32_C(0x00005000) //!< Render target MSAAx16 mode.
#define TextureFlag_RT_MSAA_SHIFT       12                   //!<
#define TextureFlag_RT_MSAA_MASK        UINT32_C(0x00007000) //!<
#define TextureFlag_RT_WRITE_ONLY       UINT32_C(0x00008000) //!< Render target will be used for writing only.
#define TextureFlag_RT_MASK             UINT32_C(0x0000f000) //!<
#define TextureFlag_COMPARE_LESS        UINT32_C(0x00010000) //!< Compare when sampling depth texture: less.
#define TextureFlag_COMPARE_LEQUAL      UINT32_C(0x00020000) //!< Compare when sampling depth texture: less or equal.
#define TextureFlag_COMPARE_EQUAL       UINT32_C(0x00030000) //!< Compare when sampling depth texture: equal.
#define TextureFlag_COMPARE_GEQUAL      UINT32_C(0x00040000) //!< Compare when sampling depth texture: greater or equal.
#define TextureFlag_COMPARE_GREATER     UINT32_C(0x00050000) //!< Compare when sampling depth texture: greater.
#define TextureFlag_COMPARE_NOTEQUAL    UINT32_C(0x00060000) //!< Compare when sampling depth texture: not equal.
#define TextureFlag_COMPARE_NEVER       UINT32_C(0x00070000) //!< Compare when sampling depth texture: never.
#define TextureFlag_COMPARE_ALWAYS      UINT32_C(0x00080000) //!< Compare when sampling depth texture: always.
#define TextureFlag_COMPARE_SHIFT       16                   //!<
#define TextureFlag_COMPARE_MASK        UINT32_C(0x000f0000) //!<
#define TextureFlag_COMPUTE_WRITE       UINT32_C(0x00100000) //!< Texture will be used for compute write.
#define TextureFlag_SRGB                UINT32_C(0x00200000) //!< Sample texture as sRGB.
#define TextureFlag_BLIT_DST            UINT32_C(0x00400000) //!< Texture will be used as blit destination.
#define TextureFlag_READ_BACK           UINT32_C(0x00800000) //!< Texture will be used for read back from GPU.

enum
{
    TEXTURE_FORMAT_BC1,          //!< DXT1
    TEXTURE_FORMAT_BC2,          //!< DXT3
    TEXTURE_FORMAT_BC3,          //!< DXT5
    TEXTURE_FORMAT_BC4,          //!< LATC1/ATI1
    TEXTURE_FORMAT_BC5,          //!< LATC2/ATI2
    TEXTURE_FORMAT_BC6H,         //!< BC6H
    TEXTURE_FORMAT_BC7,          //!< BC7
    TEXTURE_FORMAT_ETC1,         //!< ETC1 TEXTURE_FORMAT_RGB8
    TEXTURE_FORMAT_ETC2,         //!< ETC2 TEXTURE_FORMAT_RGB8
    TEXTURE_FORMAT_ETC2A,        //!< ETC2 TEXTURE_FORMAT_RGBA8
    TEXTURE_FORMAT_ETC2A1,       //!< ETC2 TEXTURE_FORMAT_RGB8A1
    TEXTURE_FORMAT_PTC12,        //!< PVRTC1 TEXTURE_FORMAT_RGB 2BPP
    TEXTURE_FORMAT_PTC14,        //!< PVRTC1 TEXTURE_FORMAT_RGB 4BPP
    TEXTURE_FORMAT_PTC12A,       //!< PVRTC1 TEXTURE_FORMAT_RGBA 2BPP
    TEXTURE_FORMAT_PTC14A,       //!< PVRTC1 TEXTURE_FORMAT_RGBA 4BPP
    TEXTURE_FORMAT_PTC22,        //!< PVRTC2 TEXTURE_FORMAT_RGBA 2BPP
    TEXTURE_FORMAT_PTC24,        //!< PVRTC2 TEXTURE_FORMAT_RGBA 4BPP

    TEXTURE_FORMAT_Unknown,      // Compressed formats above.

    TEXTURE_FORMAT_R1,
    TEXTURE_FORMAT_A8,
    TEXTURE_FORMAT_R8,
    TEXTURE_FORMAT_R8I,
    TEXTURE_FORMAT_R8U,
    TEXTURE_FORMAT_R8S,
    TEXTURE_FORMAT_R16,
    TEXTURE_FORMAT_R16I,
    TEXTURE_FORMAT_R16U,
    TEXTURE_FORMAT_R16F,
    TEXTURE_FORMAT_R16S,
    TEXTURE_FORMAT_R32I,
    TEXTURE_FORMAT_R32U,
    TEXTURE_FORMAT_R32F,
    TEXTURE_FORMAT_RG8,
    TEXTURE_FORMAT_RG8I,
    TEXTURE_FORMAT_RG8U,
    TEXTURE_FORMAT_RG8S,
    TEXTURE_FORMAT_RG16,
    TEXTURE_FORMAT_RG16I,
    TEXTURE_FORMAT_RG16U,
    TEXTURE_FORMAT_RG16F,
    TEXTURE_FORMAT_RG16S,
    TEXTURE_FORMAT_RG32I,
    TEXTURE_FORMAT_RG32U,
    TEXTURE_FORMAT_RG32F,
    TEXTURE_FORMAT_RGB8,
    TEXTURE_FORMAT_RGB8I,
    TEXTURE_FORMAT_RGB8U,
    TEXTURE_FORMAT_RGB8S,
    TEXTURE_FORMAT_RGB9E5F,
    TEXTURE_FORMAT_BGRA8,
    TEXTURE_FORMAT_RGBA8,
    TEXTURE_FORMAT_RGBA8I,
    TEXTURE_FORMAT_RGBA8U,
    TEXTURE_FORMAT_RGBA8S,
    TEXTURE_FORMAT_RGBA16,
    TEXTURE_FORMAT_RGBA16I,
    TEXTURE_FORMAT_RGBA16U,
    TEXTURE_FORMAT_RGBA16F,
    TEXTURE_FORMAT_RGBA16S,
    TEXTURE_FORMAT_RGBA32I,
    TEXTURE_FORMAT_RGBA32U,
    TEXTURE_FORMAT_RGBA32F,
    TEXTURE_FORMAT_R5G6B5,
    TEXTURE_FORMAT_RGBA4,
    TEXTURE_FORMAT_RGB5A1,
    TEXTURE_FORMAT_RGB10A2,
    TEXTURE_FORMAT_RG11B10F,

    TEXTURE_FORMAT_UnknownDepth, // Depth formats below.

    TEXTURE_FORMAT_D16,
    TEXTURE_FORMAT_D24,
    TEXTURE_FORMAT_D24S8,
    TEXTURE_FORMAT_D32,
    TEXTURE_FORMAT_D16F,
    TEXTURE_FORMAT_D24F,
    TEXTURE_FORMAT_D32F,
    TEXTURE_FORMAT_D0S8,

    TEXTURE_FORMAT_Count
};


#endif //PLAZA_TEXTURE_H
