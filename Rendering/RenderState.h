//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_RENDERSTATE_H
#define PLAZA_RENDERSTATE_H

#include <Core/NativeUtils.h>

Unit(RenderState)
    Enum(DepthTest)
    Enum(WriteMask)
    Enum(MultisampleMode)
    Enum(BlendMode)

    Component(RenderState)
    	Property(u64, RenderStateDepthTest)
		Property(u64, RenderStateWriteMask)
		Property(u64, RenderStateMultisampleMode)
		Property(u64, RenderStateBlendMode)

/// Color RGB/alpha/depth write. When it's not specified write will be disabled.
#define RenderState_STATE_R_WRITE                 UINT64_C(0x0000000000000001) //!< Enable R write.
#define RenderState_STATE_G_WRITE                 UINT64_C(0x0000000000000002) //!< Enable G write.
#define RenderState_STATE_B_WRITE                 UINT64_C(0x0000000000000004) //!< Enable B write.
#define RenderState_STATE_ALPHA_WRITE                 UINT64_C(0x0000000000000008) //!< Enable alpha write.
#define RenderState_STATE_DEPTH_WRITE                 UINT64_C(0x0000004000000000) //!< Enable depth write.

/// Enable RGB write.
#define RenderState_STATE_RGB_WRITE (0  \
			| RenderState_STATE_R_WRITE \
			| RenderState_STATE_G_WRITE \
			| RenderState_STATE_B_WRITE \
			)

/// Depth test state. When `RenderState_STATE_DEPTH_` is not specified depth test will be disabled.
#define RenderState_STATE_DEPTH_TEST_NONE         UINT64_C(0x0000000000000000)
#define RenderState_STATE_DEPTH_TEST_LESS         UINT64_C(0x0000000000000010) //!< Enable depth test, less.
#define RenderState_STATE_DEPTH_TEST_LEQUAL       UINT64_C(0x0000000000000020) //!< Enable depth test, less or equal.
#define RenderState_STATE_DEPTH_TEST_EQUAL        UINT64_C(0x0000000000000030) //!< Enable depth test, equal.
#define RenderState_STATE_DEPTH_TEST_GEQUAL       UINT64_C(0x0000000000000040) //!< Enable depth test, greater or equal.
#define RenderState_STATE_DEPTH_TEST_GREATER      UINT64_C(0x0000000000000050) //!< Enable depth test, greater.
#define RenderState_STATE_DEPTH_TEST_NOTEQUAL     UINT64_C(0x0000000000000060) //!< Enable depth test, not equal.
#define RenderState_STATE_DEPTH_TEST_NEVER        UINT64_C(0x0000000000000070) //!< Enable depth test, never.
#define RenderState_STATE_DEPTH_TEST_ALWAYS       UINT64_C(0x0000000000000080) //!< Enable depth test, always.
#define RenderState_STATE_DEPTH_TEST_SHIFT        4                            //!< Depth test state bit shift.
#define RenderState_STATE_DEPTH_TEST_MASK         UINT64_C(0x00000000000000f0) //!< Depth test state bit mask.

/// Use RenderState_STATE_BLEND_FUNC(_src, _dst) or RenderState_STATE_BLEND_FUNC_SEPARATE(_srcRGB, _dstRGB, _srcA, _dstA)
/// helper macros.
#define RenderState_STATE_BLEND_ZERO              UINT64_C(0x0000000000001000) //!< 0, 0, 0, 0
#define RenderState_STATE_BLEND_ONE               UINT64_C(0x0000000000002000) //!< 1, 1, 1, 1
#define RenderState_STATE_BLEND_SRC_COLOR         UINT64_C(0x0000000000003000) //!< Rs, Gs, Bs, As
#define RenderState_STATE_BLEND_INV_SRC_COLOR     UINT64_C(0x0000000000004000) //!< 1-Rs, 1-Gs, 1-Bs, 1-As
#define RenderState_STATE_BLEND_SRC_ALPHA         UINT64_C(0x0000000000005000) //!< As, As, As, As
#define RenderState_STATE_BLEND_INV_SRC_ALPHA     UINT64_C(0x0000000000006000) //!< 1-As, 1-As, 1-As, 1-As
#define RenderState_STATE_BLEND_DST_ALPHA         UINT64_C(0x0000000000007000) //!< Ad, Ad, Ad, Ad
#define RenderState_STATE_BLEND_INV_DST_ALPHA     UINT64_C(0x0000000000008000) //!< 1-Ad, 1-Ad, 1-Ad ,1-Ad
#define RenderState_STATE_BLEND_DST_COLOR         UINT64_C(0x0000000000009000) //!< Rd, Gd, Bd, Ad
#define RenderState_STATE_BLEND_INV_DST_COLOR     UINT64_C(0x000000000000a000) //!< 1-Rd, 1-Gd, 1-Bd, 1-Ad
#define RenderState_STATE_BLEND_SRC_ALPHA_SAT     UINT64_C(0x000000000000b000) //!< f, f, f, 1; f = min(As, 1-Ad)
#define RenderState_STATE_BLEND_FACTOR            UINT64_C(0x000000000000c000) //!< Blend factor
#define RenderState_STATE_BLEND_INV_FACTOR        UINT64_C(0x000000000000d000) //!< 1-Blend factor
#define RenderState_STATE_BLEND_SHIFT             12                           //!< Blend state bit shift.
#define RenderState_STATE_BLEND_MASK              UINT64_C(0x000000000ffff000) //!< Blend state bit mask.

/// Use RenderState_STATE_BLEND_EQUATION(_equation) or RenderState_STATE_BLEND_EQUATION_SEPARATE(_equationRGB, _equationA)
/// helper macros.
#define RenderState_STATE_BLEND_EQUATION_ADD      UINT64_C(0x0000000000000000) //!< Blend add: src + dst.
#define RenderState_STATE_BLEND_EQUATION_SUB      UINT64_C(0x0000000010000000) //!< Blend subtract: src - dst.
#define RenderState_STATE_BLEND_EQUATION_REVSUB   UINT64_C(0x0000000020000000) //!< Blend reverse subtract: dst - src.
#define RenderState_STATE_BLEND_EQUATION_MIN      UINT64_C(0x0000000030000000) //!< Blend min: min(src, dst).
#define RenderState_STATE_BLEND_EQUATION_MAX      UINT64_C(0x0000000040000000) //!< Blend max: max(src, dst).
#define RenderState_STATE_BLEND_EQUATION_SHIFT    28                           //!< Blend equation bit shift.
#define RenderState_STATE_BLEND_EQUATION_MASK     UINT64_C(0x00000003f0000000) //!< Blend equation bit mask.

#define RenderState_STATE_BLEND_INDEPENDENT       UINT64_C(0x0000000400000000) //!< Enable blend independent.
#define RenderState_STATE_BLEND_ALPHA_TO_COVERAGE UINT64_C(0x0000000800000000) //!< Enable alpha to coverage.


/// See RenderState_STATE_POINT_SIZE(_size) helper macro.
#define RenderState_STATE_POINT_SIZE_SHIFT        52                           //!< Point size bit shift.
#define RenderState_STATE_POINT_SIZE_MASK         UINT64_C(0x00f0000000000000) //!< Point size bit mask.

/// Enable MSAA write when writing into MSAA frame buffer.
/// This flag is ignored when not writing into MSAA frame buffer.
#define RenderState_STATE_MSAA                    UINT64_C(0x0100000000000000) //!< Enable MSAA rasterization.
#define RenderState_STATE_LINEAA                  UINT64_C(0x0200000000000000) //!< Enable line AA rasterization.
#define RenderState_STATE_CONSERVATIVE_RASTER     UINT64_C(0x0400000000000000) //!< Enable conservative rasterization.

/// Do not use!
#define RenderState_STATE_RESERVED_SHIFT          61                           //!< Internal bits shift.
#define RenderState_STATE_RESERVED_MASK           UINT64_C(0xe000000000000000) //!< Internal bits mask.

#define RenderState_STATE_NONE                    UINT64_C(0x0000000000000000) //!< No state.
#define RenderState_STATE_MASK                    UINT64_C(0xffffffffffffffff) //!< State mask.

/// Default state is write to RGB, alpha, and depth with depth test less enabled, with clockwise
/// culling and MSAA (when writing into MSAA frame buffer, otherwise this flag is ignored).
#define RenderState_STATE_DEFAULT (0            \
			| RenderState_STATE_RGB_WRITE       \
			| RenderState_STATE_ALPHA_WRITE     \
			| RenderState_STATE_DEPTH_TEST_LESS \
			| RenderState_STATE_DEPTH_WRITE     \
			| RenderState_STATE_CULL_CW         \
			| RenderState_STATE_MSAA            \
			)

/// Alpha reference value.
#define RenderState_STATE_ALPHA_REF(_ref)   ( ( (uint64_t)(_ref )<<RenderState_STATE_ALPHA_REF_SHIFT )&RenderState_STATE_ALPHA_REF_MASK)

/// Point size value.
#define RenderState_STATE_POINT_SIZE(_size) ( ( (uint64_t)(_size)<<RenderState_STATE_POINT_SIZE_SHIFT)&RenderState_STATE_POINT_SIZE_MASK)

/// Blend function separate.
#define RenderState_STATE_BLEND_FUNC_SEPARATE(_srcRGB, _dstRGB, _srcA, _dstA) (UINT64_C(0) \
			| ( ( (uint64_t)(_srcRGB)|( (uint64_t)(_dstRGB)<<4) )   )               \
			| ( ( (uint64_t)(_srcA  )|( (uint64_t)(_dstA  )<<4) )<<8)               \
			)

/// Blend equation separate.
#define RenderState_STATE_BLEND_EQUATION_SEPARATE(_equationRGB, _equationA) ( (uint64_t)(_equationRGB)|( (uint64_t)(_equationA)<<3) )

/// Blend function.
#define RenderState_STATE_BLEND_FUNC(_src, _dst)    RenderState_STATE_BLEND_FUNC_SEPARATE(_src, _dst, _src, _dst)

/// Blend equation.
#define RenderState_STATE_BLEND_EQUATION(_equation) RenderState_STATE_BLEND_EQUATION_SEPARATE(_equation, _equation)

/// Utility predefined blend modes.

#define RenderState_STATE_BLEND_NONE 0

/// Additive blending.
#define RenderState_STATE_BLEND_ADD (0                                         \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_ONE, RenderState_STATE_BLEND_ONE) \
	)

/// Alpha blend.
#define RenderState_STATE_BLEND_ALPHA (0                                                       \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_SRC_ALPHA, RenderState_STATE_BLEND_INV_SRC_ALPHA) \
	)

/// Selects darker color of blend.
#define RenderState_STATE_BLEND_DARKEN (0                                      \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_ONE, RenderState_STATE_BLEND_ONE) \
	| RenderState_STATE_BLEND_EQUATION(RenderState_STATE_BLEND_EQUATION_MIN)          \
	)

/// Selects lighter color of blend.
#define RenderState_STATE_BLEND_LIGHTEN (0                                     \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_ONE, RenderState_STATE_BLEND_ONE) \
	| RenderState_STATE_BLEND_EQUATION(RenderState_STATE_BLEND_EQUATION_MAX)          \
	)

/// Multiplies colors.
#define RenderState_STATE_BLEND_MULTIPLY (0                                           \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_DST_COLOR, RenderState_STATE_BLEND_ZERO) \
	)

/// Opaque pixels will cover the pixels directly below them without any math or algorithm applied to them.
#define RenderState_STATE_BLEND_NORMAL (0                                                \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_ONE, RenderState_STATE_BLEND_INV_SRC_ALPHA) \
	)

/// Multiplies the inverse of the blend and base colors.
#define RenderState_STATE_BLEND_SCREEN (0                                                \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_ONE, RenderState_STATE_BLEND_INV_SRC_COLOR) \
	)

/// Decreases the brightness of the base color based on the value of the blend color.
#define RenderState_STATE_BLEND_LINEAR_BURN (0                                                 \
	| RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_DST_COLOR, RenderState_STATE_BLEND_INV_DST_COLOR) \
	| RenderState_STATE_BLEND_EQUATION(RenderState_STATE_BLEND_EQUATION_SUB)                          \
	)

///
#define RenderState_STATE_BLEND_FUNC_RT_x(_src, _dst) (0               \
			| ( (uint32_t)( (_src)>>RenderState_STATE_BLEND_SHIFT)       \
			| ( (uint32_t)( (_dst)>>RenderState_STATE_BLEND_SHIFT)<<4) ) \
			)

#define RenderState_STATE_BLEND_FUNC_RT_xE(_src, _dst, _equation) (0               \
			| RenderState_STATE_BLEND_FUNC_RT_x(_src, _dst)                        \
			| ( (uint32_t)( (_equation)>>RenderState_STATE_BLEND_EQUATION_SHIFT)<<8) \
			)

#define RenderState_STATE_BLEND_FUNC_RT_1(_src, _dst)  (RenderState_STATE_BLEND_FUNC_RT_x(_src, _dst)<< 0)
#define RenderState_STATE_BLEND_FUNC_RT_2(_src, _dst)  (RenderState_STATE_BLEND_FUNC_RT_x(_src, _dst)<<11)
#define RenderState_STATE_BLEND_FUNC_RT_3(_src, _dst)  (RenderState_STATE_BLEND_FUNC_RT_x(_src, _dst)<<22)

#define RenderState_STATE_BLEND_FUNC_RT_1E(_src, _dst, _equation) (RenderState_STATE_BLEND_FUNC_RT_xE(_src, _dst, _equation)<< 0)
#define RenderState_STATE_BLEND_FUNC_RT_2E(_src, _dst, _equation) (RenderState_STATE_BLEND_FUNC_RT_xE(_src, _dst, _equation)<<11)
#define RenderState_STATE_BLEND_FUNC_RT_3E(_src, _dst, _equation) (RenderState_STATE_BLEND_FUNC_RT_xE(_src, _dst, _equation)<<22)

///
#define RenderState_STENCIL_FUNC_REF_SHIFT      0                    //!<
#define RenderState_STENCIL_FUNC_REF_MASK       UINT32_C(0x000000ff) //!<
#define RenderState_STENCIL_FUNC_RMASK_SHIFT    8                    //!<
#define RenderState_STENCIL_FUNC_RMASK_MASK     UINT32_C(0x0000ff00) //!<

#define RenderState_STENCIL_TEST_LESS           UINT32_C(0x00010000) //!< Enable stencil test, less.
#define RenderState_STENCIL_TEST_LEQUAL         UINT32_C(0x00020000) //!< Enable stencil test, less or equal.
#define RenderState_STENCIL_TEST_EQUAL          UINT32_C(0x00030000) //!< Enable stencil test, equal.
#define RenderState_STENCIL_TEST_GEQUAL         UINT32_C(0x00040000) //!< Enable stencil test, greater or equal.
#define RenderState_STENCIL_TEST_GREATER        UINT32_C(0x00050000) //!< Enable stencil test, greater.
#define RenderState_STENCIL_TEST_NOTEQUAL       UINT32_C(0x00060000) //!< Enable stencil test, not equal.
#define RenderState_STENCIL_TEST_NEVER          UINT32_C(0x00070000) //!< Enable stencil test, never.
#define RenderState_STENCIL_TEST_ALWAYS         UINT32_C(0x00080000) //!< Enable stencil test, always.
#define RenderState_STENCIL_TEST_SHIFT          16                   //!< Stencil test bit shift.
#define RenderState_STENCIL_TEST_MASK           UINT32_C(0x000f0000) //!< Stencil test bit mask.

#define RenderState_STENCIL_OP_FAIL_S_ZERO      UINT32_C(0x00000000) //!< Zero.
#define RenderState_STENCIL_OP_FAIL_S_KEEP      UINT32_C(0x00100000) //!< Keep.
#define RenderState_STENCIL_OP_FAIL_S_REPLACE   UINT32_C(0x00200000) //!< Replace.
#define RenderState_STENCIL_OP_FAIL_S_INCR      UINT32_C(0x00300000) //!< Increment and wrap.
#define RenderState_STENCIL_OP_FAIL_S_INCRSAT   UINT32_C(0x00400000) //!< Increment and clamp.
#define RenderState_STENCIL_OP_FAIL_S_DECR      UINT32_C(0x00500000) //!< Decrement and wrap.
#define RenderState_STENCIL_OP_FAIL_S_DECRSAT   UINT32_C(0x00600000) //!< Decrement and clamp.
#define RenderState_STENCIL_OP_FAIL_S_INVERT    UINT32_C(0x00700000) //!< Invert.
#define RenderState_STENCIL_OP_FAIL_S_SHIFT     20                   //!< Stencil operation fail bit shift.
#define RenderState_STENCIL_OP_FAIL_S_MASK      UINT32_C(0x00f00000) //!< Stencil operation fail bit mask.

#define RenderState_STENCIL_OP_FAIL_Z_ZERO      UINT32_C(0x00000000) //!< Zero.
#define RenderState_STENCIL_OP_FAIL_Z_KEEP      UINT32_C(0x01000000) //!< Keep.
#define RenderState_STENCIL_OP_FAIL_Z_REPLACE   UINT32_C(0x02000000) //!< Replace.
#define RenderState_STENCIL_OP_FAIL_Z_INCR      UINT32_C(0x03000000) //!< Increment and wrap.
#define RenderState_STENCIL_OP_FAIL_Z_INCRSAT   UINT32_C(0x04000000) //!< Increment and clamp.
#define RenderState_STENCIL_OP_FAIL_Z_DECR      UINT32_C(0x05000000) //!< Decrement and wrap.
#define RenderState_STENCIL_OP_FAIL_Z_DECRSAT   UINT32_C(0x06000000) //!< Decrement and clamp.
#define RenderState_STENCIL_OP_FAIL_Z_INVERT    UINT32_C(0x07000000) //!< Invert.
#define RenderState_STENCIL_OP_FAIL_Z_SHIFT     24                   //!< Stencil operation depth fail bit shift
#define RenderState_STENCIL_OP_FAIL_Z_MASK      UINT32_C(0x0f000000) //!< Stencil operation depth fail bit mask.

#define RenderState_STENCIL_OP_PASS_Z_ZERO      UINT32_C(0x00000000) //!< Zero.
#define RenderState_STENCIL_OP_PASS_Z_KEEP      UINT32_C(0x10000000) //!< Keep.
#define RenderState_STENCIL_OP_PASS_Z_REPLACE   UINT32_C(0x20000000) //!< Replace.
#define RenderState_STENCIL_OP_PASS_Z_INCR      UINT32_C(0x30000000) //!< Increment and wrap.
#define RenderState_STENCIL_OP_PASS_Z_INCRSAT   UINT32_C(0x40000000) //!< Increment and clamp.
#define RenderState_STENCIL_OP_PASS_Z_DECR      UINT32_C(0x50000000) //!< Decrement and wrap.
#define RenderState_STENCIL_OP_PASS_Z_DECRSAT   UINT32_C(0x60000000) //!< Decrement and clamp.
#define RenderState_STENCIL_OP_PASS_Z_INVERT    UINT32_C(0x70000000) //!< Invert.
#define RenderState_STENCIL_OP_PASS_Z_SHIFT     28                   //!< Stencil operation depth pass bit shift
#define RenderState_STENCIL_OP_PASS_Z_MASK      UINT32_C(0xf0000000) //!< Stencil operation depth pass bit mask.

#define RenderState_STENCIL_NONE                UINT32_C(0x00000000) //!<
#define RenderState_STENCIL_MASK                UINT32_C(0xffffffff) //!<
#define RenderState_STENCIL_DEFAULT             UINT32_C(0x00000000) //!<

/// Set stencil ref value.
#define RenderState_STENCIL_FUNC_REF(_ref) ( ((uint32_t)(_ref)<<RenderState_STENCIL_FUNC_REF_SHIFT)&RenderState_STENCIL_FUNC_REF_MASK)

/// Set stencil rmask value.
#define RenderState_STENCIL_FUNC_RMASK(_mask) ( ((uint32_t)(_mask)<<RenderState_STENCIL_FUNC_RMASK_SHIFT)&RenderState_STENCIL_FUNC_RMASK_MASK)


#endif //PLAZA_RENDERSTATE_H
