#ifndef RAYFORK_MATH_TYPES_H
#define RAYFORK_MATH_TYPES_H

#include "rayfork/foundation/basicdef.h"

#define rf_pi (3.14159265358979323846f)
#define rf_deg2rad (rf_pi / 180.0f)
#define rf_rad2deg (180.0f / rf_pi)

typedef struct rf_sizei
{
    int width;
    int height;
} rf_sizei;

typedef struct rf_sizef
{
    float width;
    float height;
} rf_sizef;

typedef struct rf_rec
{
    float x;
    float y;
    float width;
    float height;
} rf_rec;

typedef struct rf_vec2
{
    float x;
    float y;
} rf_vec2;

typedef struct rf_vec3
{
    float x;
    float y;
    float z;
} rf_vec3;

typedef struct rf_vec4
{
    float x;
    float y;
    float z;
    float w;
} rf_vec4, rf_quaternion;

// The matrix is OpenGL style 4x4 - right handed, column major
typedef struct rf_mat
{
    float m0, m4, m8,  m12;
    float m1, m5, m9,  m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
} rf_mat;

typedef struct rf_float16
{
    float v[16];
} rf_float16;

typedef struct rf_ray
{
    rf_vec3 position;  // position (origin)
    rf_vec3 direction; // direction
} rf_ray;

typedef struct rf_ray_hit_info
{
    bool hit;         // Did the ray hit something?
    float distance;   // Distance to nearest hit
    rf_vec3 position; // Position of nearest hit
    rf_vec3 normal;   // Surface normal of hit
} rf_ray_hit_info;

typedef struct rf_bounding_box
{
    rf_vec3 min; // Minimum vertex box-corner
    rf_vec3 max; // Maximum vertex box-corner
} rf_bounding_box;

#endif // RAYFORK_MATH_TYPES_H