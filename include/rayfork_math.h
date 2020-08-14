#ifndef RAYFORK_MATH_H
#define RAYFORK_MATH_H

#include "rayfork_common.h"

#define RF_PI (3.14159265358979323846f)
#define RF_DEG2RAD (RF_PI / 180.0f)
#define RF_RAD2DEG (180.0f / RF_PI)

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

typedef struct rf_rec
{
    float x;
    float y;
    float width;
    float height;
} rf_rec;

typedef struct rf_ray
{
    rf_vec3 position;  // position (origin)
    rf_vec3 direction; // direction
} rf_ray;

typedef struct rf_ray_hit_info
{
    bool hit; // Did the ray hit something?
    float distance; // Distance to nearest hit
    rf_vec3 position; // Position of nearest hit
    rf_vec3 normal; // Surface normal of hit
} rf_ray_hit_info;

typedef struct rf_bounding_box
{
    rf_vec3 min; // Minimum vertex box-corner
    rf_vec3 max; // Maximum vertex box-corner
} rf_bounding_box;

#pragma region misc
RF_API float rf_next_pot(float it);
RF_API rf_vec2 rf_center_to_screen(float w, float h); // Returns the position of an object such that it will be centered to the screen
RF_API rf_vec2 rf_center_to_object(rf_sizef center_this, rf_rec to_this); // Returns the position of an object such that it will be centered to a rectangle
RF_API float rf_clamp(float value, float min, float max); // Clamp float value
RF_API float rf_lerp(float start, float end, float amount); // Calculate linear interpolation between two floats
#pragma endregion

#pragma region vec and matrix math

RF_API rf_vec2 rf_vec2_add(rf_vec2 v1, rf_vec2 v2); // Add two vectors (v1 + v2)
RF_API rf_vec2 rf_vec2_sub(rf_vec2 v1, rf_vec2 v2); // Subtract two vectors (v1 - v2)
RF_API float rf_vec2_len(rf_vec2 v); // Calculate vector length
RF_API float rf_vec2_dot_product(rf_vec2 v1, rf_vec2 v2); // Calculate two vectors dot product
RF_API float rf_vec2_distance(rf_vec2 v1, rf_vec2 v2); // Calculate distance between two vectors
RF_API float rf_vec2_angle(rf_vec2 v1, rf_vec2 v2); // Calculate angle from two vectors in X-axis
RF_API rf_vec2 rf_vec2_scale(rf_vec2 v, float scale); // Scale vector (multiply by value)
RF_API rf_vec2 rf_vec2_mul_v(rf_vec2 v1, rf_vec2 v2); // Multiply vector by vector
RF_API rf_vec2 rf_vec2_negate(rf_vec2 v); // Negate vector
RF_API rf_vec2 rf_vec2_div(rf_vec2 v, float div); // Divide vector by a float value
RF_API rf_vec2 rf_vec2_div_v(rf_vec2 v1, rf_vec2 v2); // Divide vector by vector
RF_API rf_vec2 rf_vec2_normalize(rf_vec2 v); // Normalize provided vector
RF_API rf_vec2 rf_vec2_lerp(rf_vec2 v1, rf_vec2 v2, float amount); // Calculate linear interpolation between two vectors

RF_API rf_vec3 rf_vec3_add(rf_vec3 v1, rf_vec3 v2); // Add two vectors
RF_API rf_vec3 rf_vec3_sub(rf_vec3 v1, rf_vec3 v2); // Subtract two vectors
RF_API rf_vec3 rf_vec3_mul(rf_vec3 v, float scalar); // Multiply vector by scalar
RF_API rf_vec3 rf_vec3_mul_v(rf_vec3 v1, rf_vec3 v2); // Multiply vector by vector
RF_API rf_vec3 rf_vec3_cross_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors cross product
RF_API rf_vec3 rf_vec3_perpendicular(rf_vec3 v); // Calculate one vector perpendicular vector
RF_API float rf_vec3_len(rf_vec3 v); // Calculate vector length
RF_API float rf_vec3_dot_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors dot product
RF_API float rf_vec3_distance(rf_vec3 v1, rf_vec3 v2); // Calculate distance between two vectors
RF_API rf_vec3 rf_vec3_scale(rf_vec3 v, float scale); // Scale provided vector
RF_API rf_vec3 rf_vec3_negate(rf_vec3 v); // Negate provided vector (invert direction)
RF_API rf_vec3 rf_vec3_div(rf_vec3 v, float div); // Divide vector by a float value
RF_API rf_vec3 rf_vec3_div_v(rf_vec3 v1, rf_vec3 v2); // Divide vector by vector
RF_API rf_vec3 rf_vec3_normalize(rf_vec3 v); // Normalize provided vector
RF_API void rf_vec3_ortho_normalize(rf_vec3* v1, rf_vec3* v2); // Orthonormalize provided vectors. Makes vectors normalized and orthogonal to each other. Gram-Schmidt function implementation
RF_API rf_vec3 rf_vec3_transform(rf_vec3 v, rf_mat mat); // Transforms a rf_vec3 by a given rf_mat
RF_API rf_vec3 rf_vec3_rotate_by_quaternion(rf_vec3 v, rf_quaternion q); // rf_transform a vector by quaternion rotation
RF_API rf_vec3 rf_vec3_lerp(rf_vec3 v1, rf_vec3 v2, float amount); // Calculate linear interpolation between two vectors
RF_API rf_vec3 rf_vec3_reflect(rf_vec3 v, rf_vec3 normal); // Calculate reflected vector to normal
RF_API rf_vec3 rf_vec3_min(rf_vec3 v1, rf_vec3 v2); // Return min value for each pair of components
RF_API rf_vec3 rf_vec3_max(rf_vec3 v1, rf_vec3 v2); // Return max value for each pair of components
RF_API rf_vec3 rf_vec3_barycenter(rf_vec3 p, rf_vec3 a, rf_vec3 b, rf_vec3 c); // Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c) NOTE: Assumes P is on the plane of the triangle

RF_API float rf_mat_determinant(rf_mat mat); // Compute matrix determinant
RF_API float rf_mat_trace(rf_mat mat); // Returns the trace of the matrix (sum of the values along the diagonal)
RF_API rf_mat rf_mat_transpose(rf_mat mat); // Transposes provided matrix
RF_API rf_mat rf_mat_invert(rf_mat mat); // Invert provided matrix
RF_API rf_mat rf_mat_normalize(rf_mat mat); // Normalize provided matrix
RF_API rf_mat rf_mat_identity(void); // Returns identity matrix
RF_API rf_mat rf_mat_add(rf_mat left, rf_mat right); // Add two matrices
RF_API rf_mat rf_mat_sub(rf_mat left, rf_mat right); // Subtract two matrices (left - right)
RF_API rf_mat rf_mat_translate(float x, float y, float z); // Returns translation matrix
RF_API rf_mat rf_mat_rotate(rf_vec3 axis, float angle); // Create rotation matrix from axis and angle. NOTE: Angle should be provided in radians
RF_API rf_mat rf_mat_rotate_xyz(rf_vec3 ang); // Returns xyz-rotation matrix (angles in radians)
RF_API rf_mat rf_mat_rotate_x(float angle); // Returns x-rotation matrix (angle in radians)
RF_API rf_mat rf_mat_rotate_y(float angle); // Returns y-rotation matrix (angle in radians)
RF_API rf_mat rf_mat_rotate_z(float angle); // Returns z-rotation matrix (angle in radians)
RF_API rf_mat rf_mat_scale(float x, float y, float z); // Returns scaling matrix
RF_API rf_mat rf_mat_mul(rf_mat left, rf_mat right); // Returns two matrix multiplication. NOTE: When multiplying matrices... the order matters!
RF_API rf_mat rf_mat_frustum(double left, double right, double bottom, double top, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix
RF_API rf_mat rf_mat_perspective(double fovy, double aspect, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix. NOTE: Angle should be provided in radians
RF_API rf_mat rf_mat_ortho(double left, double right, double bottom, double top, double near_val, double far_val); // Returns orthographic GL_PROJECTION matrix
RF_API rf_mat rf_mat_look_at(rf_vec3 eye, rf_vec3 target, rf_vec3 up); // Returns camera look-at matrix (view matrix)
RF_API rf_float16 rf_mat_to_float16(rf_mat mat); // Returns the matrix as an array of 16 floats

RF_API rf_quaternion rf_quaternion_identity(void); // Returns identity quaternion
RF_API float rf_quaternion_len(rf_quaternion q); // Computes the length of a quaternion
RF_API rf_quaternion rf_quaternion_normalize(rf_quaternion q); // Normalize provided quaternion
RF_API rf_quaternion rf_quaternion_invert(rf_quaternion q); // Invert provided quaternion
RF_API rf_quaternion rf_quaternion_mul(rf_quaternion q1, rf_quaternion q2); // Calculate two quaternion multiplication
RF_API rf_quaternion rf_quaternion_lerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate linear interpolation between two quaternions
RF_API rf_quaternion rf_quaternion_nlerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate slerp-optimized interpolation between two quaternions
RF_API rf_quaternion rf_quaternion_slerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculates spherical linear interpolation between two quaternions
RF_API rf_quaternion rf_quaternion_from_vec3_to_vec3(rf_vec3 from, rf_vec3 to); // Calculate quaternion based on the rotation from one vector to another
RF_API rf_quaternion rf_quaternion_from_mat(rf_mat mat); // Returns a quaternion for a given rotation matrix
RF_API rf_mat rf_quaternion_to_mat(rf_quaternion q); // Returns a matrix for a given quaternion
RF_API rf_quaternion rf_quaternion_from_axis_angle(rf_vec3 axis, float angle); // Returns rotation quaternion for an angle and axis. NOTE: angle must be provided in radians
RF_API void rf_quaternion_to_axis_angle(rf_quaternion q, rf_vec3* outAxis, float* outAngle); // Returns the rotation angle and axis for a given quaternion
RF_API rf_quaternion rf_quaternion_from_euler(float roll, float pitch, float yaw); // Returns he quaternion equivalent to Euler angles
RF_API rf_vec3 rf_quaternion_to_euler(rf_quaternion q); // Return the Euler angles equivalent to quaternion (roll, pitch, yaw). NOTE: Angles are returned in a rf_vec3 struct in degrees
RF_API rf_quaternion rf_quaternion_transform(rf_quaternion q, rf_mat mat); // rf_transform a quaternion given a transformation matrix

#pragma endregion

#pragma region collision detection

RF_API bool rf_rec_match(rf_rec a, rf_rec b);
RF_API bool rf_check_collision_recs(rf_rec rec1, rf_rec rec2); // Check collision between two rectangles
RF_API bool rf_check_collision_circles(rf_vec2 center1, float radius1, rf_vec2 center2, float radius2); // Check collision between two circles
RF_API bool rf_check_collision_circle_rec(rf_vec2 center, float radius, rf_rec rec); // Check collision between circle and rectangle
RF_API bool rf_check_collision_point_rec(rf_vec2 point, rf_rec rec); // Check if point is inside rectangle
RF_API bool rf_check_collision_point_circle(rf_vec2 point, rf_vec2 center, float radius); // Check if point is inside circle
RF_API bool rf_check_collision_point_triangle(rf_vec2 point, rf_vec2 p1, rf_vec2 p2, rf_vec2 p3); // Check if point is inside a triangle

RF_API rf_rec rf_get_collision_rec(rf_rec rec1, rf_rec rec2); // Get collision rectangle for two rectangles collision

RF_API bool rf_check_collision_spheres(rf_vec3 center_a, float radius_a, rf_vec3 center_b, float radius_b); // Detect collision between two spheres
RF_API bool rf_check_collision_boxes(rf_bounding_box box1, rf_bounding_box box2); // Detect collision between two bounding boxes
RF_API bool rf_check_collision_box_sphere(rf_bounding_box box, rf_vec3 center, float radius); // Detect collision between box and sphere
RF_API bool rf_check_collision_ray_sphere(rf_ray ray, rf_vec3 center, float radius); // Detect collision between ray and sphere
RF_API bool rf_check_collision_ray_sphere_ex(rf_ray ray, rf_vec3 center, float radius, rf_vec3* collision_point); // Detect collision between ray and sphere, returns collision point
RF_API bool rf_check_collision_ray_box(rf_ray ray, rf_bounding_box box); // Detect collision between ray and box

struct rf_model;

RF_API rf_ray_hit_info rf_collision_ray_model(rf_ray ray, struct rf_model model); // Get collision info between ray and model
RF_API rf_ray_hit_info rf_collision_ray_triangle(rf_ray ray, rf_vec3 p1, rf_vec3 p2, rf_vec3 p3); // Get collision info between ray and triangle
RF_API rf_ray_hit_info rf_collision_ray_ground(rf_ray ray, float ground_height); // Get collision info between ray and ground plane (Y-normal plane)

#pragma endregion

#endif // RAYFORK_MATH_H