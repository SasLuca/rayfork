#ifndef RAYFORK_BUILTIN_CAMERA3D_H
#define RAYFORK_BUILTIN_CAMERA3D_H

#include "rayfork_camera.h"

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
    bool        is_camera_pan_control_key_down;    // Middle mouse button
    bool        is_camera_alt_control_key_down;    // Left Alt Key
    bool        is_camera_smooth_zoom_control_key; // Left Control Key
    bool        direction_keys[6];                 // 'W', 'S', 'D', 'A', 'E', 'Q'
} rf_input_state_for_update_camera;

RF_API void rf_set_camera3d_mode(rf_camera3d_state* state, rf_camera3d camera, rf_builtin_camera3d_mode mode);
RF_API void rf_update_camera3d(rf_camera3d* camera, rf_camera3d_state* state, rf_input_state_for_update_camera input_state);

#endif // RAYFORK_BUILTIN_CAMERA3D_H