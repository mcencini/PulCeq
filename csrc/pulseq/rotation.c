/**
 * @file rotation.c
 * @brief Implementation of rotation-related functions.
 */

#include "event.h"
#include <math.h>

/**
 * @brief Convert quaternion to rotation matrix
 * 
 * Converts a quaternion [w, x, y, z] to a 3x3 rotation matrix
 * stored in row-major order as a float[9] array.
 * 
 * @param[in] q Quaternion array [w, x, y, z]
 * @param[out] m Rotation matrix (3x3, flattened in row-major order as float[9])
 */
void quaternionToMatrix(const float q[4], float m[9]) {
    float w, x, y, z;
    float norm;
    
    w = q[0];
    x = q[1];
    y = q[2];
    z = q[3];
    
    /* Normalize quaternion if not already normalized */
    norm = sqrtf(w*w + x*x + y*y + z*z);
    if (norm != 1.0f && norm != 0.0f) {
        w /= norm;
        x /= norm;
        y /= norm;
        z /= norm;
    }
    
    /* Calculate rotation matrix elements */
    /* Row 1 */
    m[0] = 1.0f - 2.0f * (y*y + z*z);
    m[1] = 2.0f * (x*y - w*z);
    m[2] = 2.0f * (x*z + w*y);
    
    /* Row 2 */
    m[3] = 2.0f * (x*y + w*z);
    m[4] = 1.0f - 2.0f * (x*x + z*z);
    m[5] = 2.0f * (y*z - w*x);
    
    /* Row 3 */
    m[6] = 2.0f * (x*z - w*y);
    m[7] = 2.0f * (y*z + w*x);
    m[8] = 1.0f - 2.0f * (x*x + y*y);
}
