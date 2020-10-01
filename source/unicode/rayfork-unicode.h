#ifndef RAYFORK_UNICODE_H
#define RAYFORK_UNICODE_H

#include "rayfork-core.h"
#include "stdint.h"

#define RF_INVALID_CODEPOINT '?'

typedef uint32_t rf_rune;

typedef struct rf_decoded_utf8_stats
{
    rf_int bytes_processed;
    rf_int invalid_bytes;
    rf_int valid_rune_count;
    rf_int total_rune_count;
} rf_decoded_utf8_stats;

typedef struct rf_decoded_rune
{
    rf_rune  codepoint;
    rf_int   bytes_processed;
    rf_bool  valid;
} rf_decoded_rune;

typedef struct rf_decoded_string
{
    rf_rune* codepoints;
    rf_int   size;
    rf_int   invalid_bytes_count;
    rf_bool  valid;
} rf_decoded_string;

RF_API rf_decoded_rune rf_decode_utf8_char(const char* text, rf_int len);
RF_API rf_decoded_utf8_stats rf_count_utf8_chars(const char* text, rf_int len);
RF_API rf_decoded_string rf_decode_utf8_to_buffer(const char* text, rf_int len, rf_rune* dst, rf_int dst_size);
RF_API rf_decoded_string rf_decode_utf8(const char* text, rf_int len, rf_allocator allocator);

#endif // RAYFORK_UNICODE_H