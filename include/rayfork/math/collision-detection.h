#ifndef RAYFORK_COLLISION_DETECTION_H
#define RAYFORK_COLLISION_DETECTION_H

#include "rayfork/foundation/basicdef.h"
#include "math-types.h"

rf_extern bool rf_rec_match(rf_rec a, rf_rec b);
rf_extern bool rf_check_collision_recs(rf_rec rec1, rf_rec rec2); // Check collision between two rectangles
rf_extern bool rf_check_collision_circles(rf_vec2 center1, float radius1, rf_vec2 center2, float radius2); // Check collision between two circles
rf_extern bool rf_check_collision_circle_rec(rf_vec2 center, float radius, rf_rec rec); // Check collision between circle and rectangle
rf_extern bool rf_check_collision_point_rec(rf_vec2 point, rf_rec rec); // Check if point is inside rectangle
rf_extern bool rf_check_collision_point_circle(rf_vec2 point, rf_vec2 center, float radius); // Check if point is inside circle
rf_extern bool rf_check_collision_point_triangle(rf_vec2 point, rf_vec2 p1, rf_vec2 p2, rf_vec2 p3); // Check if point is inside a triangle

rf_extern rf_rec rf_get_collision_rec(rf_rec rec1, rf_rec rec2); // Get collision rectangle for two rectangles collision

rf_extern bool rf_check_collision_spheres(rf_vec3 center_a, float radius_a, rf_vec3 center_b, float radius_b); // Detect collision between two spheres
rf_extern bool rf_check_collision_boxes(rf_bounding_box box1, rf_bounding_box box2); // Detect collision between two bounding boxes
rf_extern bool rf_check_collision_box_sphere(rf_bounding_box box, rf_vec3 center, float radius); // Detect collision between box and sphere
rf_extern bool rf_check_collision_ray_sphere(rf_ray ray, rf_vec3 center, float radius); // Detect collision between ray and sphere
rf_extern bool rf_check_collision_ray_sphere_ex(rf_ray ray, rf_vec3 center, float radius, rf_vec3* collision_point); // Detect collision between ray and sphere, returns collision point
rf_extern bool rf_check_collision_ray_box(rf_ray ray, rf_bounding_box box); // Detect collision between ray and box

rf_extern rf_ray_hit_info rf_collision_ray_triangle(rf_ray ray, rf_vec3 p1, rf_vec3 p2, rf_vec3 p3); // Get collision info between ray and triangle
rf_extern rf_ray_hit_info rf_collision_ray_ground(rf_ray ray, float ground_height); // Get collision info between ray and ground plane (Y-normal plane)

#endif // RAYFORK_COLLISION_DETECTION_H