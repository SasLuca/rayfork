#ifndef RAYFORK_MODEL_INTERNAL_TINYOBJ_UTILS_H
#define RAYFORK_MODEL_INTERNAL_TINYOBJ_UTILS_H

#include "rayfork/foundation/basicdef.h"

rf_extern void rf_tinyobj_file_reader_callback(const char* filename, int is_mtl, const char* obj_filename, char** buf, size_t* len);

#endif // RAYFORK_MODEL_INTERNAL_TINYOBJ_UTILS_H