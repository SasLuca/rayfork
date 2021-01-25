#ifndef RAYFORK_FOUNDATION_CHARS_H
#define RAYFORK_FOUNDATION_CHARS_H

#include "basicdef.h"
#include "allocator.h"

#define rf_invalid_codepoint ('?')

typedef uint32_t rf_rune;
typedef uint64_t rf_utf8_char;

typedef struct rf_utf8_stats
{
    rf_int bytes_processed;
    rf_int invalid_bytes;
    rf_int valid_rune_count;
    rf_int total_rune_count;
} rf_utf8_stats;

typedef struct rf_decoded_rune
{
    rf_rune    codepoint;
    rf_int     bytes_processed;
    rf_valid_t valid;
} rf_decoded_rune;

typedef struct rf_decoded_string
{
    rf_rune*   codepoints;
    rf_int     size;
    rf_int     invalid_bytes_count;
    rf_valid_t valid;
} rf_decoded_string;

rf_extern rf_decoded_rune rf_decode_utf8_char(const char* src, rf_int size);

rf_extern rf_utf8_stats rf_count_utf8_chars(const char* src, rf_int size);

rf_extern rf_utf8_stats rf_count_utf8_chars_til(const char* src, rf_int size, rf_int n);

rf_extern rf_decoded_string rf_decode_utf8_to_buffer(const char* src, rf_int size, rf_rune* dst, rf_int dst_size);

rf_extern rf_decoded_string rf_decode_utf8(const char* src, rf_int size, rf_allocator allocator);

rf_extern rf_int rf_to_digit(char c);

rf_extern char rf_to_upper(char c);

rf_extern char rf_to_lower(char c);

rf_extern bool rf_is_ascii(char c);

rf_extern bool rf_is_lower(char c);

rf_extern bool rf_is_upper(char c);

rf_extern bool rf_is_alpha(char c);

rf_extern bool rf_is_digit(char c);

rf_extern bool rf_is_alnum(char c);

rf_extern bool rf_is_space(char c);

#endif // RAYFORK_FOUNDATION_CHARS_H