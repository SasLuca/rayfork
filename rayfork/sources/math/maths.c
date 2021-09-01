#include "rayfork/math/maths.h"

rf_extern float rf_next_pot(float it)
{
    return powf(2, ceilf(logf(it) / logf(2)));
}

rf_extern rf_vec2 rf_center_to_object(rf_sizef center_this, rf_rec to_this)
{
    rf_vec2 result = { to_this.x + to_this.width / 2 - center_this.width / 2, to_this.y + to_this.height / 2 - center_this.height / 2 };
    return result;
}

rf_extern float rf_clamp(float value, float min, float max)
{
    const float res = value < min ? min : value;
    return res > max ? max : res;
}

rf_extern float rf_lerp(float start, float end, float amount)
{
    return start + amount * (end - start);
}

// Add two vectors (v1 + v2)
rf_extern rf_vec2 rf_vec2_add(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x + v2.x, v1.y + v2.y};
    return result;
}

// Subtract two vectors (v1 - v2)
rf_extern rf_vec2 rf_vec2_sub(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x - v2.x, v1.y - v2.y};
    return result;
}

// Calculate vector length
rf_extern float rf_vec2_len(rf_vec2 v)
{
    float result = sqrtf((v.x * v.x) + (v.y * v.y));
    return result;
}

// Calculate two vectors dot product
rf_extern float rf_vec2_dot_product(rf_vec2 v1, rf_vec2 v2)
{
    float result = (v1.x * v2.x + v1.y * v2.y);
    return result;
}

// Calculate distance between two vectors
rf_extern float rf_vec2_distance(rf_vec2 v1, rf_vec2 v2)
{
    float result = sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
    return result;
}

// Calculate angle from two vectors in X-axis
rf_extern float rf_vec2_angle(rf_vec2 v1, rf_vec2 v2)
{
    float result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / rf_pi);
    if (result < 0) result += 360.0f;
    return result;
}

// Scale vector (multiply by value)
rf_extern rf_vec2 rf_vec2_scale(rf_vec2 v, float scale)
{
    rf_vec2 result = {v.x * scale, v.y * scale};
    return result;
}

// Multiply vector by vector
rf_extern rf_vec2 rf_vec2_mul_v(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x * v2.x, v1.y * v2.y};
    return result;
}

// Negate vector
rf_extern rf_vec2 rf_vec2_negate(rf_vec2 v)
{
    rf_vec2 result = {-v.x, -v.y};
    return result;
}

// Divide vector by a float value
rf_extern rf_vec2 rf_vec2_div(rf_vec2 v, float div)
{
    rf_vec2 result = {v.x / div, v.y / div};
    return result;
}

// Divide vector by vector
rf_extern rf_vec2 rf_vec2_div_v(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x / v2.x, v1.y / v2.y};
    return result;
}

// Normalize provided vector
rf_extern rf_vec2 rf_vec2_normalize(rf_vec2 v)
{
    rf_vec2 result = rf_vec2_div(v, rf_vec2_len(v));
    return result;
}

// Calculate linear interpolation between two vectors
rf_extern rf_vec2 rf_vec2_lerp(rf_vec2 v1, rf_vec2 v2, float amount)
{
    rf_vec2 result = {0};

    result.x = v1.x + amount * (v2.x - v1.x);
    result.y = v1.y + amount * (v2.y - v1.y);

    return result;
}

// Add two vectors
rf_extern rf_vec3 rf_vec3_add(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    return result;
}

// Subtract two vectors
rf_extern rf_vec3 rf_vec3_sub(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return result;
}

// Multiply vector by scalar
rf_extern rf_vec3 rf_vec3_mul(rf_vec3 v, float scalar)
{
    rf_vec3 result = {v.x * scalar, v.y * scalar, v.z * scalar};
    return result;
}

// Multiply vector by vector
rf_extern rf_vec3 rf_vec3_mul_v(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
    return result;
}

// Calculate two vectors cross product
rf_extern rf_vec3 rf_vec3_cross_product(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    return result;
}

// Calculate one vector perpendicular vector
rf_extern rf_vec3 rf_vec3_perpendicular(rf_vec3 v)
{
    rf_vec3 result = {0};

    float min = (float) fabsf(v.x);
    rf_vec3 cardinalAxis = {1.0f, 0.0f, 0.0f};

    if (fabsf(v.y) < min)
    {
        min = (float) fabsf(v.y);
        rf_vec3 tmp = {0.0f, 1.0f, 0.0f};
        cardinalAxis = tmp;
    }

    if (fabsf(v.z) < min)
    {
        rf_vec3 tmp = {0.0f, 0.0f, 1.0f};
        cardinalAxis = tmp;
    }

    result = rf_vec3_cross_product(v, cardinalAxis);

    return result;
}

// Calculate vector length
rf_extern float rf_vec3_len(rf_vec3 v)
{
    float result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return result;
}

// Calculate two vectors dot product
rf_extern float rf_vec3_dot_product(rf_vec3 v1, rf_vec3 v2)
{
    float result = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    return result;
}

// Calculate distance between two vectors
rf_extern float rf_vec3_distance(rf_vec3 v1, rf_vec3 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    float result = sqrtf(dx * dx + dy * dy + dz * dz);
    return result;
}

// Scale provided vector
rf_extern rf_vec3 rf_vec3_scale(rf_vec3 v, float scale)
{
    rf_vec3 result = {v.x * scale, v.y * scale, v.z * scale};
    return result;
}

// Negate provided vector (invert direction)
rf_extern rf_vec3 rf_vec3_negate(rf_vec3 v)
{
    rf_vec3 result = {-v.x, -v.y, -v.z};
    return result;
}

// Divide vector by a float value
rf_extern rf_vec3 rf_vec3_div(rf_vec3 v, float div)
{
    rf_vec3 result = {v.x / div, v.y / div, v.z / div};
    return result;
}

// Divide vector by vector
rf_extern rf_vec3 rf_vec3_div_v(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
    return result;
}

// Normalize provided vector
rf_extern rf_vec3 rf_vec3_normalize(rf_vec3 v)
{
    rf_vec3 result = v;

    float length, ilength;
    length = rf_vec3_len(v);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f / length;

    result.x *= ilength;
    result.y *= ilength;
    result.z *= ilength;

    return result;
}

// Orthonormalize provided vectors
// Makes vectors normalized and orthogonal to each other
// Gram-Schmidt function implementation
rf_extern void rf_vec3_ortho_normalize(rf_vec3 *v1, rf_vec3 *v2)
{
    *v1 = rf_vec3_normalize(*v1);
    rf_vec3 vn = rf_vec3_cross_product(*v1, *v2);
    vn = rf_vec3_normalize(vn);
    *v2 = rf_vec3_cross_product(vn, *v1);
}

// Transforms a rf_vec3 by a given rf_mat
rf_extern rf_vec3 rf_vec3_transform(rf_vec3 v, rf_mat mat)
{
    rf_vec3 result = {0};
    float x = v.x;
    float y = v.y;
    float z = v.z;

    result.x = mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12;
    result.y = mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13;
    result.z = mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14;

    return result;
}

// rf_transform a vector by quaternion rotation
rf_extern rf_vec3 rf_vec3_rotate_by_quaternion(rf_vec3 v, rf_quaternion q)
{
    rf_vec3 result = {0};

    result.x = v.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) + v.y * (2 * q.x * q.y - 2 * q.w * q.z) +
               v.z * (2 * q.x * q.z + 2 * q.w * q.y);
    result.y = v.x * (2 * q.w * q.z + 2 * q.x * q.y) + v.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) +
               v.z * (-2 * q.w * q.x + 2 * q.y * q.z);
    result.z = v.x * (-2 * q.w * q.y + 2 * q.x * q.z) + v.y * (2 * q.w * q.x + 2 * q.y * q.z) +
               v.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);

    return result;
}

// Calculate linear interpolation between two vectors
rf_extern rf_vec3 rf_vec3_lerp(rf_vec3 v1, rf_vec3 v2, float amount)
{
    rf_vec3 result = {0};

    result.x = v1.x + amount * (v2.x - v1.x);
    result.y = v1.y + amount * (v2.y - v1.y);
    result.z = v1.z + amount * (v2.z - v1.z);

    return result;
}

// Calculate reflected vector to normal
rf_extern rf_vec3 rf_vec3_reflect(rf_vec3 v, rf_vec3 normal)
{
// I is the original vector
// N is the normal of the incident plane
// R = I - (2*N*( DotProduct[ I,N] ))

    rf_vec3 result = {0};

    float dotProduct = rf_vec3_dot_product(v, normal);

    result.x = v.x - (2.0f * normal.x) * dotProduct;
    result.y = v.y - (2.0f * normal.y) * dotProduct;
    result.z = v.z - (2.0f * normal.z) * dotProduct;

    return result;
}

// Return min value for each pair of components
rf_extern rf_vec3 rf_vec3_min(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {0};

    result.x = fminf(v1.x, v2.x);
    result.y = fminf(v1.y, v2.y);
    result.z = fminf(v1.z, v2.z);

    return result;
}

// Return max value for each pair of components
rf_extern rf_vec3 rf_vec3_max(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {0};

    result.x = fmaxf(v1.x, v2.x);
    result.y = fmaxf(v1.y, v2.y);
    result.z = fmaxf(v1.z, v2.z);

    return result;
}

// Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c)
// NOTE: Assumes P is on the plane of the triangle
rf_extern rf_vec3 rf_vec3_barycenter(rf_vec3 p, rf_vec3 a, rf_vec3 b, rf_vec3 c)
{
//Vector v0 = b - a, v1 = c - a, v2 = p - a;

    rf_vec3 v0 = rf_vec3_sub(b, a);
    rf_vec3 v1 = rf_vec3_sub(c, a);
    rf_vec3 v2 = rf_vec3_sub(p, a);
    float d00 = rf_vec3_dot_product(v0, v0);
    float d01 = rf_vec3_dot_product(v0, v1);
    float d11 = rf_vec3_dot_product(v1, v1);
    float d20 = rf_vec3_dot_product(v2, v0);
    float d21 = rf_vec3_dot_product(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    rf_vec3 result = {0};

    result.y = (d11 * d20 - d01 * d21) / denom;
    result.z = (d00 * d21 - d01 * d20) / denom;
    result.x = 1.0f - (result.z + result.y);

    return result;
}

// Compute matrix determinant
rf_extern float rf_mat_determinant(rf_mat mat)
{
    float result = 0.0f;

    // Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    result = a30 * a21 * a12 * a03 - a20 * a31 * a12 * a03 - a30 * a11 * a22 * a03 + a10 * a31 * a22 * a03 +
             a20 * a11 * a32 * a03 - a10 * a21 * a32 * a03 - a30 * a21 * a02 * a13 + a20 * a31 * a02 * a13 +
             a30 * a01 * a22 * a13 - a00 * a31 * a22 * a13 - a20 * a01 * a32 * a13 + a00 * a21 * a32 * a13 +
             a30 * a11 * a02 * a23 - a10 * a31 * a02 * a23 - a30 * a01 * a12 * a23 + a00 * a31 * a12 * a23 +
             a10 * a01 * a32 * a23 - a00 * a11 * a32 * a23 - a20 * a11 * a02 * a33 + a10 * a21 * a02 * a33 +
             a20 * a01 * a12 * a33 - a00 * a21 * a12 * a33 - a10 * a01 * a22 * a33 + a00 * a11 * a22 * a33;

    return result;
}

// Returns the trace of the matrix (sum of the values along the diagonal)
rf_extern float rf_mat_trace(rf_mat mat)
{
    float result = (mat.m0 + mat.m5 + mat.m10 + mat.m15);
    return result;
}

// Transposes provided matrix
rf_extern rf_mat rf_mat_transpose(rf_mat mat)
{
    rf_mat result = {0};

    result.m0 = mat.m0;
    result.m1 = mat.m4;
    result.m2 = mat.m8;
    result.m3 = mat.m12;
    result.m4 = mat.m1;
    result.m5 = mat.m5;
    result.m6 = mat.m9;
    result.m7 = mat.m13;
    result.m8 = mat.m2;
    result.m9 = mat.m6;
    result.m10 = mat.m10;
    result.m11 = mat.m14;
    result.m12 = mat.m3;
    result.m13 = mat.m7;
    result.m14 = mat.m11;
    result.m15 = mat.m15;

    return result;
}

// Invert provided matrix
rf_extern rf_mat rf_mat_invert(rf_mat mat)
{
    rf_mat result = {0};

// Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

// Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f / (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

    result.m0 = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.m1 = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.m2 = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.m3 = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.m4 = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.m5 = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.m6 = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.m7 = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.m8 = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.m9 = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.m10 = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.m11 = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.m12 = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.m13 = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.m14 = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.m15 = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return result;
}

// Normalize provided matrix
rf_extern rf_mat rf_mat_normalize(rf_mat mat)
{
    rf_mat result = {0};

    float det = rf_mat_determinant(mat);

    result.m0 = mat.m0 / det;
    result.m1 = mat.m1 / det;
    result.m2 = mat.m2 / det;
    result.m3 = mat.m3 / det;
    result.m4 = mat.m4 / det;
    result.m5 = mat.m5 / det;
    result.m6 = mat.m6 / det;
    result.m7 = mat.m7 / det;
    result.m8 = mat.m8 / det;
    result.m9 = mat.m9 / det;
    result.m10 = mat.m10 / det;
    result.m11 = mat.m11 / det;
    result.m12 = mat.m12 / det;
    result.m13 = mat.m13 / det;
    result.m14 = mat.m14 / det;
    result.m15 = mat.m15 / det;

    return result;
}

// Returns identity matrix
rf_extern rf_mat rf_mat_identity(void)
{
    rf_mat result = {1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

// Add two matrices
rf_extern rf_mat rf_mat_add(rf_mat left, rf_mat right)
{
    rf_mat result = rf_mat_identity();

    result.m0 = left.m0 + right.m0;
    result.m1 = left.m1 + right.m1;
    result.m2 = left.m2 + right.m2;
    result.m3 = left.m3 + right.m3;
    result.m4 = left.m4 + right.m4;
    result.m5 = left.m5 + right.m5;
    result.m6 = left.m6 + right.m6;
    result.m7 = left.m7 + right.m7;
    result.m8 = left.m8 + right.m8;
    result.m9 = left.m9 + right.m9;
    result.m10 = left.m10 + right.m10;
    result.m11 = left.m11 + right.m11;
    result.m12 = left.m12 + right.m12;
    result.m13 = left.m13 + right.m13;
    result.m14 = left.m14 + right.m14;
    result.m15 = left.m15 + right.m15;

    return result;
}

// Subtract two matrices (left - right)
rf_extern rf_mat rf_mat_sub(rf_mat left, rf_mat right)
{
    rf_mat result = rf_mat_identity();

    result.m0 = left.m0 - right.m0;
    result.m1 = left.m1 - right.m1;
    result.m2 = left.m2 - right.m2;
    result.m3 = left.m3 - right.m3;
    result.m4 = left.m4 - right.m4;
    result.m5 = left.m5 - right.m5;
    result.m6 = left.m6 - right.m6;
    result.m7 = left.m7 - right.m7;
    result.m8 = left.m8 - right.m8;
    result.m9 = left.m9 - right.m9;
    result.m10 = left.m10 - right.m10;
    result.m11 = left.m11 - right.m11;
    result.m12 = left.m12 - right.m12;
    result.m13 = left.m13 - right.m13;
    result.m14 = left.m14 - right.m14;
    result.m15 = left.m15 - right.m15;

    return result;
}

// Returns translation matrix
rf_extern rf_mat rf_mat_translate(float x, float y, float z)
{
    rf_mat result = {1.0f, 0.0f, 0.0f, x,
                     0.0f, 1.0f, 0.0f, y,
                     0.0f, 0.0f, 1.0f, z,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

// Create rotation matrix from axis and angle
// NOTE: Angle should be provided in radians
rf_extern rf_mat rf_mat_rotate(rf_vec3 axis, float angle)
{
    rf_mat result = {0};

    float x = axis.x, y = axis.y, z = axis.z;

    float length = sqrtf(x * x + y * y + z * z);

    if ((length != 1.0f) && (length != 0.0f))
    {
        length = 1.0f / length;
        x *= length;
        y *= length;
        z *= length;
    }

    float sinres = sinf(angle);
    float cosres = cosf(angle);
    float t = 1.0f - cosres;

    result.m0 = x * x * t + cosres;
    result.m1 = y * x * t + z * sinres;
    result.m2 = z * x * t - y * sinres;
    result.m3 = 0.0f;

    result.m4 = x * y * t - z * sinres;
    result.m5 = y * y * t + cosres;
    result.m6 = z * y * t + x * sinres;
    result.m7 = 0.0f;

    result.m8 = x * z * t + y * sinres;
    result.m9 = y * z * t - x * sinres;
    result.m10 = z * z * t + cosres;
    result.m11 = 0.0f;

    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

// Returns xyz-rotation matrix (angles in radians)
rf_extern rf_mat rf_mat_rotate_xyz(rf_vec3 ang)
{
    rf_mat result = rf_mat_identity();

    float cosz = cosf(-ang.z);
    float sinz = sinf(-ang.z);
    float cosy = cosf(-ang.y);
    float siny = sinf(-ang.y);
    float cosx = cosf(-ang.x);
    float sinx = sinf(-ang.x);

    result.m0 = cosz * cosy;
    result.m4 = (cosz * siny * sinx) - (sinz * cosx);
    result.m8 = (cosz * siny * cosx) + (sinz * sinx);

    result.m1 = sinz * cosy;
    result.m5 = (sinz * siny * sinx) + (cosz * cosx);
    result.m9 = (sinz * siny * cosx) - (cosz * sinx);

    result.m2 = -siny;
    result.m6 = cosy * sinx;
    result.m10 = cosy * cosx;

    return result;
}

// Returns x-rotation matrix (angle in radians)
rf_extern rf_mat rf_mat_rotate_x(float angle)
{
    rf_mat result = rf_mat_identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m5 = cosres;
    result.m6 = -sinres;
    result.m9 = sinres;
    result.m10 = cosres;

    return result;
}

// Returns y-rotation matrix (angle in radians)
rf_extern rf_mat rf_mat_rotate_y(float angle)
{
    rf_mat result = rf_mat_identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m0 = cosres;
    result.m2 = sinres;
    result.m8 = -sinres;
    result.m10 = cosres;

    return result;
}

// Returns z-rotation matrix (angle in radians)
rf_extern rf_mat rf_mat_rotate_z(float angle)
{
    rf_mat result = rf_mat_identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m0 = cosres;
    result.m1 = -sinres;
    result.m4 = sinres;
    result.m5 = cosres;

    return result;
}

// Returns scaling matrix
rf_extern rf_mat rf_mat_scale(float x, float y, float z)
{
    rf_mat result = {x, 0.0f, 0.0f, 0.0f,
                     0.0f, y, 0.0f, 0.0f,
                     0.0f, 0.0f, z, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

// Returns two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
rf_extern rf_mat rf_mat_mul(rf_mat left, rf_mat right)
{
    rf_mat result = {0};

    result.m0 = left.m0 * right.m0 + left.m1 * right.m4 + left.m2 * right.m8 + left.m3 * right.m12;
    result.m1 = left.m0 * right.m1 + left.m1 * right.m5 + left.m2 * right.m9 + left.m3 * right.m13;
    result.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m10 + left.m3 * right.m14;
    result.m3 = left.m0 * right.m3 + left.m1 * right.m7 + left.m2 * right.m11 + left.m3 * right.m15;
    result.m4 = left.m4 * right.m0 + left.m5 * right.m4 + left.m6 * right.m8 + left.m7 * right.m12;
    result.m5 = left.m4 * right.m1 + left.m5 * right.m5 + left.m6 * right.m9 + left.m7 * right.m13;
    result.m6 = left.m4 * right.m2 + left.m5 * right.m6 + left.m6 * right.m10 + left.m7 * right.m14;
    result.m7 = left.m4 * right.m3 + left.m5 * right.m7 + left.m6 * right.m11 + left.m7 * right.m15;
    result.m8 = left.m8 * right.m0 + left.m9 * right.m4 + left.m10 * right.m8 + left.m11 * right.m12;
    result.m9 = left.m8 * right.m1 + left.m9 * right.m5 + left.m10 * right.m9 + left.m11 * right.m13;
    result.m10 = left.m8 * right.m2 + left.m9 * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
    result.m11 = left.m8 * right.m3 + left.m9 * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
    result.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8 + left.m15 * right.m12;
    result.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9 + left.m15 * right.m13;
    result.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
    result.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;

    return result;
}

// Returns perspective GL_PROJECTION matrix
rf_extern rf_mat rf_mat_frustum(double left, double right, double bottom, double top, double near_val, double far_val)
{
    rf_mat result = {0};

    float rl = (float) (right - left);
    float tb = (float) (top - bottom);
    float fn = (float) (far_val - near_val);

    result.m0 = ((float) near_val * 2.0f) / rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;

    result.m4 = 0.0f;
    result.m5 = ((float) near_val * 2.0f) / tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;

    result.m8 = ((float) right + (float) left) / rl;
    result.m9 = ((float) top + (float) bottom) / tb;
    result.m10 = -((float) far_val + (float) near_val) / fn;
    result.m11 = -1.0f;

    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = -((float) far_val * (float) near_val * 2.0f) / fn;
    result.m15 = 0.0f;

    return result;
}

// Returns perspective GL_PROJECTION matrix
// NOTE: Angle should be provided in radians
rf_extern rf_mat rf_mat_perspective(double fovy, double aspect, double near_val, double far_val)
{
    double top = near_val * tan(fovy * 0.5);
    double right = top * aspect;
    rf_mat result = rf_mat_frustum(-right, right, -top, top, near_val, far_val);

    return result;
}

// Returns orthographic GL_PROJECTION matrix
rf_extern rf_mat rf_mat_ortho(double left, double right, double bottom, double top, double near_val, double far_val)
{
    rf_mat result = {0};

    float rl = (float) (right - left);
    float tb = (float) (top - bottom);
    float fn = (float) (far_val - near_val);

    result.m0 = 2.0f / rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;
    result.m4 = 0.0f;
    result.m5 = 2.0f / tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;
    result.m8 = 0.0f;
    result.m9 = 0.0f;
    result.m10 = -2.0f / fn;
    result.m11 = 0.0f;
    result.m12 = -((float) left + (float) right) / rl;
    result.m13 = -((float) top + (float) bottom) / tb;
    result.m14 = -((float) far_val + (float) near_val) / fn;
    result.m15 = 1.0f;

    return result;
}

// Returns camera look-at matrix (view matrix)
rf_extern rf_mat rf_mat_look_at(rf_vec3 eye, rf_vec3 target, rf_vec3 up)
{
    rf_mat result = {0};

    rf_vec3 z = rf_vec3_sub(eye, target);
    z = rf_vec3_normalize(z);
    rf_vec3 x = rf_vec3_cross_product(up, z);
    x = rf_vec3_normalize(x);
    rf_vec3 y = rf_vec3_cross_product(z, x);
    y = rf_vec3_normalize(y);

    result.m0 = x.x;
    result.m1 = x.y;
    result.m2 = x.z;
    result.m3 = 0.0f;
    result.m4 = y.x;
    result.m5 = y.y;
    result.m6 = y.z;
    result.m7 = 0.0f;
    result.m8 = z.x;
    result.m9 = z.y;
    result.m10 = z.z;
    result.m11 = 0.0f;
    result.m12 = eye.x;
    result.m13 = eye.y;
    result.m14 = eye.z;
    result.m15 = 1.0f;

    result = rf_mat_invert(result);

    return result;
}

rf_extern rf_float16 rf_mat_to_float16(rf_mat mat)
{
    rf_float16 buffer = {0};

    buffer.v[0] = mat.m0;
    buffer.v[1] = mat.m1;
    buffer.v[2] = mat.m2;
    buffer.v[3] = mat.m3;
    buffer.v[4] = mat.m4;
    buffer.v[5] = mat.m5;
    buffer.v[6] = mat.m6;
    buffer.v[7] = mat.m7;
    buffer.v[8] = mat.m8;
    buffer.v[9] = mat.m9;
    buffer.v[10] = mat.m10;
    buffer.v[11] = mat.m11;
    buffer.v[12] = mat.m12;
    buffer.v[13] = mat.m13;
    buffer.v[14] = mat.m14;
    buffer.v[15] = mat.m15;

    return buffer;
}

// Returns identity quaternion
rf_extern rf_quaternion rf_quaternion_identity(void)
{
    rf_quaternion result = {0.0f, 0.0f, 0.0f, 1.0f};
    return result;
}

// Computes the length of a quaternion
rf_extern float rf_quaternion_len(rf_quaternion q)
{
    float result = (float) sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return result;
}

// Normalize provided quaternion
rf_extern rf_quaternion rf_quaternion_normalize(rf_quaternion q)
{
    rf_quaternion result = {0};

    float length, ilength;
    length = rf_quaternion_len(q);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f / length;

    result.x = q.x * ilength;
    result.y = q.y * ilength;
    result.z = q.z * ilength;
    result.w = q.w * ilength;

    return result;
}

// Invert provided quaternion
rf_extern rf_quaternion rf_quaternion_invert(rf_quaternion q)
{
    rf_quaternion result = q;
    float length = rf_quaternion_len(q);
    float lengthSq = length * length;

    if (lengthSq != 0.0)
    {
        float i = 1.0f / lengthSq;

        result.x *= -i;
        result.y *= -i;
        result.z *= -i;
        result.w *= i;
    }

    return result;
}

// Calculate two quaternion multiplication
rf_extern rf_quaternion rf_quaternion_mul(rf_quaternion q1, rf_quaternion q2)
{
    rf_quaternion result = {0};

    float qax = q1.x, qay = q1.y, qaz = q1.z, qaw = q1.w;
    float qbx = q2.x, qby = q2.y, qbz = q2.z, qbw = q2.w;

    result.x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
    result.y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
    result.z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
    result.w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

    return result;
}

// Calculate linear interpolation between two quaternions
rf_extern rf_quaternion rf_quaternion_lerp(rf_quaternion q1, rf_quaternion q2, float amount)
{
    rf_quaternion result = {0};

    result.x = q1.x + amount * (q2.x - q1.x);
    result.y = q1.y + amount * (q2.y - q1.y);
    result.z = q1.z + amount * (q2.z - q1.z);
    result.w = q1.w + amount * (q2.w - q1.w);

    return result;
}

// Calculate slerp-optimized interpolation between two quaternions
rf_extern rf_quaternion rf_quaternion_nlerp(rf_quaternion q1, rf_quaternion q2, float amount)
{
    rf_quaternion result = rf_quaternion_lerp(q1, q2, amount);
    result = rf_quaternion_normalize(result);

    return result;
}

// Calculates spherical linear interpolation between two quaternions
rf_extern rf_quaternion rf_quaternion_slerp(rf_quaternion q1, rf_quaternion q2, float amount)
{
    rf_quaternion result = {0};

    float cosHalfTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

    if (fabsf(cosHalfTheta) >= 1.0f) result = q1;
    else if (cosHalfTheta > 0.95f) result = rf_quaternion_nlerp(q1, q2, amount);
    else
    {
        float halfTheta = (float) acosf(cosHalfTheta);
        float sinHalfTheta = (float) sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

        if (fabsf(sinHalfTheta) < 0.001f)
        {
            result.x = (q1.x * 0.5f + q2.x * 0.5f);
            result.y = (q1.y * 0.5f + q2.y * 0.5f);
            result.z = (q1.z * 0.5f + q2.z * 0.5f);
            result.w = (q1.w * 0.5f + q2.w * 0.5f);
        } else
        {
            float ratioA = sinf((1 - amount) * halfTheta) / sinHalfTheta;
            float ratioB = sinf(amount * halfTheta) / sinHalfTheta;

            result.x = (q1.x * ratioA + q2.x * ratioB);
            result.y = (q1.y * ratioA + q2.y * ratioB);
            result.z = (q1.z * ratioA + q2.z * ratioB);
            result.w = (q1.w * ratioA + q2.w * ratioB);
        }
    }

    return result;
}

// Calculate quaternion based on the rotation from one vector to another
rf_extern rf_quaternion rf_quaternion_from_vector3_to_vector3(rf_vec3 from, rf_vec3 to)
{
    rf_quaternion result = {0};

    float cos2Theta = rf_vec3_dot_product(from, to);
    rf_vec3 cross = rf_vec3_cross_product(from, to);

    result.x = cross.x;
    result.y = cross.y;
    result.z = cross.y;
    result.w = 1.0f + cos2Theta; // NOTE: Added QuaternioIdentity()

// Normalize to essentially nlerp the original and identity to 0.5
    result = rf_quaternion_normalize(result);

// Above lines are equivalent to:
//rf_quaternion result = rf_quaternion_nlerp(q, rf_quaternion_identity(), 0.5f);

    return result;
}

// Returns a quaternion for a given rotation matrix
rf_extern rf_quaternion rf_quaternion_from_matrix(rf_mat mat)
{
    rf_quaternion result = {0};

    float trace = rf_mat_trace(mat);

    if (trace > 0.0f)
    {
        float s = (float) sqrtf(trace + 1) * 2.0f;
        float invS = 1.0f / s;

        result.w = s * 0.25f;
        result.x = (mat.m6 - mat.m9) * invS;
        result.y = (mat.m8 - mat.m2) * invS;
        result.z = (mat.m1 - mat.m4) * invS;
    } else
    {
        float m00 = mat.m0, m11 = mat.m5, m22 = mat.m10;

        if (m00 > m11 && m00 > m22)
        {
            float s = (float) sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
            float invS = 1.0f / s;

            result.w = (mat.m6 - mat.m9) * invS;
            result.x = s * 0.25f;
            result.y = (mat.m4 + mat.m1) * invS;
            result.z = (mat.m8 + mat.m2) * invS;
        } else if (m11 > m22)
        {
            float s = (float) sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
            float invS = 1.0f / s;

            result.w = (mat.m8 - mat.m2) * invS;
            result.x = (mat.m4 + mat.m1) * invS;
            result.y = s * 0.25f;
            result.z = (mat.m9 + mat.m6) * invS;
        } else
        {
            float s = (float) sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
            float invS = 1.0f / s;

            result.w = (mat.m1 - mat.m4) * invS;
            result.x = (mat.m8 + mat.m2) * invS;
            result.y = (mat.m9 + mat.m6) * invS;
            result.z = s * 0.25f;
        }
    }

    return result;
}

// Returns a matrix for a given quaternion
rf_extern rf_mat rf_quaternion_to_matrix(rf_quaternion q)
{
    rf_mat result = {0};

    float x = q.x, y = q.y, z = q.z, w = q.w;

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    float length = rf_quaternion_len(q);
    float lengthSquared = length * length;

    float xx = x * x2 / lengthSquared;
    float xy = x * y2 / lengthSquared;
    float xz = x * z2 / lengthSquared;

    float yy = y * y2 / lengthSquared;
    float yz = y * z2 / lengthSquared;
    float zz = z * z2 / lengthSquared;

    float wx = w * x2 / lengthSquared;
    float wy = w * y2 / lengthSquared;
    float wz = w * z2 / lengthSquared;

    result.m0 = 1.0f - (yy + zz);
    result.m1 = xy - wz;
    result.m2 = xz + wy;
    result.m3 = 0.0f;
    result.m4 = xy + wz;
    result.m5 = 1.0f - (xx + zz);
    result.m6 = yz - wx;
    result.m7 = 0.0f;
    result.m8 = xz - wy;
    result.m9 = yz + wx;
    result.m10 = 1.0f - (xx + yy);
    result.m11 = 0.0f;
    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

// Returns rotation quaternion for an angle and axis
// NOTE: angle must be provided in radians
rf_extern rf_quaternion rf_quaternion_from_axis_angle(rf_vec3 axis, float angle)
{
    rf_quaternion result = {0.0f, 0.0f, 0.0f, 1.0f};

    if (rf_vec3_len(axis) != 0.0f)

        angle *= 0.5f;

    axis = rf_vec3_normalize(axis);

    float sinres = sinf(angle);
    float cosres = cosf(angle);

    result.x = axis.x * sinres;
    result.y = axis.y * sinres;
    result.z = axis.z * sinres;
    result.w = cosres;

    result = rf_quaternion_normalize(result);

    return result;
}

// Returns the rotation angle and axis for a given quaternion
rf_extern void rf_quaternion_to_axis_angle(rf_quaternion q, rf_vec3 *outAxis, float *outAngle)
{
    if (fabsf(q.w) > 1.0f) q = rf_quaternion_normalize(q);

    rf_vec3 resAxis = {0.0f, 0.0f, 0.0f};
    float resAngle = 0.0f;

    resAngle = 2.0f * (float) acosf(q.w);
    float den = (float) sqrtf(1.0f - q.w * q.w);

    if (den > 0.0001f)
    {
        resAxis.x = q.x / den;
        resAxis.y = q.y / den;
        resAxis.z = q.z / den;
    } else
    {
        // This occurs when the angle is zero.
        // Not a problem: just set an arbitrary normalized axis.
        resAxis.x = 1.0f;
    }

    *outAxis = resAxis;
    *outAngle = resAngle;
}

// Returns he quaternion equivalent to Euler angles
rf_extern rf_quaternion rf_quaternion_from_euler(float roll, float pitch, float yaw)
{
    rf_quaternion q = {0};

    float x0 = cosf(roll * 0.5f);
    float x1 = sinf(roll * 0.5f);
    float y0 = cosf(pitch * 0.5f);
    float y1 = sinf(pitch * 0.5f);
    float z0 = cosf(yaw * 0.5f);
    float z1 = sinf(yaw * 0.5f);

    q.x = x1 * y0 * z0 - x0 * y1 * z1;
    q.y = x0 * y1 * z0 + x1 * y0 * z1;
    q.z = x0 * y0 * z1 - x1 * y1 * z0;
    q.w = x0 * y0 * z0 + x1 * y1 * z1;

    return q;
}

// Return the Euler angles equivalent to quaternion (roll, pitch, yaw)
// NOTE: Angles are returned in a rf_vec3 struct in degrees
rf_extern rf_vec3 rf_quaternion_to_euler(rf_quaternion q)
{
    rf_vec3 result = {0};

// roll (x-axis rotation)
    float x0 = 2.0f * (q.w * q.x + q.y * q.z);
    float x1 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    result.x = atan2f(x0, x1) * rf_rad2deg;

// pitch (y-axis rotation)
    float y0 = 2.0f * (q.w * q.y - q.z * q.x);
    y0 = y0 > 1.0f ? 1.0f : y0;
    y0 = y0 < -1.0f ? -1.0f : y0;
    result.y = sinf(y0) * rf_rad2deg;

// yaw (z-axis rotation)
    float z0 = 2.0f * (q.w * q.z + q.x * q.y);
    float z1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    result.z = atan2f(z0, z1) * rf_rad2deg;

    return result;
}

// rf_transform a quaternion given a transformation matrix
rf_extern rf_quaternion rf_quaternion_transform(rf_quaternion q, rf_mat mat)
{
    rf_quaternion result = {0};

    result.x = mat.m0 * q.x + mat.m4 * q.y + mat.m8 * q.z + mat.m12 * q.w;
    result.y = mat.m1 * q.x + mat.m5 * q.y + mat.m9 * q.z + mat.m13 * q.w;
    result.z = mat.m2 * q.x + mat.m6 * q.y + mat.m10 * q.z + mat.m14 * q.w;
    result.w = mat.m3 * q.x + mat.m7 * q.y + mat.m11 * q.z + mat.m15 * q.w;

    return result;
}