#include "rayfork/gfx/camera.h"
#include "rayfork/math/maths.h"

// Get world coordinates from screen coordinates
rf_extern rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view)
{
    rf_vec3 result = {0.0f, 0.0f, 0.0f};

    // Calculate unproject matrix (multiply view patrix by rf_ctx->gl_ctx.projection matrix) and invert it
    rf_mat mat_viewProj = rf_mat_mul(view, proj);
    mat_viewProj = rf_mat_invert(mat_viewProj);

    // Create quaternion from source point
    rf_quaternion quat = {source.x, source.y, source.z, 1.0f};

    // Multiply quat point by unproject matrix
    quat = rf_quaternion_transform(quat, mat_viewProj);

    // Normalized world points in vectors
    result.x = quat.x / quat.w;
    result.y = quat.y / quat.w;
    result.z = quat.z / quat.w;

    return result;
}

// Returns a ray trace from mouse position
rf_extern rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera)
{
    rf_ray ray = {0};

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f * mouse_position.x) / (float) screen_size.width - 1.0f;
    float y = 1.0f - (2.0f * mouse_position.y) / (float) screen_size.height;
    float z = 1.0f;

    // Store values in a vector
    rf_vec3 device_coords = { x, y, z };

    // Calculate view matrix from camera look at
    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);

    rf_mat mat_proj = rf_mat_identity();

    if (camera.type == rf_camera_type_perspective)
    {
        // Calculate projection matrix from perspective
        mat_proj = rf_mat_perspective(camera.fovy * rf_deg2rad, ((double) screen_size.width / (double) screen_size.height), 0.01, 1000.0);
    }
    else if (camera.type == rf_camera_type_orthographic)
    {
        float aspect = (float) screen_size.width / (float) screen_size.height;
        double top = camera.fovy / 2.0;
        double right = top * aspect;

        // Calculate projection matrix from orthographic
        mat_proj = rf_mat_ortho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    rf_vec3 near_point = rf_unproject((rf_vec3) {device_coords.x, device_coords.y, 0.0f}, mat_proj, mat_view);
    rf_vec3 far_point  = rf_unproject((rf_vec3) {device_coords.x, device_coords.y, 1.0f}, mat_proj, mat_view);

    // Unproject the mouse cursor in the near plane.
    // We need this as the source position because orthographic projects, compared to perspect doesn't have a
    // convergence point, meaning that the "eye" of the camera is more like a plane than a point.
    rf_vec3 camera_plane_pointer_pos = rf_unproject((rf_vec3) {device_coords.x, device_coords.y, -1.0f}, mat_proj,
                                                    mat_view);

    // Calculate normalized direction vector
    rf_vec3 direction = rf_vec3_normalize(rf_vec3_sub(far_point, near_point));

    if (camera.type == rf_camera_type_perspective)
    {
        ray.position = camera.position;
    }
    else if (camera.type == rf_camera_type_orthographic)
    {
        ray.position = camera_plane_pointer_pos;
    }

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}

// Get transform matrix for camera
rf_extern rf_mat rf_get_camera_matrix(rf_camera3d camera)
{
    return rf_mat_look_at(camera.position, camera.target, camera.up);
}

// Returns camera 2d transform matrix
rf_extern rf_mat rf_get_camera_matrix2d(rf_camera2d camera)
{
    rf_mat mat_transform = {0};
// The camera in world-space is set by
//   1. Move it to target
//   2. Rotate by -rotation and scale by (1/zoom)
//      When setting higher scale, it's more intuitive for the world to become bigger (= camera become smaller),
//      not for the camera getting bigger, hence the invert. Same deal with rotation.
//   3. Move it by (-offset);
//      Offset defines target transform relative to screen, but since we're effectively "moving" screen (camera)
//      we need to do it into opposite direction (inverse transform)

// Having camera transform in world-space, inverse of it gives the rf_gfxobal_model_view transform.
// Since (A*B*C)' = C'*B'*A', the rf_gfxobal_model_view is
//   1. Move to offset
//   2. Rotate and Scale
//   3. Move by -target
    rf_mat mat_origin = rf_mat_translate(-camera.target.x, -camera.target.y, 0.0f);
    rf_mat mat_rotation = rf_mat_rotate((rf_vec3) {0.0f, 0.0f, 1.0f}, camera.rotation * rf_deg2rad);
    rf_mat mat_scale = rf_mat_scale(camera.zoom, camera.zoom, 1.0f);
    rf_mat mat_translation = rf_mat_translate(camera.offset.x, camera.offset.y, 0.0f);

    mat_transform = rf_mat_mul(rf_mat_mul(mat_origin, rf_mat_mul(mat_scale, mat_rotation)), mat_translation);

    return mat_transform;
}

// Returns the screen space position from a 3d world space position
rf_extern rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera)
{
    // Calculate projection matrix from perspective instead of frustum
    rf_mat mat_proj = rf_mat_identity();

    if (camera.type == rf_camera_type_perspective)
    {
        // Calculate projection matrix from perspective
        mat_proj = rf_mat_perspective(camera.fovy * rf_deg2rad, ((double) screen_size.width / (double) screen_size.height), 0.01, 1000.0);
    }
    else if (camera.type == rf_camera_type_orthographic)
    {
        float aspect = (float) screen_size.width / (float) screen_size.height;
        double top = camera.fovy / 2.0;
        double right = top * aspect;

    // Calculate projection matrix from orthographic
        mat_proj = rf_mat_ortho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Calculate view matrix from camera look at (and transpose it)
    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);

    // Convert world position vector to quaternion
    rf_quaternion world_pos = { position.x, position.y, position.z, 1.0f };

    // Transform world position to view
    world_pos = rf_quaternion_transform(world_pos, mat_view);

    // Transform result to projection (clip space position)
    world_pos = rf_quaternion_transform(world_pos, mat_proj);

    // Calculate normalized device coordinates (inverted y)
    rf_vec3 ndc_pos = {world_pos.x / world_pos.w, -world_pos.y / world_pos.w, world_pos.z / world_pos.w};

    // Calculate 2d screen position vector
    rf_vec2 screen_position = {(ndc_pos.x + 1.0f) / 2.0f * (float) screen_size.width,
                               (ndc_pos.y + 1.0f) / 2.0f * (float) screen_size.height};

    return screen_position;
}

// Returns the screen space position for a 2d camera world space position
rf_extern rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera)
{
    rf_mat mat_camera = rf_get_camera_matrix2d(camera);
    rf_vec3 transform = rf_vec3_transform((rf_vec3) {position.x, position.y, 0}, mat_camera);

    return (rf_vec2) {transform.x, transform.y};
}

// Returns the world space position for a 2d camera screen space position
rf_extern rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera)
{
    rf_mat inv_mat_camera = rf_mat_invert(rf_get_camera_matrix2d(camera));
    rf_vec3 transform = rf_vec3_transform((rf_vec3) {position.x, position.y, 0}, inv_mat_camera);

    return (rf_vec2) {transform.x, transform.y};
}