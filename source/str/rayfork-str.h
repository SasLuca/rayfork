#ifndef RAYFORK_STRBUF_H
#define RAYFORK_STRBUF_H

#include "rayfork-core.h"

#define RF_INVALID_CODEPOINT '?' 

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

typedef struct rf_str
{
    char* data;
    rf_int size;
} rf_str;

typedef struct rf_strbuf
{
    char*        data;
    rf_int       size;
    rf_int       capacity;
    rf_allocator allocator;
    rf_bool      valid;
} rf_strbuf;

#pragma region unicode
RF_API rf_decoded_rune   rf_decode_utf8_char(const char* src, rf_int size);

RF_API rf_utf8_stats     rf_count_utf8_chars(const char* src, rf_int size);

RF_API rf_utf8_stats     rf_count_utf8_chars_til(const char* src, rf_int size, rf_int n);

RF_API rf_decoded_string rf_decode_utf8_to_buffer(const char* src, rf_int size, rf_rune* dst, rf_int dst_size);

RF_API rf_decoded_string rf_decode_utf8(const char* src, rf_int size, rf_allocator allocator);
#pragma endregion

#pragma region strbuf
RF_API rf_strbuf rf_strbuf_make_ex(rf_int initial_amount, rf_allocator allocator);

RF_API rf_strbuf rf_strbuf_clone_ex(rf_strbuf buf, rf_allocator allocator);

RF_API rf_str rf_strbuf_to_str(rf_strbuf src);

RF_API rf_int rf_strbuf_remaining_capacity(const rf_strbuf* this_buf);

RF_API void rf_strbuf_reserve(rf_strbuf* this_buf, rf_int size);

RF_API void rf_strbuf_ensure_capacity_for(rf_strbuf* this_buf, rf_int size);

RF_API void rf_strbuf_append(rf_strbuf* this_buf, rf_str it);

RF_API void rf_strbuf_prepend(rf_strbuf* this_buf, rf_str it);

RF_API void rf_strbuf_insert_utf8(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at);

RF_API void rf_strbuf_insert_b(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at);

RF_API void rf_strbuf_remove_range_utf8(rf_strbuf* this_buf, rf_int begin, rf_int end);

RF_API void rf_strbuf_remove_range_b(rf_strbuf* this_buf, rf_int begin, rf_int end);

RF_API void rf_strbuf_free(rf_strbuf* this_buf);
#pragma endregion

#pragma region str
RF_API rf_bool rf_str_valid(rf_str src);

RF_API rf_int rf_str_len(rf_str src);

RF_API rf_str rf_cstr(const char* src);

RF_API rf_rune rf_str_get_rune(rf_str src, rf_int n);

RF_API rf_utf8_char rf_str_get_utf8_char(rf_str src, rf_int n);

RF_API rf_str rf_str_sub_utf8(rf_str, rf_int begin, rf_int end);

RF_API rf_str rf_str_sub_b(rf_str, rf_int begin, rf_int end);

RF_API int rf_str_cmp(rf_str a, rf_str b);

RF_API rf_bool rf_str_match_prefix(rf_str, rf_str);

RF_API rf_bool rf_str_match_suffix(rf_str, rf_str);

RF_API rf_bool rf_str_match(rf_str, rf_str);

RF_API rf_int rf_str_find_first(rf_str haystack, rf_str needle);

RF_API rf_int rf_str_find_last(rf_str haystack, rf_str needle);

RF_API rf_bool rf_str_contains(rf_str, rf_str);

RF_API rf_utf8_char rf_rune_to_utf8_char(rf_rune src);

RF_API rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by);

RF_API rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by);

#define rf_for_str_split(iter, src, split_by) \
    rf_str RF_MACRO_VAR(src_) = src;\
    rf_str iter = rf_str_pop_first_split(&RF_MACRO_VAR(src_), split_by);\
    rf_str RF_MACRO_VAR(split_by_) = split_by;\
    for (;rf_str_valid(RF_MACRO_VAR(src_)); iter = rf_str_pop_first_split(&RF_MACRO_VAR(src_), RF_MACRO_VAR(split_by_)))
#pragma endregion

#endif // RAYFORK_STRBUF_H