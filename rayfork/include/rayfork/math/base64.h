#ifndef RAYFORK_BASE64_H
#define RAYFORK_BASE64_H

#include "rayfork/foundation/allocator.h"

typedef struct rf_base64_output
{
    rf_int size;
    char* buffer;
} rf_base64_output;

rf_extern rf_int rf_get_size_base64(const char* input);
rf_extern rf_base64_output rf_decode_base64(const char* input, rf_allocator allocator);

#endif // RAYFORK_BASE64_H