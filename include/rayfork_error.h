#ifndef RAYFORK_ERROR_H
#define RAYFORK_ERROR_H

#include "rayfork_common.h"

typedef enum rf_error_type
{
    RF_NO_ERROR,
    RF_BAD_ARGUMENT,
    RF_BAD_ALLOC,
    RF_BAD_IO,
    RF_BAD_BUFFER_SIZE,
    RF_BAD_FORMAT,
    RF_LIMIT_REACHED,
    RF_STBI_FAILED,
    RF_STBTT_FAILED,
    RF_UNSUPPORTED,
} rf_error_type;

#endif // RAYFORK_ERROR_H
