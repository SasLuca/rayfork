#include "rayfork/math/base64.h"

rf_extern rf_int rf_get_size_base64(const char* input)
{
    rf_int size = 0;

    for (rf_int i = 0; input[4 * i] != 0; i++)
    {
        if (input[4 * i + 3] == '=')
        {
            if (input[4 * i + 2] == '=')
            {
                size += 1;
            }
            else
            {
                size += 2;
            }
        }
        else
        {
            size += 3;
        }
    }

    return size;
}

rf_extern rf_base64_output rf_decode_base64(const char* input, rf_allocator allocator)
{
    static const char rf_base64_table[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 62, 0, 0, 0, 63, 52, 53,
        54, 55, 56, 57, 58, 59, 60, 61, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
        5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 0, 0, 0, 0, 0, 0, 26, 27, 28,
        29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51
    };

    rf_base64_output result = {
        .size   = rf_get_size_base64(input),
        .buffer = rf_alloc(allocator, result.size),
    };

    for (rf_int i = 0; i < result.size / 3; i++)
    {
        char a = rf_base64_table[input[4 * i + 0]];
        char b = rf_base64_table[input[4 * i + 1]];
        char c = rf_base64_table[input[4 * i + 2]];
        char d = rf_base64_table[input[4 * i + 3]];

        result.buffer[3 * i + 0] = (a << 2) | (b >> 4);
        result.buffer[3 * i + 1] = (b << 4) | (c >> 2);
        result.buffer[3 * i + 2] = (c << 6) | d;
    }

    rf_int n = result.size / 3;

    if (result.size % 3 == 1)
    {
        char a = rf_base64_table[input[4 * n + 0]];
        char b = rf_base64_table[input[4 * n + 1]];

        result.buffer[result.size - 1] = (a << 2) | (b >> 4);
    }
    else if (result.size % 3 == 2)
    {
        char a = rf_base64_table[input[4 * n + 0]];
        char b = rf_base64_table[input[4 * n + 1]];
        char c = rf_base64_table[input[4 * n + 2]];

        result.buffer[result.size - 2] = (a << 2) | (b >> 4);
        result.buffer[result.size - 1] = (b << 4) | (c >> 2);
    }

    return result;
}
