#ifndef RAYFORK_MODEL_INTERNAL_CGLTF_UTILS_H
#define RAYFORK_MODEL_INTERNAL_CGLTF_UTILS_H

#include "rayfork/foundation/allocator.h"
#include "rayfork/foundation/io.h"
#include "cgltf/cgltf.h"

rf_extern cgltf_result rf_cgltf_io_read(const cgltf_memory_options* memory_options, const cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data);

rf_extern void rf_cgltf_io_release(const cgltf_memory_options* memory_options, const cgltf_file_options* file_options, void* data);

rf_extern void* rf_cgltf_alloc(void* user, cgltf_size size);

rf_extern void rf_cgltf_free(void* user, void* ptr);

#endif // RAYFORK_MODEL_INTERNAL_CGLTF_UTILS_H