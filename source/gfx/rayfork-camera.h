#ifndef RAYFORK_CAMERA_H
#define RAYFORK_CAMERA_H

#include "rayfork-core.h"

// Camera projection modes
typedef enum rf_camera_type
{
    RF_CAMERA_PERSPECTIVE = 0,
    RF_CAMERA_ORTHOGRAPHIC
} rf_camera_type;

typedef struct rf_camera2d
{
    rf_vec2 offset;   // Camera offset (displacement from target)
    rf_vec2 target;   // Camera target (rotation and zoom origin)
    float   rotation; // Camera rotation in degrees
    float   zoom;     // Camera zoom (scaling), should be 1.0f by default
} rf_camera2d;

typedef struct rf_camera3d
{
    rf_camera_type type;     // Camera type, defines projection types: RF_CAMERA_PERSPECTIVE or RF_CAMERA_ORTHOGRAPHIC
    rf_vec3        position; // Camera position
    rf_vec3        target;   // Camera target it looks-at
    rf_vec3        up;       // Camera up vector (rotation over its axis)
    float          fovy;     // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
} rf_camera3d;

rf_public rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view); // Get world coordinates from screen coordinates
rf_public rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera); // Returns a ray trace from mouse position
rf_public rf_mat rf_get_camera_matrix(rf_camera3d camera); // Get transform matrix for camera
rf_public rf_mat rf_get_camera_matrix2d(rf_camera2d camera); // Returns camera 2d transform matrix
rf_public rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera); // Returns the screen space position from a 3d world space position
rf_public rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera); // Returns the screen space position for a 2d camera world space position
rf_public rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera); // Returns the world space position for a 2d camera screen space position

#pragma region builtin camera

// Camera system modes
typedef enum rf_builtin_camera3d_mode
{
    RF_CAMERA_CUSTOM = 0,
    RF_CAMERA_FREE,
    RF_CAMERA_ORBITAL,
    RF_CAMERA_FIRST_PERSON,
    RF_CAMERA_THIRD_PERSON
} rf_builtin_camera3d_mode;

typedef struct rf_camera3d_state
{
    rf_vec2 camera_angle;         // rf_camera3d angle in plane XZ
    float camera_target_distance; // rf_camera3d distance from position to target
    float player_eyes_position;
    rf_builtin_camera3d_mode camera_mode; // Current camera mode
    int swing_counter; // Used for 1st person swinging movement
    rf_vec2 previous_mouse_position;
} rf_camera3d_state;

typedef struct rf_input_state_for_update_camera
{
    rf_vec2     mouse_position;
    int         mouse_wheel_move;                  // Mouse wheel movement Y
    rf_bool     is_camera_pan_control_key_down;    // Middle mouse button
    rf_bool     is_camera_alt_control_key_down;    // Left Alt Key
    rf_bool     is_camera_smooth_zoom_control_key; // Left Control Key
    rf_bool     direction_keys[6];                 // 'W', 'S', 'D', 'A', 'E', 'Q'
} rf_input_state_for_update_camera;

rf_public void rf_set_camera3d_mode(rf_camera3d_state* state, rf_camera3d camera, rf_builtin_camera3d_mode mode);
rf_public void rf_update_camera3d(rf_camera3d* camera, rf_camera3d_state* state, rf_input_state_for_update_camera input_state);

#pragma endregion

#endif // RAYFORK_CAMERA_H