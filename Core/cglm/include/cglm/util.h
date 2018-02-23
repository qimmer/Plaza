/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Functions:
   CGLM_INLINE int   glm_sign(int val);
   CGLM_INLINE float glm_rad(float deg);
   CGLM_INLINE float glm_deg(float rad);
   CGLM_INLINE void  glm_make_rad(float *deg);
   CGLM_INLINE void  glm_make_deg(float *rad);
   CGLM_INLINE float glm_pow2(float x);
 */

#ifndef cglm_util_h
#define cglm_util_h

#include "common.h"

/*!
 * @brief get sign of 32 bit integer as +1 or -1
 *
 * @param val integer value
 */
CGLM_INLINE
int
glm_sign(int val) {
  return ((val >> 31) - (-val >> 31));
}

/*!
 * @brief convert degree to radians
 *
 * @param[in] deg angle in degrees
 */
CGLM_INLINE
float
glm_rad(float deg) {
  return deg * CGLM_PI / 180.0f;
}

/*!
 * @brief convert radians to degree
 *
 * @param[in] deg angle in radians
 */
CGLM_INLINE
float
glm_deg(float rad) {
  return rad * 180.0f / CGLM_PI;
}

/*!
 * @brief convert exsisting degree to radians. this will override degrees value
 *
 * @param[in, out] deg pointer to angle in degrees
 */
CGLM_INLINE
void
glm_make_rad(float *deg) {
  *deg = *deg * CGLM_PI / 180.0f;
}

/*!
 * @brief convert exsisting radians to degree. this will override radians value
 *
 * @param[in, out] rad pointer to angle in radians
 */
CGLM_INLINE
void
glm_make_deg(float *rad) {
  *rad = *rad * 180.0f / CGLM_PI;
}

/*!
 * @brief multiplies given parameter with itself = x * x or powf(x, 2)
 *
 * @param[in] x x
 */
CGLM_INLINE
float
glm_pow2(float x) {

  return x * x;
}

/*!
 * @brief find minimum of given two values
 *
 * @param[in] a number 1
 * @param[in] b number 2
 */
CGLM_INLINE
float
glm_min(float a, float b) {
  if (a < b)
    return a;
  return b;
}

/*!
 * @brief find maximum of given two values
 *
 * @param[in] a number 1
 * @param[in] b number 2
 */
CGLM_INLINE
float
glm_max(float a, float b) {
  if (a > b)
    return a;
  return b;
}

#endif /* cglm_util_h */
