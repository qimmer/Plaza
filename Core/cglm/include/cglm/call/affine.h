/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef cglmc_affine_h
#define cglmc_affine_h
#ifdef __cplusplus
extern "C" {
#endif

#include "../cglm.h"

CGLM_EXPORT
void
glmc_translate_to(mat4 m, vec3 v, mat4 dest);

CGLM_EXPORT
void
glmc_translate(mat4 m, vec3 v);

CGLM_EXPORT
void
glmc_translate_x(mat4 m, float to);

CGLM_EXPORT
void
glmc_translate_y(mat4 m, float to);

CGLM_EXPORT
void
glmc_translate_z(mat4 m, float to);

CGLM_EXPORT
void
glmc_scale_to(mat4 m, vec3 v, mat4 dest);

CGLM_EXPORT
void
glmc_scale(mat4 m, vec3 v);

CGLM_EXPORT
void
glmc_scale1(mat4 m, float s);

CGLM_EXPORT
void
glmc_rotate_x(mat4 m, float rad, mat4 dest);

CGLM_EXPORT
void
glmc_rotate_y(mat4 m, float rad, mat4 dest);

CGLM_EXPORT
void
glmc_rotate_z(mat4 m, float rad, mat4 dest);

CGLM_EXPORT
void
glmc_rotate_ndc_make(mat4 m, float angle, vec3 axis_ndc);

CGLM_EXPORT
void
glmc_rotate_make(mat4 m, float angle, vec3 axis);

CGLM_EXPORT
void
glmc_rotate_ndc(mat4 m, float angle, vec3 axis_ndc);

CGLM_EXPORT
void
glmc_rotate(mat4 m, float angle, vec3 axis);

CGLM_EXPORT
void
glmc_decompose_scalev(mat4 m, vec3 s);

CGLM_EXPORT
void
glmc_decompose_rs(mat4 m, mat4 r, vec3 s);

CGLM_EXPORT
void
glmc_decompose(mat4 m, vec4 t, mat4 r, vec3 s);

#ifdef __cplusplus
}
#endif
#endif /* cglmc_affine_h */
