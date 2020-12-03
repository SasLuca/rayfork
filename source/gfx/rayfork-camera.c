#include "rayfork-camera.h"

// Get world coordinates from screen coordinates
rf_public rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view)
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
rf_public rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera)
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

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        mat_proj = rf_mat_perspective(camera.fovy * rf_deg2rad, ((double) screen_size.width / (double) screen_size.height), 0.01, 1000.0);
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
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

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        ray.position = camera.position;
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
    {
        ray.position = camera_plane_pointer_pos;
    }

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}

// Get transform matrix for camera
rf_public rf_mat rf_get_camera_matrix(rf_camera3d camera)
{
    return rf_mat_look_at(camera.position, camera.target, camera.up);
}

// Returns camera 2d transform matrix
rf_public rf_mat rf_get_camera_matrix2d(rf_camera2d camera)
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
rf_public rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera)
{
    // Calculate projection matrix from perspective instead of frustum
    rf_mat mat_proj = rf_mat_identity();

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        mat_proj = rf_mat_perspective(camera.fovy * rf_deg2rad, ((double) screen_size.width / (double) screen_size.height), 0.01, 1000.0);
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
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
rf_public rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera)
{
    rf_mat mat_camera = rf_get_camera_matrix2d(camera);
    rf_vec3 transform = rf_vec3_transform((rf_vec3) {position.x, position.y, 0}, mat_camera);

    return (rf_vec2) {transform.x, transform.y};
}

// Returns the world space position for a 2d camera screen space position
rf_public rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera)
{
    rf_mat inv_mat_camera = rf_mat_invert(rf_get_camera_matrix2d(camera));
    rf_vec3 transform = rf_vec3_transform((rf_vec3) {position.x, position.y, 0}, inv_mat_camera);

    return (rf_vec2) {transform.x, transform.y};
}

// Select camera mode (multiple camera modes available)
rf_public void rf_set_camera3d_mode(rf_camera3d_state* state, rf_camera3d camera, rf_builtin_camera3d_mode mode)
{
    rf_vec3 v1 = camera.position;
    rf_vec3 v2 = camera.target;

    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;

    state->camera_target_distance = sqrtf(dx*dx + dy*dy + dz*dz);

    rf_vec2 distance = { 0.0f, 0.0f };
    distance.x = sqrtf(dx*dx + dz*dz);
    distance.y = sqrtf(dx*dx + dy*dy);

    // rf_camera3d angle calculation
    state->camera_angle.x = asinf((float)fabs(dx)/distance.x); // rf_camera3d angle in plane XZ (0 aligned with Z, move positive CCW)
    state->camera_angle.y = -asinf((float)fabs(dy)/distance.y); // rf_camera3d angle in plane XY (0 aligned with X, move positive CW)

    state->player_eyes_position = camera.position.y;

    // Lock cursor for first person and third person cameras
    // if ((mode == rf_camera_first_person) || (mode == rf_camera_third_person)) DisableCursor();
    // else EnableCursor();

    state->camera_mode = mode;
}

// Update camera depending on selected mode
// NOTE: rf_camera3d controls depend on some raylib functions:
//       System: EnableCursor(), DisableCursor()
//       Mouse: IsMouseButtonDown(), GetMousePosition(), GetMouseWheelMove()
//       Keys:  IsKeyDown()
// TODO: Port to quaternion-based camera
rf_public void rf_update_camera3d(rf_camera3d* camera, rf_camera3d_state* state, rf_input_state_for_update_camera input_state)
{
    // rf_camera3d mouse movement sensitivity
    #define RF_CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f
    #define RF_CAMERA_MOUSE_SCROLL_SENSITIVITY 1.5f

    // FREE_CAMERA
    #define RF_CAMERA_FREE_MOUSE_SENSITIVITY 0.01f
    #define RF_CAMERA_FREE_DISTANCE_MIN_CLAMP 0.3f
    #define RF_CAMERA_FREE_DISTANCE_MAX_CLAMP 120.0f
    #define RF_CAMERA_FREE_MIN_CLAMP 85.0f
    #define RF_CAMERA_FREE_MAX_CLAMP -85.0f
    #define RF_CAMERA_FREE_SMOOTH_ZOOM_SENSITIVITY 0.05f
    #define RF_CAMERA_FREE_PANNING_DIVIDER 5.1f

    // ORBITAL_CAMERA
    #define RF_CAMERA_ORBITAL_SPEED 0.01f // Radians per frame

    // FIRST_PERSON
    //#define CAMERA_FIRST_PERSON_MOUSE_SENSITIVITY           0.003f
    #define RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE 25.0f
    #define RF_CAMERA_FIRST_PERSON_MIN_CLAMP 85.0f
    #define RF_CAMERA_FIRST_PERSON_MAX_CLAMP -85.0f

    #define RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER 5.0f
    #define RF_CAMERA_FIRST_PERSON_STEP_DIVIDER 30.0f
    #define RF_CAMERA_FIRST_PERSON_WAVING_DIVIDER 200.0f

    // THIRD_PERSON
    //#define CAMERA_THIRD_PERSON_MOUSE_SENSITIVITY           0.003f
    #define RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP 1.2f
    #define RF_CAMERA_THIRD_PERSON_MIN_CLAMP 5.0f
    #define RF_CAMERA_THIRD_PERSON_MAX_CLAMP -85.0f
    #define RF_CAMERA_THIRD_PERSON_OFFSET (rf_vec3) { 0.4f, 0.0f, 0.0f }

    // PLAYER (used by camera)
    #define RF_PLAYER_MOVEMENT_SENSITIVITY 20.0f

    // rf_camera3d move modes (first person and third person cameras)
    typedef enum rf_camera_move
    {
        rf_move_front = 0,
        rf_move_back,
        rf_move_right,
        rf_move_left,
        rf_move_up,
        rf_move_down
    } rf_camera_move;

    // rf_internal float player_eyes_position = 1.85f;

    // TODO: CRF_INTERNAL rf_ctx->gl_ctx.camera_target_distance and rf_ctx->gl_ctx.camera_angle here

    // Mouse movement detection
    rf_vec2 mouse_position_delta = { 0.0f, 0.0f };
    rf_vec2 mouse_position = input_state.mouse_position;
    int mouse_wheel_move = input_state.mouse_wheel_move;

    // Keys input detection
    rf_bool pan_key = input_state.is_camera_pan_control_key_down;
    rf_bool alt_key = input_state.is_camera_alt_control_key_down;
    rf_bool szoom_key = input_state.is_camera_smooth_zoom_control_key;

    rf_bool direction[6];
    direction[0] = input_state.direction_keys[0];
    direction[1] = input_state.direction_keys[1];
    direction[2] = input_state.direction_keys[2];
    direction[3] = input_state.direction_keys[3];
    direction[4] = input_state.direction_keys[4];
    direction[5] = input_state.direction_keys[5];

    // TODO: Consider touch inputs for camera

    if (state->camera_mode != RF_CAMERA_CUSTOM)
    {
        mouse_position_delta.x = mouse_position.x - state->previous_mouse_position.x;
        mouse_position_delta.y = mouse_position.y - state->previous_mouse_position.y;

        state->previous_mouse_position = mouse_position;
    }

    // Support for multiple automatic camera modes
    switch (state->camera_mode)
    {
        case RF_CAMERA_FREE:
        {
            // Camera zoom
            if ((state->camera_target_distance < RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) && (mouse_wheel_move < 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);

                if (state->camera_target_distance > RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) {
                    state->camera_target_distance = RF_CAMERA_FREE_DISTANCE_MAX_CLAMP;
                }
            }
            // Camera looking down
            // TODO: Review, weird comparisson of rf_ctx->gl_ctx.camera_target_distance == 120.0f?
            else if ((camera->position.y > camera->target.y) && (state->camera_target_distance == RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) && (mouse_wheel_move < 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
            }
            else if ((camera->position.y > camera->target.y) && (camera->target.y >= 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;

                // if (camera->target.y < 0) camera->target.y = -0.001;
            }
            else if ((camera->position.y > camera->target.y) && (camera->target.y < 0) && (mouse_wheel_move > 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);
                if (state->camera_target_distance < RF_CAMERA_FREE_DISTANCE_MIN_CLAMP) {
                    state->camera_target_distance = RF_CAMERA_FREE_DISTANCE_MIN_CLAMP;
                }
            }
            // Camera looking up
            // TODO: Review, weird comparisson of rf_ctx->gl_ctx.camera_target_distance == 120.0f?
            else if ((camera->position.y < camera->target.y) && (state->camera_target_distance == RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) && (mouse_wheel_move < 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
            }
            else if ((camera->position.y < camera->target.y) && (camera->target.y <= 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;

                // if (camera->target.y > 0) camera->target.y = 0.001;
            }
            else if ((camera->position.y < camera->target.y) && (camera->target.y > 0) && (mouse_wheel_move > 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);
                if (state->camera_target_distance < RF_CAMERA_FREE_DISTANCE_MIN_CLAMP) {
                    state->camera_target_distance = RF_CAMERA_FREE_DISTANCE_MIN_CLAMP;
                }
            }

            // Input keys checks
            if (pan_key)
            {
                if (alt_key) // Alternative key behaviour
                {
                    if (szoom_key)
                    {
                        // Camera smooth zoom
                        state->camera_target_distance += (mouse_position_delta.y * RF_CAMERA_FREE_SMOOTH_ZOOM_SENSITIVITY);
                    }
                    else
                    {
                        // Camera rotation
                        state->camera_angle.x += mouse_position_delta.x*-RF_CAMERA_FREE_MOUSE_SENSITIVITY;
                        state->camera_angle.y += mouse_position_delta.y*-RF_CAMERA_FREE_MOUSE_SENSITIVITY;

                        // Angle clamp
                        if (state->camera_angle.y > RF_CAMERA_FREE_MIN_CLAMP * rf_deg2rad) {
                            state->camera_angle.y = RF_CAMERA_FREE_MIN_CLAMP * rf_deg2rad;
                        }
                        else if (state->camera_angle.y < RF_CAMERA_FREE_MAX_CLAMP * rf_deg2rad) {
                            state->camera_angle.y = RF_CAMERA_FREE_MAX_CLAMP * rf_deg2rad;
                        }
                    }
                }
                else
                {
                    // Camera panning
                    camera->target.x += ((mouse_position_delta.x*-RF_CAMERA_FREE_MOUSE_SENSITIVITY) * cosf(state->camera_angle.x) + (mouse_position_delta.y * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * sinf(state->camera_angle.x) * sinf(state->camera_angle.y)) * (state->camera_target_distance / RF_CAMERA_FREE_PANNING_DIVIDER);
                    camera->target.y += ((mouse_position_delta.y * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * cosf(state->camera_angle.y)) * (state->camera_target_distance / RF_CAMERA_FREE_PANNING_DIVIDER);
                    camera->target.z += ((mouse_position_delta.x * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * sinf(state->camera_angle.x) + (mouse_position_delta.y * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * cosf(state->camera_angle.x) * sinf(state->camera_angle.y)) * (state->camera_target_distance / RF_CAMERA_FREE_PANNING_DIVIDER);
                }
            }

            // Update camera position with changes
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;
            camera->position.y = ((state->camera_angle.y <= 0.0f)? 1 : -1)*sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        case RF_CAMERA_ORBITAL:
        {
            state->camera_angle.x += RF_CAMERA_ORBITAL_SPEED; // Camera orbit angle
            state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY); // Camera zoom

            // Camera distance clamp
            if (state->camera_target_distance < RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP) {
                state->camera_target_distance = RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP;
            }

            // Update camera position with changes
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;
            camera->position.y = ((state->camera_angle.y <= 0.0f)? 1 : -1)*sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        case RF_CAMERA_FIRST_PERSON:
        {
            camera->position.x += (sinf(state->camera_angle.x)*direction[rf_move_back] -
                                   sinf(state->camera_angle.x)*direction[rf_move_front] -
                                   cosf(state->camera_angle.x)*direction[rf_move_left] +
                                   cosf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.y += (sinf(state->camera_angle.y)*direction[rf_move_front] -
                                   sinf(state->camera_angle.y)*direction[rf_move_back] +
                                   1.0f*direction[rf_move_up] - 1.0f*direction[rf_move_down]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.z += (cosf(state->camera_angle.x)*direction[rf_move_back] -
                                   cosf(state->camera_angle.x)*direction[rf_move_front] +
                                   sinf(state->camera_angle.x)*direction[rf_move_left] -
                                   sinf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            rf_bool is_moving = 0; // Required for swinging

            for (rf_int i = 0; i < 6; i++) if (direction[i]) { is_moving = 1; break; }

            // Camera orientation calculation
            state->camera_angle.x += (mouse_position_delta.x*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);
            state->camera_angle.y += (mouse_position_delta.y*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);

            // Angle clamp
            if (state->camera_angle.y > RF_CAMERA_FIRST_PERSON_MIN_CLAMP * rf_deg2rad) {
                state->camera_angle.y = RF_CAMERA_FIRST_PERSON_MIN_CLAMP * rf_deg2rad;
            }
            else if (state->camera_angle.y < RF_CAMERA_FIRST_PERSON_MAX_CLAMP * rf_deg2rad) {
                state->camera_angle.y = RF_CAMERA_FIRST_PERSON_MAX_CLAMP * rf_deg2rad;
            }

            // Camera is always looking at player
            camera->target.x = camera->position.x - sinf(state->camera_angle.x) * RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
            camera->target.y = camera->position.y + sinf(state->camera_angle.y) * RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
            camera->target.z = camera->position.z - cosf(state->camera_angle.x) * RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE;

            if (is_moving) {
                state->swing_counter++;
            }

            // Camera position update
            // NOTE: On RF_CAMERA_FIRST_PERSON player Y-movement is limited to player 'eyes position'
            camera->position.y = state->player_eyes_position - sinf(state->swing_counter / RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER) / RF_CAMERA_FIRST_PERSON_STEP_DIVIDER;

            camera->up.x = sinf(state->swing_counter/(RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) / RF_CAMERA_FIRST_PERSON_WAVING_DIVIDER;
            camera->up.z = -sinf(state->swing_counter/(RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) / RF_CAMERA_FIRST_PERSON_WAVING_DIVIDER;


        } break;

        case RF_CAMERA_THIRD_PERSON:
        {
            camera->position.x += (sinf(state->camera_angle.x)*direction[rf_move_back] -
                                   sinf(state->camera_angle.x)*direction[rf_move_front] -
                                   cosf(state->camera_angle.x)*direction[rf_move_left] +
                                   cosf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.y += (sinf(state->camera_angle.y)*direction[rf_move_front] -
                                   sinf(state->camera_angle.y)*direction[rf_move_back] +
                                   1.0f*direction[rf_move_up] - 1.0f*direction[rf_move_down]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.z += (cosf(state->camera_angle.x)*direction[rf_move_back] -
                                   cosf(state->camera_angle.x)*direction[rf_move_front] +
                                   sinf(state->camera_angle.x)*direction[rf_move_left] -
                                   sinf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            // Camera orientation calculation
            state->camera_angle.x += (mouse_position_delta.x*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);
            state->camera_angle.y += (mouse_position_delta.y*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);

            // Angle clamp
            if (state->camera_angle.y > RF_CAMERA_THIRD_PERSON_MIN_CLAMP * rf_deg2rad)
            {
                state->camera_angle.y = RF_CAMERA_THIRD_PERSON_MIN_CLAMP * rf_deg2rad;
            }
            else if (state->camera_angle.y < RF_CAMERA_THIRD_PERSON_MAX_CLAMP * rf_deg2rad)
            {
                state->camera_angle.y = RF_CAMERA_THIRD_PERSON_MAX_CLAMP * rf_deg2rad;
            }

            // Camera zoom
            state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);

            // Camera distance clamp
            if (state->camera_target_distance < RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP)
            {
                state->camera_target_distance = RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP;
            }

            // TODO: It seems camera->position is not correctly updated or some rounding issue makes the camera move straight to camera->target...
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;

            if (state->camera_angle.y <= 0.0f)
            {
                camera->position.y = sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            }
            else
            {
                camera->position.y = -sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            }

            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        default: break;
    }
}