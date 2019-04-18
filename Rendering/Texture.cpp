//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Stream.h>
#include "Texture.h"

API_EXPORT Type GetTextureFormatChannelType(u16 format) {
    return 0;
}

API_EXPORT u8 GetTextureFormatNumChannels(u16 format) {
    switch(format) {
        case TextureFormat_R1:
        case TextureFormat_A8:
        case TextureFormat_R8:
        case TextureFormat_R8I:
        case TextureFormat_R8U:
        case TextureFormat_R8S:
        case TextureFormat_R16:
        case TextureFormat_R16I:
        case TextureFormat_R16U:
        case TextureFormat_R16F:
        case TextureFormat_R16S:
        case TextureFormat_R32I:
        case TextureFormat_R32U:
        case TextureFormat_R32F:
            return 1;
        case TextureFormat_RG8:
        case TextureFormat_RG8I:
        case TextureFormat_RG8U:
        case TextureFormat_RG8S:
        case TextureFormat_RG16:
        case TextureFormat_RG16I:
        case TextureFormat_RG16U:
        case TextureFormat_RG16F:
        case TextureFormat_RG16S:
        case TextureFormat_RG32I:
        case TextureFormat_RG32U:
        case TextureFormat_RG32F:
            return 2;
        case TextureFormat_RGB8:
        case TextureFormat_RGB8I:
        case TextureFormat_RGB8U:
        case TextureFormat_RGB8S:
            return 3;
        case TextureFormat_BGRA8:
        case TextureFormat_RGBA8:
        case TextureFormat_RGBA8I:
        case TextureFormat_RGBA8U:
        case TextureFormat_RGBA8S:
        case TextureFormat_RGBA16:
        case TextureFormat_RGBA16I:
        case TextureFormat_RGBA16U:
        case TextureFormat_RGBA16F:
        case TextureFormat_RGBA16S:
        case TextureFormat_RGBA32I:
        case TextureFormat_RGBA32U:
        case TextureFormat_RGBA32F:
            return 4;
        default:
            return 0;
    }
}

BeginUnit(Texture)
    BeginEnum(TextureFormat, false)
        RegisterFlag(TextureFormat_BC1)          //!< DXT1
        RegisterFlag(TextureFormat_BC2)          //!< DXT3
        RegisterFlag(TextureFormat_BC3)          //!< DXT5
        RegisterFlag(TextureFormat_BC4)          //!< LATC1/ATI1
        RegisterFlag(TextureFormat_BC5)          //!< LATC2/ATI2
        RegisterFlag(TextureFormat_BC6H)         //!< BC6H
        RegisterFlag(TextureFormat_BC7)          //!< BC7
        RegisterFlag(TextureFormat_ETC1)         //!< ETC1     RegisterFlag(TextureFormat_RGB8
        RegisterFlag(TextureFormat_ETC2)         //!< ETC2     RegisterFlag(TextureFormat_RGB8
        RegisterFlag(TextureFormat_ETC2A)        //!< ETC2     RegisterFlag(TextureFormat_RGBA8
        RegisterFlag(TextureFormat_ETC2A1)       //!< ETC2     RegisterFlag(TextureFormat_RGB8A1
        RegisterFlag(TextureFormat_PTC12)       //!< PVRTC1     RegisterFlag(TextureFormat_RGB 2BPP
        RegisterFlag(TextureFormat_PTC14)        //!< PVRTC1     RegisterFlag(TextureFormat_RGB 4BPP
        RegisterFlag(TextureFormat_PTC12A)       //!< PVRTC1     RegisterFlag(TextureFormat_RGBA 2BPP
        RegisterFlag(TextureFormat_PTC14A)       //!< PVRTC1     RegisterFlag(TextureFormat_RGBA 4BPP
        RegisterFlag(TextureFormat_PTC22)        //!< PVRTC2     RegisterFlag(TextureFormat_RGBA 2BPP
        RegisterFlag(TextureFormat_PTC24)        //!< PVRTC2     RegisterFlag(TextureFormat_RGBA 4BPP

        RegisterFlag(TextureFormat_Unknown)      // Compressed formats above.

        RegisterFlag(TextureFormat_R1)
        RegisterFlag(TextureFormat_A8)
        RegisterFlag(TextureFormat_R8)
        RegisterFlag(TextureFormat_R8I)
        RegisterFlag(TextureFormat_R8U)
        RegisterFlag(TextureFormat_R8S)
        RegisterFlag(TextureFormat_R16)
        RegisterFlag(TextureFormat_R16I)
        RegisterFlag(TextureFormat_R16U)
        RegisterFlag(TextureFormat_R16F)
        RegisterFlag(TextureFormat_R16S)
        RegisterFlag(TextureFormat_R32I)
        RegisterFlag(TextureFormat_R32U)
        RegisterFlag(TextureFormat_R32F)
        RegisterFlag(TextureFormat_RG8)
        RegisterFlag(TextureFormat_RG8I)
        RegisterFlag(TextureFormat_RG8U)
        RegisterFlag(TextureFormat_RG8S)
        RegisterFlag(TextureFormat_RG16)
        RegisterFlag(TextureFormat_RG16I)
        RegisterFlag(TextureFormat_RG16U)
        RegisterFlag(TextureFormat_RG16F)
        RegisterFlag(TextureFormat_RG16S)
        RegisterFlag(TextureFormat_RG32I)
        RegisterFlag(TextureFormat_RG32U)
        RegisterFlag(TextureFormat_RG32F)
        RegisterFlag(TextureFormat_RGB8)
        RegisterFlag(TextureFormat_RGB8I)
        RegisterFlag(TextureFormat_RGB8U)
        RegisterFlag(TextureFormat_RGB8S)
        RegisterFlag(TextureFormat_RGB9E5F)
        RegisterFlag(TextureFormat_BGRA8)
        RegisterFlag(TextureFormat_RGBA8)
        RegisterFlag(TextureFormat_RGBA8I)
        RegisterFlag(TextureFormat_RGBA8U)
        RegisterFlag(TextureFormat_RGBA8S)
        RegisterFlag(TextureFormat_RGBA16)
        RegisterFlag(TextureFormat_RGBA16I)
        RegisterFlag(TextureFormat_RGBA16U)
        RegisterFlag(TextureFormat_RGBA16F)
        RegisterFlag(TextureFormat_RGBA16S)
        RegisterFlag(TextureFormat_RGBA32I)
        RegisterFlag(TextureFormat_RGBA32U)
        RegisterFlag(TextureFormat_RGBA32F)
        RegisterFlag(TextureFormat_R5G6B5)
        RegisterFlag(TextureFormat_RGBA4)
        RegisterFlag(TextureFormat_RGB5A1)
        RegisterFlag(TextureFormat_RGB10A2)
        RegisterFlag(TextureFormat_RG11B10F)

        RegisterFlag(TextureFormat_UnknownDepth) // Depth formats below.

        RegisterFlag(TextureFormat_D16)
        RegisterFlag(TextureFormat_D24)
        RegisterFlag(TextureFormat_D24S8)
        RegisterFlag(TextureFormat_D32)
        RegisterFlag(TextureFormat_D16F)
        RegisterFlag(TextureFormat_D24F)
        RegisterFlag(TextureFormat_D32F)
        RegisterFlag(TextureFormat_D0S8)
    EndEnum()

    BeginEnum(TextureFlag, true)
        RegisterFlag(TextureFlag_NONE) //!<

        RegisterFlag(TextureFlag_U_MIRROR) //!< Wrap U mode: Mirror
        RegisterFlag(TextureFlag_U_CLAMP) //!< Wrap U mode: Clamp
        RegisterFlag(TextureFlag_U_BORDER) //!< Wrap U mode: Border
        RegisterFlag(TextureFlag_V_MIRROR) //!< Wrap V mode: Mirror
        RegisterFlag(TextureFlag_V_CLAMP) //!< Wrap V mode: Clamp
        RegisterFlag(TextureFlag_V_BORDER) //!< Wrap V mode: Border
        RegisterFlag(TextureFlag_W_MIRROR) //!< Wrap W mode: Mirror
        RegisterFlag(TextureFlag_W_CLAMP) //!< Wrap W mode: Clamp
        RegisterFlag(TextureFlag_W_BORDER) //!< Wrap W mode: Border

        RegisterFlag(TextureFlag_MIN_POINT) //!< Min sampling mode: Point
        RegisterFlag(TextureFlag_MIN_ANISOTROPIC) //!< Min sampling mode: Anisotropic
        RegisterFlag(TextureFlag_MAG_POINT) //!< Mag sampling mode: Point
        RegisterFlag(TextureFlag_MAG_ANISOTROPIC) //!< Mag sampling mode: Anisotropic
        RegisterFlag(TextureFlag_MIP_POINT) //!< Mip sampling mode: Point

        RegisterFlag(TextureFlag_MSAA_SAMPLE) //!< Texture will be used for MSAA sampling.

        RegisterFlag(TextureFlag_RT) //!<
        RegisterFlag(TextureFlag_RT_MSAA_X2) //!< Render target MSAAx2 mode.
        RegisterFlag(TextureFlag_RT_MSAA_X4) //!< Render target MSAAx4 mode.
        RegisterFlag(TextureFlag_RT_MSAA_X8) //!< Render target MSAAx8 mode.
        RegisterFlag(TextureFlag_RT_MSAA_X16) //!< Render target MSAAx16 mode.
        RegisterFlag(TextureFlag_RT_WRITE_ONLY) //!< Render target will be used for writing only.

        RegisterFlag(TextureFlag_COMPARE_LESS) //!< Compare when sampling depth texture: less.
        RegisterFlag(TextureFlag_COMPARE_LEQUAL) //!< Compare when sampling depth texture: less or equal.
        RegisterFlag(TextureFlag_COMPARE_EQUAL) //!< Compare when sampling depth texture: equal.
        RegisterFlag(TextureFlag_COMPARE_GEQUAL) //!< Compare when sampling depth texture: greater or equal.
        RegisterFlag(TextureFlag_COMPARE_GREATER) //!< Compare when sampling depth texture: greater.
        RegisterFlag(TextureFlag_COMPARE_NOTEQUAL) //!< Compare when sampling depth texture: not equal.
        RegisterFlag(TextureFlag_COMPARE_NEVER) //!< Compare when sampling depth texture: never.
        RegisterFlag(TextureFlag_COMPARE_ALWAYS) //!< Compare when sampling depth texture: always.

        RegisterFlag(TextureFlag_COMPUTE_WRITE) //!< Texture will be used for compute write.

        RegisterFlag(TextureFlag_SRGB) //!< Sample texture as sRGB.

        RegisterFlag(TextureFlag_BLIT_DST) //!< Texture will be used as blit destination.
        RegisterFlag(TextureFlag_READ_BACK) //!< Texture will be used for read back from GPU.
    EndEnum()

    BeginComponent(Texture)
        RegisterBase(Stream)
        RegisterPropertyEnum(u16, TextureFormat, TextureFormat)
        RegisterPropertyEnum(u32, TextureFlag, TextureFlag)
        RegisterProperty(bool, TextureDynamic)
        RegisterProperty(u8, TextureMipLevels)
    EndComponent()
EndUnit()
