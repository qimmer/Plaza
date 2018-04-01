//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Stream.h>
#include "Texture.h"

DefineEnum(TextureFormat, false)
    DefineFlag(TextureFormat_BC1)          //!< DXT1
    DefineFlag(TextureFormat_BC2)          //!< DXT3
    DefineFlag(TextureFormat_BC3)          //!< DXT5
    DefineFlag(TextureFormat_BC4)          //!< LATC1/ATI1
    DefineFlag(TextureFormat_BC5)          //!< LATC2/ATI2
    DefineFlag(TextureFormat_BC6H)         //!< BC6H
    DefineFlag(TextureFormat_BC7)          //!< BC7
    DefineFlag(TextureFormat_ETC1)         //!< ETC1     DefineFlag(TextureFormat_RGB8
    DefineFlag(TextureFormat_ETC2)         //!< ETC2     DefineFlag(TextureFormat_RGB8
    DefineFlag(TextureFormat_ETC2A)        //!< ETC2     DefineFlag(TextureFormat_RGBA8
    DefineFlag(TextureFormat_ETC2A1)       //!< ETC2     DefineFlag(TextureFormat_RGB8A1
    DefineFlag(TextureFormat_PTC12)       //!< PVRTC1     DefineFlag(TextureFormat_RGB 2BPP
    DefineFlag(TextureFormat_PTC14)        //!< PVRTC1     DefineFlag(TextureFormat_RGB 4BPP
    DefineFlag(TextureFormat_PTC12A)       //!< PVRTC1     DefineFlag(TextureFormat_RGBA 2BPP
    DefineFlag(TextureFormat_PTC14A)       //!< PVRTC1     DefineFlag(TextureFormat_RGBA 4BPP
    DefineFlag(TextureFormat_PTC22)        //!< PVRTC2     DefineFlag(TextureFormat_RGBA 2BPP
    DefineFlag(TextureFormat_PTC24)        //!< PVRTC2     DefineFlag(TextureFormat_RGBA 4BPP

    DefineFlag(TextureFormat_Unknown)      // Compressed formats above.

    DefineFlag(TextureFormat_R1)
    DefineFlag(TextureFormat_A8)
    DefineFlag(TextureFormat_R8)
    DefineFlag(TextureFormat_R8I)
    DefineFlag(TextureFormat_R8U)
    DefineFlag(TextureFormat_R8S)
    DefineFlag(TextureFormat_R16)
    DefineFlag(TextureFormat_R16I)
    DefineFlag(TextureFormat_R16U)
    DefineFlag(TextureFormat_R16F)
    DefineFlag(TextureFormat_R16S)
    DefineFlag(TextureFormat_R32I)
    DefineFlag(TextureFormat_R32U)
    DefineFlag(TextureFormat_R32F)
    DefineFlag(TextureFormat_RG8)
    DefineFlag(TextureFormat_RG8I)
    DefineFlag(TextureFormat_RG8U)
    DefineFlag(TextureFormat_RG8S)
    DefineFlag(TextureFormat_RG16)
    DefineFlag(TextureFormat_RG16I)
    DefineFlag(TextureFormat_RG16U)
    DefineFlag(TextureFormat_RG16F)
    DefineFlag(TextureFormat_RG16S)
    DefineFlag(TextureFormat_RG32I)
    DefineFlag(TextureFormat_RG32U)
    DefineFlag(TextureFormat_RG32F)
    DefineFlag(TextureFormat_RGB8)
    DefineFlag(TextureFormat_RGB8I)
    DefineFlag(TextureFormat_RGB8U)
    DefineFlag(TextureFormat_RGB8S)
    DefineFlag(TextureFormat_RGB9E5F)
    DefineFlag(TextureFormat_BGRA8)
    DefineFlag(TextureFormat_RGBA8)
    DefineFlag(TextureFormat_RGBA8I)
    DefineFlag(TextureFormat_RGBA8U)
    DefineFlag(TextureFormat_RGBA8S)
    DefineFlag(TextureFormat_RGBA16)
    DefineFlag(TextureFormat_RGBA16I)
    DefineFlag(TextureFormat_RGBA16U)
    DefineFlag(TextureFormat_RGBA16F)
    DefineFlag(TextureFormat_RGBA16S)
    DefineFlag(TextureFormat_RGBA32I)
    DefineFlag(TextureFormat_RGBA32U)
    DefineFlag(TextureFormat_RGBA32F)
    DefineFlag(TextureFormat_R5G6B5)
    DefineFlag(TextureFormat_RGBA4)
    DefineFlag(TextureFormat_RGB5A1)
    DefineFlag(TextureFormat_RGB10A2)
    DefineFlag(TextureFormat_RG11B10F)

    DefineFlag(TextureFormat_UnknownDepth) // Depth formats below.

    DefineFlag(TextureFormat_D16)
    DefineFlag(TextureFormat_D24)
    DefineFlag(TextureFormat_D24S8)
    DefineFlag(TextureFormat_D32)
    DefineFlag(TextureFormat_D16F)
    DefineFlag(TextureFormat_D24F)
    DefineFlag(TextureFormat_D32F)
    DefineFlag(TextureFormat_D0S8)
EndEnum()

DefineEnum(TextureFlag, true)
    DefineFlag(TextureFlag_NONE) //!<

    DefineFlag(TextureFlag_U_MIRROR) //!< Wrap U mode: Mirror
    DefineFlag(TextureFlag_U_CLAMP) //!< Wrap U mode: Clamp
    DefineFlag(TextureFlag_U_BORDER) //!< Wrap U mode: Border
    DefineFlag(TextureFlag_V_MIRROR) //!< Wrap V mode: Mirror
    DefineFlag(TextureFlag_V_CLAMP) //!< Wrap V mode: Clamp
    DefineFlag(TextureFlag_V_BORDER) //!< Wrap V mode: Border
    DefineFlag(TextureFlag_W_MIRROR) //!< Wrap W mode: Mirror
    DefineFlag(TextureFlag_W_CLAMP) //!< Wrap W mode: Clamp
    DefineFlag(TextureFlag_W_BORDER) //!< Wrap W mode: Border

    DefineFlag(TextureFlag_MIN_POINT) //!< Min sampling mode: Point
    DefineFlag(TextureFlag_MIN_ANISOTROPIC) //!< Min sampling mode: Anisotropic
    DefineFlag(TextureFlag_MAG_POINT) //!< Mag sampling mode: Point
    DefineFlag(TextureFlag_MAG_ANISOTROPIC) //!< Mag sampling mode: Anisotropic
    DefineFlag(TextureFlag_MIP_POINT) //!< Mip sampling mode: Point

    DefineFlag(TextureFlag_MSAA_SAMPLE) //!< Texture will be used for MSAA sampling.

    DefineFlag(TextureFlag_RT) //!<
    DefineFlag(TextureFlag_RT_MSAA_X2) //!< Render target MSAAx2 mode.
    DefineFlag(TextureFlag_RT_MSAA_X4) //!< Render target MSAAx4 mode.
    DefineFlag(TextureFlag_RT_MSAA_X8) //!< Render target MSAAx8 mode.
    DefineFlag(TextureFlag_RT_MSAA_X16) //!< Render target MSAAx16 mode.
    DefineFlag(TextureFlag_RT_WRITE_ONLY) //!< Render target will be used for writing only.

    DefineFlag(TextureFlag_COMPARE_LESS) //!< Compare when sampling depth texture: less.
    DefineFlag(TextureFlag_COMPARE_LEQUAL) //!< Compare when sampling depth texture: less or equal.
    DefineFlag(TextureFlag_COMPARE_EQUAL) //!< Compare when sampling depth texture: equal.
    DefineFlag(TextureFlag_COMPARE_GEQUAL) //!< Compare when sampling depth texture: greater or equal.
    DefineFlag(TextureFlag_COMPARE_GREATER) //!< Compare when sampling depth texture: greater.
    DefineFlag(TextureFlag_COMPARE_NOTEQUAL) //!< Compare when sampling depth texture: not equal.
    DefineFlag(TextureFlag_COMPARE_NEVER) //!< Compare when sampling depth texture: never.
    DefineFlag(TextureFlag_COMPARE_ALWAYS) //!< Compare when sampling depth texture: always.

    DefineFlag(TextureFlag_COMPUTE_WRITE) //!< Texture will be used for compute write.

    DefineFlag(TextureFlag_SRGB) //!< Sample texture as sRGB.

    DefineFlag(TextureFlag_BLIT_DST) //!< Texture will be used as blit destination.
    DefineFlag(TextureFlag_READ_BACK) //!< Texture will be used for read back from GPU.
EndEnum()

DefineEvent(TextureReadbackInitiated, TextureReadbackInitiatedHandler)

struct Texture {
    Texture() : TextureMipLevels(1), TextureFlag(TextureFlag_NONE), TextureFormat(0), TextureDynamic(false) {}

    u32 TextureFlag;
    u16 TextureFormat;
    bool TextureDynamic;
    u8 TextureMipLevels;
    Entity TextureReadbackTarget;
};

DefineComponent(Texture)
    Dependency(Stream)
    DefinePropertyEnum(u16, TextureFormat, TextureFormat)
    DefinePropertyEnum(u32, TextureFlag, TextureFlag)
    DefinePropertyReactive(bool, TextureDynamic)
    DefinePropertyReactive(u8, TextureMipLevels)
EndComponent()

DefineComponentPropertyReactive(Texture, u32, TextureFlag)
DefineComponentPropertyReactive(Texture, u16, TextureFormat)
DefineComponentPropertyReactive(Texture, bool, TextureDynamic)
DefineComponentPropertyReactive(Texture, u8, TextureMipLevels)
DefineComponentPropertyReactive(Texture, Entity, TextureReadbackTarget)

void TextureReadback(Entity texture, TextureReadbackHandler handler) {
    FireEvent(TextureReadbackInitiated, texture, handler);
}
