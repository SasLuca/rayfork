#include "rayfork/math/collision-detection.h"
#include "rayfork/math/maths.h"

// Check if point is inside rectangle
bool rf_check_collision_point_rec(rf_vec2 point, rf_rec rec)
{
    bool collision = 0;

    if ((point.x >= rec.x) && (point.x <= (rec.x + rec.width)) && (point.y >= rec.y) &&
        (point.y <= (rec.y + rec.height)))
        collision = 1;

    return collision;
}

// Check if point is inside circle
bool rf_check_collision_point_circle(rf_vec2 point, rf_vec2 center, float radius)
{
    return rf_check_collision_circles(point, 0, center, radius);
}

// Check if point is inside a triangle defined by three points (p1, p2, p3)
bool rf_check_collision_point_triangle(rf_vec2 point, rf_vec2 p1, rf_vec2 p2, rf_vec2 p3)
{
    bool collision = 0;

    float alpha = ((p2.y - p3.y) * (point.x - p3.x) + (p3.x - p2.x) * (point.y - p3.y)) /
                  ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));

    float beta = ((p3.y - p1.y) * (point.x - p3.x) + (p1.x - p3.x) * (point.y - p3.y)) /
                 ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));

    float gamma = 1.0f - alpha - beta;

    if ((alpha > 0) && (beta > 0) & (gamma > 0)) collision = 1;

    return collision;
}

// Check collision between two rectangles
bool rf_check_collision_recs(rf_rec rec1, rf_rec rec2)
{
    bool collision = 0;

    if ((rec1.x < (rec2.x + rec2.width) && (rec1.x + rec1.width) > rec2.x) &&
        (rec1.y < (rec2.y + rec2.height) && (rec1.y + rec1.height) > rec2.y))
        collision = 1;

    return collision;
}

// Check collision between two circles
bool rf_check_collision_circles(rf_vec2 center1, float radius1, rf_vec2 center2, float radius2)
{
    bool collision = 0;

    float dx = center2.x - center1.x; // X distance between centers
    float dy = center2.y - center1.y; // Y distance between centers

    float distance = sqrtf(dx * dx + dy * dy); // Distance between centers

    if (distance <= (radius1 + radius2)) collision = 1;

    return collision;
}

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool rf_check_collision_circle_rec(rf_vec2 center, float radius, rf_rec rec)
{
    int recCenterX = (int) (rec.x + rec.width / 2.0f);
    int recCenterY = (int) (rec.y + rec.height / 2.0f);

    float dx = (float) fabs(center.x - recCenterX);
    float dy = (float) fabs(center.y - recCenterY);

    if (dx > (rec.width / 2.0f + radius))
    { return 0; }
    if (dy > (rec.height / 2.0f + radius))
    { return 0; }

    if (dx <= (rec.width / 2.0f))
    { return 1; }
    if (dy <= (rec.height / 2.0f))
    { return 1; }

    float cornerDistanceSq = (dx - rec.width / 2.0f) * (dx - rec.width / 2.0f) +
                             (dy - rec.height / 2.0f) * (dy - rec.height / 2.0f);

    return (cornerDistanceSq <= (radius * radius));
}

// Get collision rectangle for two rectangles collision
rf_rec rf_get_collision_rec(rf_rec rec1, rf_rec rec2)
{
    rf_rec retRec = {0, 0, 0, 0};

    if (rf_check_collision_recs(rec1, rec2))
    {
        float dxx = (float) fabs(rec1.x - rec2.x);
        float dyy = (float) fabs(rec1.y - rec2.y);

        if (rec1.x <= rec2.x)
        {
            if (rec1.y <= rec2.y)
            {
                retRec.x = rec2.x;
                retRec.y = rec2.y;
                retRec.width = rec1.width - dxx;
                retRec.height = rec1.height - dyy;
            } else
            {
                retRec.x = rec2.x;
                retRec.y = rec1.y;
                retRec.width = rec1.width - dxx;
                retRec.height = rec2.height - dyy;
            }
        } else
        {
            if (rec1.y <= rec2.y)
            {
                retRec.x = rec1.x;
                retRec.y = rec2.y;
                retRec.width = rec2.width - dxx;
                retRec.height = rec1.height - dyy;
            } else
            {
                retRec.x = rec1.x;
                retRec.y = rec1.y;
                retRec.width = rec2.width - dxx;
                retRec.height = rec2.height - dyy;
            }
        }

        if (rec1.width > rec2.width)
        {
            if (retRec.width >= rec2.width) retRec.width = rec2.width;
        } else
        {
            if (retRec.width >= rec1.width) retRec.width = rec1.width;
        }

        if (rec1.height > rec2.height)
        {
            if (retRec.height >= rec2.height) retRec.height = rec2.height;
        } else
        {
            if (retRec.height >= rec1.height) retRec.height = rec1.height;
        }
    }

    return retRec;
}

// Detect collision between two spheres
rf_extern bool rf_check_collision_spheres(rf_vec3 center_a, float radius_a, rf_vec3 center_b, float radius_b)
{
    bool collision = 0;

// Simple way to check for collision, just checking distance between two points
// Unfortunately, rf_sqrtf() is a costly operation, so we avoid it with following solution
/*
float dx = centerA.x - centerB.x;      // X distance between centers
float dy = centerA.y - centerB.y;      // Y distance between centers
float dz = centerA.z - centerB.z;      // Z distance between centers

float distance = rf_sqrtf(dx*dx + dy*dy + dz*dz);  // Distance between centers

if (distance <= (radiusA + radiusB)) collision = 1;
*/
// Check for distances squared to avoid rf_sqrtf()
    if (rf_vec3_dot_product(rf_vec3_sub(center_b, center_a), rf_vec3_sub(center_b, center_a)) <=
        (radius_a + radius_b) * (radius_a + radius_b))
        collision = 1;

    return collision;
}

// Detect collision between two boxes. Note: Boxes are defined by two points minimum and maximum
rf_extern bool rf_check_collision_boxes(rf_bounding_box box1, rf_bounding_box box2)
{
    bool collision = 1;

    if ((box1.max.x >= box2.min.x) && (box1.min.x <= box2.max.x))
    {
        if ((box1.max.y < box2.min.y) || (box1.min.y > box2.max.y)) collision = 0;
        if ((box1.max.z < box2.min.z) || (box1.min.z > box2.max.z)) collision = 0;
    } else collision = 0;

    return collision;
}

// Detect collision between box and sphere
rf_extern bool rf_check_collision_box_sphere(rf_bounding_box box, rf_vec3 center, float radius)
{
    bool collision = 0;

    float dmin = 0;

    if (center.x < box.min.x) dmin += powf(center.x - box.min.x, 2);
    else if (center.x > box.max.x) dmin += powf(center.x - box.max.x, 2);

    if (center.y < box.min.y) dmin += powf(center.y - box.min.y, 2);
    else if (center.y > box.max.y) dmin += powf(center.y - box.max.y, 2);

    if (center.z < box.min.z) dmin += powf(center.z - box.min.z, 2);
    else if (center.z > box.max.z) dmin += powf(center.z - box.max.z, 2);

    if (dmin <= (radius * radius)) collision = 1;

    return collision;
}

// Detect collision between ray and sphere
rf_extern bool rf_check_collision_ray_sphere(rf_ray ray, rf_vec3 center, float radius)
{
    bool collision = 0;

    rf_vec3 ray_sphere_pos = rf_vec3_sub(center, ray.position);
    float distance = rf_vec3_len(ray_sphere_pos);
    float vector = rf_vec3_dot_product(ray_sphere_pos, ray.direction);
    float d = radius * radius - (distance * distance - vector * vector);

    if (d >= 0.0f) collision = 1;

    return collision;
}

// Detect collision between ray and sphere with extended parameters and collision point detection
rf_extern bool rf_check_collision_ray_sphere_ex(rf_ray ray, rf_vec3 center, float radius, rf_vec3 *collision_point)
{
    bool collision = 0;

    rf_vec3 ray_sphere_pos = rf_vec3_sub(center, ray.position);
    float distance = rf_vec3_len(ray_sphere_pos);
    float vector = rf_vec3_dot_product(ray_sphere_pos, ray.direction);
    float d = radius * radius - (distance * distance - vector * vector);

    if (d >= 0.0f) collision = 1;

// Check if ray origin is inside the sphere to calculate the correct collision point
    float collision_distance = 0;

    if (distance < radius) collision_distance = vector + sqrtf(d);
    else collision_distance = vector - sqrtf(d);

// Calculate collision point
    rf_vec3 c_point = rf_vec3_add(ray.position, rf_vec3_scale(ray.direction, collision_distance));

    collision_point->x = c_point.x;
    collision_point->y = c_point.y;
    collision_point->z = c_point.z;

    return collision;
}

// Detect collision between ray and bounding box
rf_extern bool rf_check_collision_ray_box(rf_ray ray, rf_bounding_box box)
{
    bool collision = 0;

    float t[8];
    t[0] = (box.min.x - ray.position.x) / ray.direction.x;
    t[1] = (box.max.x - ray.position.x) / ray.direction.x;
    t[2] = (box.min.y - ray.position.y) / ray.direction.y;
    t[3] = (box.max.y - ray.position.y) / ray.direction.y;
    t[4] = (box.min.z - ray.position.z) / ray.direction.z;
    t[5] = (box.max.z - ray.position.z) / ray.direction.z;
    t[6] = (float) fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
    t[7] = (float) fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

    collision = !(t[7] < 0 || t[6] > t[7]);

    return collision;
}

// Get collision info between ray and triangle. Note: Based on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
rf_extern rf_ray_hit_info rf_collision_ray_triangle(rf_ray ray, rf_vec3 p1, rf_vec3 p2, rf_vec3 p3)
{
    rf_ray_hit_info result = {0};
    rf_vec3 edge1  = {0};
    rf_vec3 edge2  = {0};
    rf_vec3 p      = {0};
    rf_vec3 q      = {0};
    rf_vec3 tv     = {0};
    float det      = 0;
    float inv_det  = 0;
    float u        = 0;
    float v        = 0;
    float t        = 0;
    double epsilon = 0.000001; // Just a small number

    // Find vectors for two edges sharing V1
    edge1 = rf_vec3_sub(p2, p1);
    edge2 = rf_vec3_sub(p3, p1);

    // Begin calculating determinant - also used to calculate u parameter
    p = rf_vec3_cross_product(ray.direction, edge2);

    // If determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
    det = rf_vec3_dot_product(edge1, p);

    // Avoid culling!
    if ((det > -epsilon) && (det < epsilon)) {
        return result;
    }

    inv_det = 1.0f / det;

    // Calculate distance from V1 to ray origin
    tv = rf_vec3_sub(ray.position, p1);

    // Calculate u parameter and test bound
    u = rf_vec3_dot_product(tv, p) * inv_det;

    // The intersection lies outside of the triangle
    if ((u < 0.0f) || (u > 1.0f)) {
        return result;
    }

    // Prepare to test v parameter
    q = rf_vec3_cross_product(tv, edge1);

    // Calculate V parameter and test bound
    v = rf_vec3_dot_product(ray.direction, q) * inv_det;

    // The intersection lies outside of the triangle
    if ((v < 0.0f) || ((u + v) > 1.0f)) {
        return result;
    }

    t = rf_vec3_dot_product(edge2, q) * inv_det;

    if (t > epsilon)
    {
        // rf_ray hit, get hit point and normal
        result.hit = 1;
        result.distance = t;
        result.hit = 1;
        result.normal = rf_vec3_normalize(rf_vec3_cross_product(edge1, edge2));
        result.position = rf_vec3_add(ray.position, rf_vec3_scale(ray.direction, t));
    }

    return result;
}

// Get collision info between ray and ground plane (Y-normal plane)
rf_extern rf_ray_hit_info rf_collision_ray_ground(rf_ray ray, float ground_height)
{
    rf_ray_hit_info result = {0};
    double epsilon = 0.000001; // Just a small number

    if (fabs(ray.direction.y) > epsilon)
    {
        float distance = (ray.position.y - ground_height) / -ray.direction.y;

        if (distance >= 0.0)
        {
            result.hit = 1;
            result.distance = distance;
            result.normal = (rf_vec3) {0.0, 1.0, 0.0};
            result.position = rf_vec3_add(ray.position, rf_vec3_scale(ray.direction, distance));
        }
    }

    return result;
}