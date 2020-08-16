#ifndef RAYFORK_UNICODE_H
#define RAYFORK_UNICODE_H

#include "rayfork-std.h"

#define RF_INVALID_CODEPOINT ('?')

typedef uint32_t rf_rune;

typedef struct rf_decoded_utf8_stats
{
    int bytes_processed;
    int invalid_bytes;
    int valid_rune_count;
    int total_rune_count;
} rf_decoded_utf8_stats;

typedef struct rf_decoded_rune
{
    rf_rune  codepoint;
    int      bytes_processed;
    bool     valid;
} rf_decoded_rune;

typedef struct rf_decoded_string
{
    rf_rune* codepoints;
    int      size;
    int      invalid_bytes_count;
    bool     valid;
} rf_decoded_string;

RF_API rf_decoded_rune rf_decode_utf8_char(const char* text, int len);
RF_API rf_decoded_utf8_stats rf_count_utf8_chars(const char* text, int len);
RF_API rf_decoded_string rf_decode_utf8_to_buffer(const char* text, int len, rf_rune* dst, int dst_size);
RF_API rf_decoded_string rf_decode_utf8(const char* text, int len, rf_allocator allocator);

#endif // RAYFORK_UNICODE_H