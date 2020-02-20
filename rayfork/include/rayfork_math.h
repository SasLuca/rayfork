#ifndef RAYFORK_MATH_H
#define RAYFORK_MATH_H

#include "rayfork_common.h"

#ifndef RF_MATH_API
    #define RF_MATH_API RF_API
#endif

#ifndef RF_MATH_INTERNAL
    #define RF_MATH_INTERNAL RF_MATH_API
#endif

#define RF_PI (3.14159265358979323846f)

#define RF_DEG2RAD (RF_PI/180.0f)
#define RF_RAD2DEG (180.0f/RF_PI)

#define RF_VEC2_ZERO ((rf_vec2) { 0.0f, 0.0f })
#define RF_VEC2_ONE  ((rf_vec2) { 1.0f, 1.0f })

#define RF_VEC3_ZERO ((rf_vec3) { 0.0f, 0.0f, 0.0f })
#define RF_VEC3_ONE  ((rf_vec3) { 1.0f, 1.0f, 1.0f })

//region color
RF_MATH_API int rf_color_to_int(rf_color color); // Returns hexadecimal value for a rf_color
RF_MATH_API rf_vec4 rf_color_normalize(rf_color color); // Returns color normalized as float [0..1]
RF_MATH_API rf_color rf_color_from_normalized(rf_vec4 normalized); // Returns color from normalized values [0..1]
RF_MATH_API rf_vec3 rf_color_to_hsv(rf_color color); // Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
RF_MATH_API rf_color rf_color_from_hsv(rf_vec3 hsv); // Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
RF_MATH_API rf_color rf_color_from_int(int hex_value); // Returns a rf_color struct from hexadecimal value
RF_MATH_API rf_color rf_fade(rf_color color, float alpha); // rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
//endregion

//region camera
RF_MATH_API rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view); // Get world coordinates from screen coordinates
RF_MATH_API rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera); // Returns a ray trace from mouse position
RF_MATH_API rf_mat rf_get_camera_matrix(rf_camera3d camera); // Get transform matrix for camera
RF_MATH_API rf_mat rf_get_camera_matrix2d(rf_camera2d camera); // Returns camera 2d transform matrix
RF_MATH_API rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera); // Returns the screen space position from a 3d world space position
RF_MATH_API rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera); // Returns the screen space position for a 2d camera world space position
RF_MATH_API  rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera); // Returns the world space position for a 2d camera screen space position
//endregion

//region math
RF_MATH_API int rf_get_buffer_size_for_pixel_format(int width, int height, int format); //Get the buffer size of an image of a specific width and height in a given format
RF_MATH_API float rf_clamp(float value, float min, float max); // Clamp float value
RF_MATH_API float rf_lerp(float start, float end, float amount); // Calculate linear interpolation between two floats

RF_MATH_API rf_vec2 rf_vec2_add(rf_vec2 v1, rf_vec2 v2); // Add two vectors (v1 + v2)
RF_MATH_API rf_vec2 rf_vec2_sub(rf_vec2 v1, rf_vec2 v2); // Subtract two vectors (v1 - v2)
RF_MATH_API float rf_vec2_len(rf_vec2 v); // Calculate vector length
RF_MATH_API float rf_vec2_dot_product(rf_vec2 v1, rf_vec2 v2); // Calculate two vectors dot product
RF_MATH_API float rf_vec2_distance(rf_vec2 v1, rf_vec2 v2); // Calculate distance between two vectors
RF_MATH_API float rf_vec2_angle(rf_vec2 v1, rf_vec2 v2); // Calculate angle from two vectors in X-axis
RF_MATH_API rf_vec2 rf_vec2_scale(rf_vec2 v, float scale); // Scale vector (multiply by value)
RF_MATH_API rf_vec2 rf_vec2_mul_v(rf_vec2 v1, rf_vec2 v2); // Multiply vector by vector
RF_MATH_API rf_vec2 rf_vec2_negate(rf_vec2 v); // Negate vector
RF_MATH_API rf_vec2 rf_vec2_div(rf_vec2 v, float div); // Divide vector by a float value
RF_MATH_API rf_vec2 rf_vec2_div_v(rf_vec2 v1, rf_vec2 v2); // Divide vector by vector
RF_MATH_API rf_vec2 rf_vec2_normalize(rf_vec2 v); // Normalize provided vector
RF_MATH_API rf_vec2 rf_vec2_lerp(rf_vec2 v1, rf_vec2 v2, float amount); // Calculate linear interpolation between two vectors

RF_MATH_API rf_vec3 rf_vec3_add(rf_vec3 v1, rf_vec3 v2); // Add two vectors
RF_MATH_API rf_vec3 rf_vec3_sub(rf_vec3 v1, rf_vec3 v2); // Subtract two vectors
RF_MATH_API rf_vec3 rf_vec3_mul(rf_vec3 v, float scalar); // Multiply vector by scalar
RF_MATH_API rf_vec3 rf_vec3_mul_v(rf_vec3 v1, rf_vec3 v2); // Multiply vector by vector
RF_MATH_API rf_vec3 rf_vec3_cross_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors cross product
RF_MATH_API rf_vec3 rf_vec3_perpendicular(rf_vec3 v); // Calculate one vector perpendicular vector
RF_MATH_API float rf_vec3_len(rf_vec3 v); // Calculate vector length
RF_MATH_API float rf_vec3_dot_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors dot product
RF_MATH_API float rf_vec3_distance(rf_vec3 v1, rf_vec3 v2); // Calculate distance between two vectors
RF_MATH_API rf_vec3 rf_vec3_scale(rf_vec3 v, float scale); // Scale provided vector
RF_MATH_API rf_vec3 rf_vec3_negate(rf_vec3 v); // Negate provided vector (invert direction)
RF_MATH_API rf_vec3 rf_vec3_div(rf_vec3 v, float div); // Divide vector by a float value
RF_MATH_API rf_vec3 rf_vec3_div_v(rf_vec3 v1, rf_vec3 v2); // Divide vector by vector
RF_MATH_API rf_vec3 rf_vec3_normalize(rf_vec3 v); // Normalize provided vector
RF_MATH_API void rf_vec3_ortho_normalize(rf_vec3* v1, rf_vec3* v2); // Orthonormalize provided vectors. Makes vectors normalized and orthogonal to each other. Gram-Schmidt function implementation
RF_MATH_API rf_vec3 rf_vec3_transform(rf_vec3 v, rf_mat mat); // Transforms a rf_vec3 by a given rf_mat
RF_MATH_API rf_vec3 rf_vec3_rotate_by_quaternion(rf_vec3 v, rf_quaternion q); // rf_transform a vector by quaternion rotation
RF_MATH_API rf_vec3 rf_vec3_lerp(rf_vec3 v1, rf_vec3 v2, float amount); // Calculate linear interpolation between two vectors
RF_MATH_API rf_vec3 rf_vec3_reflect(rf_vec3 v, rf_vec3 normal); // Calculate reflected vector to normal
RF_MATH_API rf_vec3 rf_vec3_min(rf_vec3 v1, rf_vec3 v2); // Return min value for each pair of components
RF_MATH_API rf_vec3 rf_vec3_max(rf_vec3 v1, rf_vec3 v2); // Return max value for each pair of components
RF_MATH_API rf_vec3 rf_vec3_barycenter(rf_vec3 p, rf_vec3 a, rf_vec3 b, rf_vec3 c); // Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c) NOTE: Assumes P is on the plane of the triangle

RF_MATH_API float rf_mat_determinant(rf_mat mat); // Compute matrix determinant
RF_MATH_API float rf_mat_trace(rf_mat mat); // Returns the trace of the matrix (sum of the values along the diagonal)
RF_MATH_API rf_mat rf_mat_transpose(rf_mat mat); // Transposes provided matrix
RF_MATH_API rf_mat rf_mat_invert(rf_mat mat); // Invert provided matrix
RF_MATH_API rf_mat rf_mat_normalize(rf_mat mat); // Normalize provided matrix
RF_MATH_API rf_mat rf_mat_identity(void); // Returns identity matrix
RF_MATH_API rf_mat rf_mat_add(rf_mat left, rf_mat right); // Add two matrices
RF_MATH_API rf_mat rf_mat_sub(rf_mat left, rf_mat right); // Subtract two matrices (left - right)
RF_MATH_API rf_mat rf_mat_translate(float x, float y, float z); // Returns translation matrix
RF_MATH_API rf_mat rf_mat_rotate(rf_vec3 axis, float angle); // Create rotation matrix from axis and angle. NOTE: Angle should be provided in radians
RF_MATH_API rf_mat rf_mat_rotate_xyz(rf_vec3 ang); // Returns xyz-rotation matrix (angles in radians)
RF_MATH_API rf_mat rf_mat_rotate_x(float angle); // Returns x-rotation matrix (angle in radians)
RF_MATH_API rf_mat rf_mat_rotate_y(float angle); // Returns y-rotation matrix (angle in radians)
RF_MATH_API rf_mat rf_mat_rotate_z(float angle); // Returns z-rotation matrix (angle in radians)
RF_MATH_API rf_mat rf_mat_scale(float x, float y, float z); // Returns scaling matrix
RF_MATH_API rf_mat rf_mat_mul(rf_mat left, rf_mat right); // Returns two matrix multiplication. NOTE: When multiplying matrices... the order matters!
RF_MATH_API rf_mat rf_mat_frustum(double left, double right, double bottom, double top, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix
RF_MATH_API rf_mat rf_mat_perspective(double fovy, double aspect, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix. NOTE: Angle should be provided in radians
RF_MATH_API rf_mat rf_mat_ortho(double left, double right, double bottom, double top, double near_val, double far_val); // Returns orthographic GL_PROJECTION matrix
RF_MATH_API rf_mat rf_mat_look_at(rf_vec3 eye, rf_vec3 target, rf_vec3 up); // Returns camera look-at matrix (view matrix)
RF_MATH_API rf_float16 rf_mat_to_float16(rf_mat mat); // Returns the matrix as an array of 16 floats

RF_MATH_API rf_quaternion rf_quaternion_identity(void); // Returns identity quaternion
RF_MATH_API float rf_quaternion_len(rf_quaternion q); // Computes the length of a quaternion
RF_MATH_API rf_quaternion rf_quaternion_normalize(rf_quaternion q); // Normalize provided quaternion
RF_MATH_API rf_quaternion rf_quaternion_invert(rf_quaternion q); // Invert provided quaternion
RF_MATH_API rf_quaternion rf_quaternion_mul(rf_quaternion q1, rf_quaternion q2); // Calculate two quaternion multiplication
RF_MATH_API rf_quaternion rf_quaternion_lerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate linear interpolation between two quaternions
RF_MATH_API rf_quaternion rf_quaternion_nlerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate slerp-optimized interpolation between two quaternions
RF_MATH_API rf_quaternion rf_quaternion_slerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculates spherical linear interpolation between two quaternions
RF_MATH_API rf_quaternion rf_quaternion_from_vec3_to_vec3(rf_vec3 from, rf_vec3 to); // Calculate quaternion based on the rotation from one vector to another
RF_MATH_API rf_quaternion rf_quaternion_from_mat(rf_mat mat); // Returns a quaternion for a given rotation matrix
RF_MATH_API rf_mat rf_quaternion_to_mat(rf_quaternion q); // Returns a matrix for a given quaternion
RF_MATH_API rf_quaternion rf_quaternion_from_axis_angle(rf_vec3 axis, float angle); // Returns rotation quaternion for an angle and axis. NOTE: angle must be provided in radians
RF_MATH_API void rf_quaternion_to_axis_angle(rf_quaternion q, rf_vec3* outAxis, float* outAngle); // Returns the rotation angle and axis for a given quaternion
RF_MATH_API rf_quaternion rf_quaternion_from_euler(float roll, float pitch, float yaw); // Returns he quaternion equivalent to Euler angles
RF_MATH_API rf_vec3 rf_quaternion_to_euler(rf_quaternion q); // Return the Euler angles equivalent to quaternion (roll, pitch, yaw). NOTE: Angles are returned in a rf_vec3 struct in degrees
RF_MATH_API rf_quaternion rf_quaternion_transform(rf_quaternion q, rf_mat mat); // rf_transform a quaternion given a transformation matrix

//endregion

//region collision detection

RF_MATH_API bool rf_check_collision_rec(rf_rec rec1, rf_rec rec2); // Check collision between two rectangles
RF_MATH_API bool rf_check_collision_circles(rf_vec2 center1, float radius1, rf_vec2 center2, float radius2); // Check collision between two circles
RF_MATH_API bool rf_check_collision_circle_rec(rf_vec2 center, float radius, rf_rec rec); // Check collision between circle and rectangle
RF_MATH_API rf_rec rf_get_collision_rec(rf_rec rec1, rf_rec rec2); // Get collision rectangle for two rectangles collision
RF_MATH_API bool rf_check_collision_point_rec(rf_vec2 point, rf_rec rec); // Check if point is inside rectangle
RF_MATH_API bool rf_check_collision_point_circle(rf_vec2 point, rf_vec2 center, float radius); // Check if point is inside circle
RF_MATH_API bool rf_check_collision_point_triangle(rf_vec2 point, rf_vec2 p1, rf_vec2 p2, rf_vec2 p3); // Check if point is inside a triangle

//endregion

#endif