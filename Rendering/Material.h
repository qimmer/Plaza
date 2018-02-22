//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MATERIAL_H
#define PLAZA_MATERIAL_H


#include <Core/Entity.h>


    DeclareComponent(Material)

    DeclareComponentProperty(Material, Entity, MaterialProgram)
    DeclareComponentProperty(Material, u64, MaterialRenderState)


/// Color RGB/alpha/depth write. When it's not specified write will be disabled.
#define RenderState_RGB_WRITE               UINT64_C(0x0000000000000001) //!< Enable RGB write.
#define RenderState_ALPHA_WRITE             UINT64_C(0x0000000000000002) //!< Enable alpha write.
#define RenderState_DEPTH_WRITE             UINT64_C(0x0000000000000004) //!< Enable depth write.

/// Depth test state. When `RenderState_DEPTH_` is not specified depth test will be disabled.
#define RenderState_DEPTH_TEST_LESS         UINT64_C(0x0000000000000010) //!< Enable depth test, less.
#define RenderState_DEPTH_TEST_LEQUAL       UINT64_C(0x0000000000000020) //!< Enable depth test, less or equal.
#define RenderState_DEPTH_TEST_EQUAL        UINT64_C(0x0000000000000030) //!< Enable depth test, equal.
#define RenderState_DEPTH_TEST_GEQUAL       UINT64_C(0x0000000000000040) //!< Enable depth test, greater or equal.
#define RenderState_DEPTH_TEST_GREATER      UINT64_C(0x0000000000000050) //!< Enable depth test, greater.
#define RenderState_DEPTH_TEST_NOTEQUAL     UINT64_C(0x0000000000000060) //!< Enable depth test, not equal.
#define RenderState_DEPTH_TEST_NEVER        UINT64_C(0x0000000000000070) //!< Enable depth test, never.
#define RenderState_DEPTH_TEST_ALWAYS       UINT64_C(0x0000000000000080) //!< Enable depth test, always.
#define RenderState_DEPTH_TEST_SHIFT        4                            //!< Depth test state bit shift.
#define RenderState_DEPTH_TEST_MASK         UINT64_C(0x00000000000000f0) //!< Depth test state bit mask.

/// Use RenderState_BLEND_FUNC(_src, _dst) or RenderState_BLEND_FUNC_SEPARATE(_srcRGB, _dstRGB, _srcA, _dstA)
/// helper macros.
#define RenderState_BLEND_ZERO              UINT64_C(0x0000000000001000) //!< 0, 0, 0, 0
#define RenderState_BLEND_ONE               UINT64_C(0x0000000000002000) //!< 1, 1, 1, 1
#define RenderState_BLEND_SRC_COLOR         UINT64_C(0x0000000000003000) //!< Rs, Gs, Bs, As
#define RenderState_BLEND_INV_SRC_COLOR     UINT64_C(0x0000000000004000) //!< 1-Rs, 1-Gs, 1-Bs, 1-As
#define RenderState_BLEND_SRC_ALPHA         UINT64_C(0x0000000000005000) //!< As, As, As, As
#define RenderState_BLEND_INV_SRC_ALPHA     UINT64_C(0x0000000000006000) //!< 1-As, 1-As, 1-As, 1-As
#define RenderState_BLEND_DST_ALPHA         UINT64_C(0x0000000000007000) //!< Ad, Ad, Ad, Ad
#define RenderState_BLEND_INV_DST_ALPHA     UINT64_C(0x0000000000008000) //!< 1-Ad, 1-Ad, 1-Ad ,1-Ad
#define RenderState_BLEND_DST_COLOR         UINT64_C(0x0000000000009000) //!< Rd, Gd, Bd, Ad
#define RenderState_BLEND_INV_DST_COLOR     UINT64_C(0x000000000000a000) //!< 1-Rd, 1-Gd, 1-Bd, 1-Ad
#define RenderState_BLEND_SRC_ALPHA_SAT     UINT64_C(0x000000000000b000) //!< f, f, f, 1; f = min(As, 1-Ad)
#define RenderState_BLEND_FACTOR            UINT64_C(0x000000000000c000) //!< Blend factor
#define RenderState_BLEND_INV_FACTOR        UINT64_C(0x000000000000d000) //!< 1-Blend factor
#define RenderState_BLEND_SHIFT             12                           //!< Blend state bit shift.
#define RenderState_BLEND_MASK              UINT64_C(0x000000000ffff000) //!< Blend state bit mask.

/// Use RenderState_BLEND_EQUATION(_equation) or RenderState_BLEND_EQUATION_SEPARATE(_equationRGB, _equationA)
/// helper macros.
#define RenderState_BLEND_EQUATION_ADD      UINT64_C(0x0000000000000000) //!< Blend add: src + dst.
#define RenderState_BLEND_EQUATION_SUB      UINT64_C(0x0000000010000000) //!< Blend subtract: src - dst.
#define RenderState_BLEND_EQUATION_REVSUB   UINT64_C(0x0000000020000000) //!< Blend reverse subtract: dst - src.
#define RenderState_BLEND_EQUATION_MIN      UINT64_C(0x0000000030000000) //!< Blend min: min(src, dst).
#define RenderState_BLEND_EQUATION_MAX      UINT64_C(0x0000000040000000) //!< Blend max: max(src, dst).
#define RenderState_BLEND_EQUATION_SHIFT    28                           //!< Blend equation bit shift.
#define RenderState_BLEND_EQUATION_MASK     UINT64_C(0x00000003f0000000) //!< Blend equation bit mask.

#define RenderState_BLEND_INDEPENDENT       UINT64_C(0x0000000400000000) //!< Enable blend independent.
#define RenderState_BLEND_ALPHA_TO_COVERAGE UINT64_C(0x0000000800000000) //!< Enable alpha to coverage.

/// Cull state. When `RenderState_CULL_*` is not specified culling will be disabled.
#define RenderState_CULL_CW                 UINT64_C(0x0000001000000000) //!< Cull clockwise triangles.
#define RenderState_CULL_CCW                UINT64_C(0x0000002000000000) //!< Cull counter-clockwise triangles.
#define RenderState_CULL_SHIFT              36                           //!< Culling mode bit shift.
#define RenderState_CULL_MASK               UINT64_C(0x0000003000000000) //!< Culling mode bit mask.

/// See RenderState_ALPHA_REF(_ref) helper macro.
#define RenderState_ALPHA_REF_SHIFT         40                           //!< Alpha reference bit shift.
#define RenderState_ALPHA_REF_MASK          UINT64_C(0x0000ff0000000000) //!< Alpha reference bit mask.

/// See RenderState_POINT_SIZE(_size) helper macro.
#define RenderState_POINT_SIZE_SHIFT        52                           //!< Point size bit shift.
#define RenderState_POINT_SIZE_MASK         UINT64_C(0x00f0000000000000) //!< Point size bit mask.

/// Enable MSAA write when writing into MSAA frame buffer.
/// This flag is ignored when not writing into MSAA frame buffer.
#define RenderState_MSAA                    UINT64_C(0x0100000000000000) //!< Enable MSAA rasterization.
#define RenderState_LINEAA                  UINT64_C(0x0200000000000000) //!< Enable line AA rasterization.
#define RenderState_CONSERVATIVE_RASTER     UINT64_C(0x0400000000000000) //!< Enable conservative rasterization.


/// Default state is write to RGB, alpha, and depth with depth test less enabled, with clockwise
/// culling and MSAA (when writing into MSAA frame buffer, otherwise this flag is ignored).
#define RenderState_DEFAULT (0            \
			| RenderState_RGB_WRITE       \
			| RenderState_ALPHA_WRITE     \
			| RenderState_DEPTH_TEST_LESS \
			| RenderState_DEPTH_WRITE     \
			| RenderState_CULL_CW         \
			| RenderState_MSAA            \
			)

/// Alpha reference value.
#define RenderState_ALPHA_REF(_ref)   ( ( (uint64_t)(_ref )<<RenderState_ALPHA_REF_SHIFT )&RenderState_ALPHA_REF_MASK)

/// Point size value.
#define RenderState_POINT_SIZE(_size) ( ( (uint64_t)(_size)<<RenderState_POINT_SIZE_SHIFT)&RenderState_POINT_SIZE_MASK)

/// Blend function separate.
#define RenderState_BLEND_FUNC_SEPARATE(_srcRGB, _dstRGB, _srcA, _dstA) (UINT64_C(0) \
			| ( ( (uint64_t)(_srcRGB)|( (uint64_t)(_dstRGB)<<4) )   )               \
			| ( ( (uint64_t)(_srcA  )|( (uint64_t)(_dstA  )<<4) )<<8)               \
			)

/// Blend equation separate.
#define RenderState_BLEND_EQUATION_SEPARATE(_equationRGB, _equationA) ( (uint64_t)(_equationRGB)|( (uint64_t)(_equationA)<<3) )

/// Blend function.
#define RenderState_BLEND_FUNC(_src, _dst)    RenderState_BLEND_FUNC_SEPARATE(_src, _dst, _src, _dst)

/// Blend equation.
#define RenderState_BLEND_EQUATION(_equation) RenderState_BLEND_EQUATION_SEPARATE(_equation, _equation)

/// Utility predefined blend modes.

/// Additive blending.
#define RenderState_BLEND_ADD (0                                         \
	| RenderState_BLEND_FUNC(RenderState_BLEND_ONE, RenderState_BLEND_ONE) \
	)

/// Alpha blend.
#define RenderState_BLEND_ALPHA (0                                                       \
	| RenderState_BLEND_FUNC(RenderState_BLEND_SRC_ALPHA, RenderState_BLEND_INV_SRC_ALPHA) \
	)

/// Selects darker color of blend.
#define RenderState_BLEND_DARKEN (0                                      \
	| RenderState_BLEND_FUNC(RenderState_BLEND_ONE, RenderState_BLEND_ONE) \
	| RenderState_BLEND_EQUATION(RenderState_BLEND_EQUATION_MIN)          \
	)

/// Selects lighter color of blend.
#define RenderState_BLEND_LIGHTEN (0                                     \
	| RenderState_BLEND_FUNC(RenderState_BLEND_ONE, RenderState_BLEND_ONE) \
	| RenderState_BLEND_EQUATION(RenderState_BLEND_EQUATION_MAX)          \
	)

/// Multiplies colors.
#define RenderState_BLEND_MULTIPLY (0                                           \
	| RenderState_BLEND_FUNC(RenderState_BLEND_DST_COLOR, RenderState_BLEND_ZERO) \
	)

/// Opaque pixels will cover the pixels directly below them without any math or algorithm applied to them.
#define RenderState_BLEND_NORMAL (0                                                \
	| RenderState_BLEND_FUNC(RenderState_BLEND_ONE, RenderState_BLEND_INV_SRC_ALPHA) \
	)

/// Multiplies the inverse of the blend and base colors.
#define RenderState_BLEND_SCREEN (0                                                \
	| RenderState_BLEND_FUNC(RenderState_BLEND_ONE, RenderState_BLEND_INV_SRC_COLOR) \
	)

/// Decreases the brightness of the base color based on the value of the blend color.
#define RenderState_BLEND_LINEAR_BURN (0                                                 \
	| RenderState_BLEND_FUNC(RenderState_BLEND_DST_COLOR, RenderState_BLEND_INV_DST_COLOR) \
	| RenderState_BLEND_EQUATION(RenderState_BLEND_EQUATION_SUB)                          \
	)

///
#define RenderState_BLEND_FUNC_RT_x(_src, _dst) (0               \
			| ( (uint32_t)( (_src)>>RenderState_BLEND_SHIFT)       \
			| ( (uint32_t)( (_dst)>>RenderState_BLEND_SHIFT)<<4) ) \
			)

#define RenderState_BLEND_FUNC_RT_xE(_src, _dst, _equation) (0               \
			| RenderState_BLEND_FUNC_RT_x(_src, _dst)                        \
			| ( (uint32_t)( (_equation)>>RenderState_BLEND_EQUATION_SHIFT)<<8) \
			)

#define RenderState_BLEND_FUNC_RT_1(_src, _dst)  (RenderState_BLEND_FUNC_RT_x(_src, _dst)<< 0)
#define RenderState_BLEND_FUNC_RT_2(_src, _dst)  (RenderState_BLEND_FUNC_RT_x(_src, _dst)<<11)
#define RenderState_BLEND_FUNC_RT_3(_src, _dst)  (RenderState_BLEND_FUNC_RT_x(_src, _dst)<<22)

#define RenderState_BLEND_FUNC_RT_1E(_src, _dst, _equation) (RenderState_BLEND_FUNC_RT_xE(_src, _dst, _equation)<< 0)
#define RenderState_BLEND_FUNC_RT_2E(_src, _dst, _equation) (RenderState_BLEND_FUNC_RT_xE(_src, _dst, _equation)<<11)
#define RenderState_BLEND_FUNC_RT_3E(_src, _dst, _equation) (RenderState_BLEND_FUNC_RT_xE(_src, _dst, _equation)<<22)

#endif //PLAZA_MATERIAL_H
