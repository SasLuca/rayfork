#ifndef RAYFORK_BASE64_H
#define RAYFORK_BASE64_H

#include "rayfork_common.h"
#include "rayfork_allocator.h"

typedef struct rf_base64_output
{
    int size;
    unsigned char* buffer;
} rf_base64_output;

RF_API int rf_get_size_base64(const unsigned char* input);
RF_API rf_base64_output rf_decode_base64(const unsigned char* input, rf_allocator allocator);

#endif // RAYFORK_BASE64_H