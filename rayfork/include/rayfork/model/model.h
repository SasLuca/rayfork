#ifndef RAYFORK_MODEL_H
#define RAYFORK_MODEL_H

#include "rayfork/gfx/gfx-types.h"
#include "rayfork/foundation/io.h"

rf_extern rf_bounding_box rf_mesh_bounding_box(rf_mesh mesh); // Compute mesh bounding box limits
rf_extern void rf_mesh_compute_tangents(rf_mesh* mesh, rf_allocator allocator, rf_allocator temp_allocator); // Compute mesh tangents
rf_extern void rf_mesh_compute_binormals(rf_mesh* mesh); // Compute mesh binormals
rf_extern void rf_unload_mesh(rf_mesh mesh, rf_allocator allocator); // Unload mesh from memory (RAM and/or VRAM)

rf_extern rf_model rf_load_model(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io);
rf_extern rf_model rf_load_model_from_obj(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io); // Load model from files (meshes and materials)
rf_extern rf_model rf_load_model_from_iqm(const char* path_to_file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io); // Load model from files (meshes and materials)
rf_extern rf_model rf_load_model_from_gltf(const char* path_to_file_cstr, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io); // Load model from files (meshes and materials)
rf_extern rf_model rf_load_model_from_mesh(rf_mesh mesh, rf_allocator allocator); // Load model from generated mesh. Note: The function takes ownership of the mesh in model.meshes[0]

rf_extern void rf_model_mesh_set_material(rf_model* model, int mesh_id, int material_id); // Set material for a mesh
rf_extern void rf_model_free(rf_model model, rf_allocator allocator); // Unload model from memory (RAM and/or VRAM)

#endif // RAYFORK_MODEL_H