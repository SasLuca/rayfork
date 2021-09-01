#ifndef RAYFORK_MATERIALS_H
#define RAYFORK_MATERIALS_H

#include "rayfork/gfx/gfx-types.h"
#include "rayfork/foundation/io.h"

rf_extern rf_material rf_load_default_material(rf_allocator allocator); // Load default material
rf_extern rf_materials_array rf_load_materials_from_mtl(const char* filename, rf_allocator allocator, rf_io_callbacks io); // Load materials from model file
rf_extern void rf_material_set_texture(rf_material* material, rf_material_map_type map_type, rf_texture2d texture); // Set texture for a material map type (rf_map_diffuse, rf_map_specular...)
rf_extern void rf_material_free(rf_material material, rf_allocator allocator); // Unload material from GPU memory (VRAM)

// Animations

#endif // RAYFORK_MATERIALS_H