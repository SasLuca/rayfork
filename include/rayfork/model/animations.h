#ifndef RAYFORK_MODEL_ANIMATIONS_H
#define RAYFORK_MODEL_ANIMATIONS_H

#include "rayfork/gfx/gfx-types.h"
#include "rayfork/foundation/io.h"

rf_extern rf_model_animation_array rf_load_model_animations_from_iqm_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io);
rf_extern rf_model_animation_array rf_load_model_animations_from_iqm(const unsigned char* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator); // Load model animations from file
rf_extern void rf_model_animation_update(rf_model model, rf_model_animation anim, int frame); // Update model animation pose
rf_extern rf_valid_t rf_model_animation_valid(rf_model model, rf_model_animation anim); // Check model animation skeleton match
rf_extern rf_ray_hit_info rf_collision_ray_model(rf_ray ray, struct rf_model model); // Get collision info between ray and model
rf_extern void rf_model_animation_free(rf_model_animation anim, rf_allocator allocator); // Unload animation data

// mesh generation functions

#endif // RAYFORK_MODEL_ANIMATIONS_H