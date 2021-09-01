#ifndef RAYFORK_FOUNDATION_ERROR_H
#define RAYFORK_FOUNDATION_ERROR_H

#include "rayfork/foundation/basicdef.h"

typedef enum rf_error_type
{
    rf_no_error,
    rf_bad_argument,
    rf_bad_alloc,
    rf_bad_io,
    rf_bad_buffer_size,
    rf_bad_format,
    rf_limit_reached,
    rf_stbi_failed,
    rf_stbtt_failed,
    rf_unsupported,
} rf_error_type;

rf_extern rf_thread_local rf_error_type rf_global_last_recorded_error;

#endif // RAYFORK_FOUNDATION_ERROR_H