#include "rayfork/gfx/builtin-camera.h"

// Select camera mode (multiple camera modes available)
rf_extern void rf_set_camera3d_mode(rf_camera3d_state* state, rf_camera3d camera, rf_builtin_camera3d_mode mode)
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
rf_extern void rf_update_camera3d(rf_camera3d* camera, rf_camera3d_state* state, rf_input_state_for_update_camera input_state)
{
    // rf_camera3d mouse movement sensitivity
    #define rf_camera_mouse_move_sensitivity 0.003f
    #define rf_camera_mouse_scroll_sensitivity 1.5f

    // FREE_CAMERA
    #define rf_camera_free_mouse_sensitivity 0.01f
    #define rf_camera_free_distance_min_clamp 0.3f
    #define rf_camera_free_distance_max_clamp 120.0f
    #define rf_camera_free_min_clamp 85.0f
    #define rf_camera_free_max_clamp -85.0f
    #define rf_camera_free_smooth_zoom_sensitivity 0.05f
    #define rf_camera_free_panning_divider 5.1f

    // ORBITAL_CAMERA
    #define rf_camera_orbital_speed 0.01f // Radians per frame

    // FIRST_PERSON
    //#define CAMERA_FIRST_PERSON_MOUSE_SENSITIVITY           0.003f
    #define rf_camera_first_person_focus_distance 25.0f
    #define rf_camera_first_person_min_clamp 85.0f
    #define rf_camera_first_person_max_clamp -85.0f

    #define rf_camera_first_person_step_trigonometric_divider 5.0f
    #define rf_camera_first_person_step_divider 30.0f
    #define rf_camera_first_person_waving_divider 200.0f

    // THIRD_PERSON
    //#define CAMERA_THIRD_PERSON_MOUSE_SENSITIVITY           0.003f
    #define rf_camera_third_person_distance_clamp 1.2f
    #define rf_camera_third_person_min_clamp 5.0f
    #define rf_camera_third_person_max_clamp -85.0f
    #define rf_camera_third_person_offset (rf_vec3) { 0.4f, 0.0f, 0.0f }

    // PLAYER (used by camera)
    #define rf_player_movement_sensitivity 20.0f

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

    // static float player_eyes_position = 1.85f;

    // TODO: CRF_INTERNAL rf_ctx->gl_ctx.camera_target_distance and rf_ctx->gl_ctx.camera_angle here

    // Mouse movement detection
    rf_vec2 mouse_position_delta = { 0.0f, 0.0f };
    rf_vec2 mouse_position = input_state.mouse_position;
    int mouse_wheel_move = input_state.mouse_wheel_move;

    // Keys input detection
    bool pan_key = input_state.is_camera_pan_control_key_down;
    bool alt_key = input_state.is_camera_alt_control_key_down;
    bool szoom_key = input_state.is_camera_smooth_zoom_control_key;

    bool direction[6] = {0};
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
            if ((state->camera_target_distance < rf_camera_free_distance_max_clamp) && (mouse_wheel_move < 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * rf_camera_mouse_scroll_sensitivity);

                if (state->camera_target_distance > rf_camera_free_distance_max_clamp) {
                    state->camera_target_distance = rf_camera_free_distance_max_clamp;
                }
            }
            // Camera looking down
            // TODO: Review, weird comparisson of rf_ctx->gl_ctx.camera_target_distance == 120.0f?
            else if ((camera->position.y > camera->target.y) && (state->camera_target_distance == rf_camera_free_distance_max_clamp) && (mouse_wheel_move < 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
            }
            else if ((camera->position.y > camera->target.y) && (camera->target.y >= 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;

                // if (camera->target.y < 0) camera->target.y = -0.001;
            }
            else if ((camera->position.y > camera->target.y) && (camera->target.y < 0) && (mouse_wheel_move > 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * rf_camera_mouse_scroll_sensitivity);
                if (state->camera_target_distance < rf_camera_free_distance_min_clamp) {
                    state->camera_target_distance = rf_camera_free_distance_min_clamp;
                }
            }
            // Camera looking up
            // TODO: Review, weird comparisson of rf_ctx->gl_ctx.camera_target_distance == 120.0f?
            else if ((camera->position.y < camera->target.y) && (state->camera_target_distance == rf_camera_free_distance_max_clamp) && (mouse_wheel_move < 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
            }
            else if ((camera->position.y < camera->target.y) && (camera->target.y <= 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * rf_camera_mouse_scroll_sensitivity / state->camera_target_distance;

                // if (camera->target.y > 0) camera->target.y = 0.001;
            }
            else if ((camera->position.y < camera->target.y) && (camera->target.y > 0) && (mouse_wheel_move > 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * rf_camera_mouse_scroll_sensitivity);
                if (state->camera_target_distance < rf_camera_free_distance_min_clamp) {
                    state->camera_target_distance = rf_camera_free_distance_min_clamp;
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
                        state->camera_target_distance += (mouse_position_delta.y * rf_camera_free_smooth_zoom_sensitivity);
                    }
                    else
                    {
                        // Camera rotation
                        state->camera_angle.x += mouse_position_delta.x*-rf_camera_free_mouse_sensitivity;
                        state->camera_angle.y += mouse_position_delta.y*-rf_camera_free_mouse_sensitivity;

                        // Angle clamp
                        if (state->camera_angle.y > rf_camera_free_min_clamp * rf_deg2rad) {
                            state->camera_angle.y = rf_camera_free_min_clamp * rf_deg2rad;
                        }
                        else if (state->camera_angle.y < rf_camera_free_max_clamp * rf_deg2rad) {
                            state->camera_angle.y = rf_camera_free_max_clamp * rf_deg2rad;
                        }
                    }
                }
                else
                {
                    // Camera panning
                    camera->target.x += ((mouse_position_delta.x*-rf_camera_free_mouse_sensitivity) * cosf(state->camera_angle.x) + (mouse_position_delta.y * rf_camera_free_mouse_sensitivity) * sinf(state->camera_angle.x) * sinf(state->camera_angle.y)) * (state->camera_target_distance / rf_camera_free_panning_divider);
                    camera->target.y += ((mouse_position_delta.y * rf_camera_free_mouse_sensitivity) * cosf(state->camera_angle.y)) * (state->camera_target_distance / rf_camera_free_panning_divider);
                    camera->target.z += ((mouse_position_delta.x * rf_camera_free_mouse_sensitivity) * sinf(state->camera_angle.x) + (mouse_position_delta.y * rf_camera_free_mouse_sensitivity) * cosf(state->camera_angle.x) * sinf(state->camera_angle.y)) * (state->camera_target_distance / rf_camera_free_panning_divider);
                }
            }

            // Update camera position with changes
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;
            camera->position.y = ((state->camera_angle.y <= 0.0f)? 1 : -1)*sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        case RF_CAMERA_ORBITAL:
        {
            state->camera_angle.x += rf_camera_orbital_speed; // Camera orbit angle
            state->camera_target_distance -= (mouse_wheel_move * rf_camera_mouse_scroll_sensitivity); // Camera zoom

            // Camera distance clamp
            if (state->camera_target_distance < rf_camera_third_person_distance_clamp) {
                state->camera_target_distance = rf_camera_third_person_distance_clamp;
            }

            // Update camera position with changes
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;
            camera->position.y = ((state->camera_angle.y <= 0.0f)? 1 : -1)*sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        case RF_CAMERA_FIRST_PERSON:
        {
            camera->position.x += (sinf(state->camera_angle.x) * direction[rf_move_back ] -
                                   sinf(state->camera_angle.x) * direction[rf_move_front] -
                                   cosf(state->camera_angle.x) * direction[rf_move_left ] +
                                   cosf(state->camera_angle.x) * direction[rf_move_right]) / rf_player_movement_sensitivity;

            camera->position.y += (sinf(state->camera_angle.y) * direction[rf_move_front] -
                                   sinf(state->camera_angle.y) * direction[rf_move_back ] +
                                   1.0f * direction[rf_move_up] - 1.0f * direction[rf_move_down]) / rf_player_movement_sensitivity;

            camera->position.z += (cosf(state->camera_angle.x) *direction[rf_move_back ] -
                                   cosf(state->camera_angle.x) *direction[rf_move_front] +
                                   sinf(state->camera_angle.x) *direction[rf_move_left ] -
                                   sinf(state->camera_angle.x) *direction[rf_move_right]) / rf_player_movement_sensitivity;

            bool is_moving = 0; // Required for swinging

            for (rf_int i = 0; i < 6; i++)
            {
                if (direction[i])
                {
                    is_moving = 1; break;
                }
            }

            // Camera orientation calculation
            state->camera_angle.x += (mouse_position_delta.x*-rf_camera_mouse_move_sensitivity);
            state->camera_angle.y += (mouse_position_delta.y*-rf_camera_mouse_move_sensitivity);

            // Angle clamp
            if (state->camera_angle.y > rf_camera_first_person_min_clamp * rf_deg2rad)
            {
                state->camera_angle.y = rf_camera_first_person_min_clamp * rf_deg2rad;
            }
            else if (state->camera_angle.y < rf_camera_first_person_max_clamp * rf_deg2rad)
            {
                state->camera_angle.y = rf_camera_first_person_max_clamp * rf_deg2rad;
            }

            // Camera is always looking at player
            camera->target.x = camera->position.x - sinf(state->camera_angle.x) * rf_camera_first_person_focus_distance;
            camera->target.y = camera->position.y + sinf(state->camera_angle.y) * rf_camera_first_person_focus_distance;
            camera->target.z = camera->position.z - cosf(state->camera_angle.x) * rf_camera_first_person_focus_distance;

            if (is_moving)
            {
                state->swing_counter++;
            }

            // Camera position update
            // NOTE: On RF_CAMERA_FIRST_PERSON player Y-movement is limited to player 'eyes position'
            camera->position.y = state->player_eyes_position - sinf(state->swing_counter / rf_camera_first_person_step_trigonometric_divider) / rf_camera_first_person_step_divider;

            camera->up.x = sinf(state->swing_counter/(rf_camera_first_person_step_trigonometric_divider * 2)) / rf_camera_first_person_waving_divider;
            camera->up.z = -sinf(state->swing_counter/(rf_camera_first_person_step_trigonometric_divider * 2)) / rf_camera_first_person_waving_divider;


        } break;

        case RF_CAMERA_THIRD_PERSON:
        {
            camera->position.x += (sinf(state->camera_angle.x)*direction[rf_move_back] -
                                   sinf(state->camera_angle.x)*direction[rf_move_front] -
                                   cosf(state->camera_angle.x)*direction[rf_move_left] +
                                   cosf(state->camera_angle.x)*direction[rf_move_right]) / rf_player_movement_sensitivity;

            camera->position.y += (sinf(state->camera_angle.y)*direction[rf_move_front] -
                                   sinf(state->camera_angle.y)*direction[rf_move_back] +
                                   1.0f*direction[rf_move_up] - 1.0f*direction[rf_move_down]) / rf_player_movement_sensitivity;

            camera->position.z += (cosf(state->camera_angle.x)*direction[rf_move_back] -
                                   cosf(state->camera_angle.x)*direction[rf_move_front] +
                                   sinf(state->camera_angle.x)*direction[rf_move_left] -
                                   sinf(state->camera_angle.x)*direction[rf_move_right]) / rf_player_movement_sensitivity;

            // Camera orientation calculation
            state->camera_angle.x += (mouse_position_delta.x*-rf_camera_mouse_move_sensitivity);
            state->camera_angle.y += (mouse_position_delta.y*-rf_camera_mouse_move_sensitivity);

            // Angle clamp
            if (state->camera_angle.y > rf_camera_third_person_min_clamp * rf_deg2rad)
            {
                state->camera_angle.y = rf_camera_third_person_min_clamp * rf_deg2rad;
            }
            else if (state->camera_angle.y < rf_camera_third_person_max_clamp * rf_deg2rad)
            {
                state->camera_angle.y = rf_camera_third_person_max_clamp * rf_deg2rad;
            }

            // Camera zoom
            state->camera_target_distance -= (mouse_wheel_move * rf_camera_mouse_scroll_sensitivity);

            // Camera distance clamp
            if (state->camera_target_distance < rf_camera_third_person_distance_clamp)
            {
                state->camera_target_distance = rf_camera_third_person_distance_clamp;
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