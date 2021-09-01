#ifndef RAYFORK_FOUNDATION_STR_H
#define RAYFORK_FOUNDATION_STR_H

#include <rayfork/foundation/basicdef.h>
#include <rayfork/foundation/allocator.h>

#define rf_invalid_codepoint ('?')

typedef uint32_t rf_rune;
typedef uint64_t rf_utf8_char;
typedef uint64_t rf_utf16_char;

typedef struct rf_str_stats
{
    rf_int bytes_processed;
    rf_int invalid_bytes;
    rf_int valid_rune_count;
    rf_int total_rune_count;
} rf_str_stats;

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

typedef struct rf_str
{
    char* data;
    rf_int size;
} rf_str;

typedef struct rf_str_buf
{
    rf_str       str;
    rf_int       capacity;
    rf_allocator allocator;
    rf_valid_t   valid;
} rf_str_buf;

typedef struct rf_str_to_float_result
{
    float      value;
    rf_valid_t valid;
} rf_str_to_float_result;

typedef struct rf_str_to_int_result
{
    rf_int     value;
    rf_valid_t valid;
} rf_str_to_int_result;

rf_extern bool rf_char_is_ascii(char src);
rf_extern bool rf_rune_is_ascii(rf_rune src);
rf_extern bool rf_str_is_ascii(rf_str str);

rf_extern rf_int rf_to_digit_ascii(char src);
rf_extern char   rf_to_upper_ascii(char src);
rf_extern char   rf_to_lower_ascii(char src);
rf_extern bool   rf_is_lower_ascii(char src);
rf_extern bool   rf_is_upper_ascii(char src);
rf_extern bool   rf_is_alpha_ascii(char src);
rf_extern bool   rf_is_digit_ascii(char src);
rf_extern bool   rf_is_alnum_ascii(char src);
rf_extern bool   rf_is_space_ascii(char src);

rf_extern bool rf_str_is_lower_ascii(rf_str src);
rf_extern bool rf_str_is_upper_ascii(rf_str src);
rf_extern bool rf_str_is_alpha_ascii(rf_str src);
rf_extern bool rf_str_is_digit_ascii(rf_str src);
rf_extern bool rf_str_is_alnum_ascii(rf_str src);
rf_extern bool rf_str_is_space_ascii(rf_str src);

// str
rf_extern rf_str     rf_cstr(const char* src);
rf_extern rf_int     rf_str_len(rf_str src);
rf_extern rf_valid_t rf_str_valid(rf_str src);

rf_extern rf_str rf_str_advance_ascii(rf_str src, rf_int amount);
rf_extern rf_str rf_str_eat_spaces_ascii(rf_str src);
rf_extern rf_str rf_str_get_char_ascii(rf_str src, rf_int n);
rf_extern rf_str rf_str_sub_ascii(rf_str, rf_int begin, rf_int end);

rf_extern rf_int rf_str_cmp(rf_str a, rf_str b);
rf_extern bool   rf_str_match_prefix(rf_str a, rf_str b);
rf_extern bool   rf_str_match_suffix(rf_str a, rf_str b);
rf_extern bool   rf_str_match(rf_str a, rf_str b);
rf_extern bool   rf_str_contains(rf_str a, rf_str b);
rf_extern rf_int rf_str_find_first(rf_str haystack, rf_str needle);
rf_extern rf_int rf_str_find_last(rf_str haystack, rf_str needle);

rf_extern rf_str rf_str_eat_until(rf_str src, rf_str until);
rf_extern rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by);
rf_extern rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by);
rf_extern rf_str rf_str_get_first_split(rf_str src, rf_str split_by);
rf_extern rf_str rf_str_get_last_split(rf_str src, rf_str split_by);
rf_extern rf_str rf_str_get_nth_split(rf_str src, rf_str split_by, rf_int n);

typedef char (*rf_str_filter)(rf_str s);

rf_extern rf_str rf_str_eat_ascii_f(rf_str src, rf_str_filter filter);
rf_extern rf_str rf_str_get_char_ascii_f(rf_str src, rf_int n, rf_str_filter filter);
rf_extern rf_str rf_str_sub_ascii_f(rf_str, rf_int begin, rf_int end, rf_str_filter filter);

rf_extern rf_int rf_str_cmp_f(rf_str a, rf_str b, rf_str_filter filter);
rf_extern bool   rf_str_match_prefix_f(rf_str a, rf_str b, rf_str_filter filter);
rf_extern bool   rf_str_match_suffix_f(rf_str a, rf_str b, rf_str_filter filter);
rf_extern bool   rf_str_match_f(rf_str a, rf_str b, rf_str_filter filter);
rf_extern bool   rf_str_contains_f(rf_str a, rf_str b, rf_str_filter filter);
rf_extern rf_int rf_str_find_first_f(rf_str haystack, rf_str needle, rf_str_filter filter);
rf_extern rf_int rf_str_find_last_f(rf_str haystack, rf_str needle, rf_str_filter filter);

rf_extern rf_str rf_str_eat_until_f(rf_str src, rf_str until, rf_str_filter filter);
rf_extern rf_str rf_str_pop_first_split_f(rf_str* src, rf_str split_by, rf_str_filter filter);
rf_extern rf_str rf_str_pop_last_split_f(rf_str* src, rf_str split_by, rf_str_filter filter);
rf_extern rf_str rf_str_get_first_split_f(rf_str src, rf_str split_by, rf_str_filter filter);
rf_extern rf_str rf_str_get_last_split_f(rf_str src, rf_str split_by, rf_str_filter filter);
rf_extern rf_str rf_str_get_nth_split_f(rf_str src, rf_str split_by, rf_int n, rf_str_filter filter);

// str_buf
rf_extern rf_str_buf rf_str_buf_make(rf_int initial_amount, rf_allocator allocator);
rf_extern rf_str_buf rf_str_buf_clone(rf_str_buf src, rf_allocator allocator);
rf_extern rf_str     rf_str_buf_to_str(rf_str_buf src);
rf_extern void       rf_str_buf_free(rf_str_buf* src);

rf_extern void     rf_str_to_upper_ascii(rf_str_buf* src);
rf_extern void     rf_str_to_lower_ascii(rf_str_buf* src);

rf_extern rf_int rf_str_buf_remaining_capacity(rf_str_buf src);
rf_extern void   rf_str_buf_reserve(rf_str_buf* src, rf_int size);
rf_extern void   rf_str_buf_ensure_capacity_for(rf_str_buf* src, rf_int size);

rf_extern void   rf_str_buf_append(rf_str_buf* src, rf_str it);
rf_extern void   rf_str_buf_prepend(rf_str_buf* src, rf_str it);

rf_extern void   rf_str_buf_insert_ascii(rf_str_buf* src, rf_str str_to_insert, rf_int insert_at);
rf_extern void   rf_str_buf_remove_range_ascii(rf_str_buf* src, rf_int begin, rf_int end);

rf_extern rf_str_to_int_result   rf_str_to_int_in_base(rf_str src, rf_int base);
rf_extern rf_str_to_int_result   rf_str_to_int(rf_str src);
rf_extern rf_str_to_float_result rf_str_to_float(rf_str src);

#if defined(rf_c11) && 0

    #define rf_generic_str(src) (_Generic((src), char: (rf_str) { &src, 1 }, const char*: rf_cstr(src), rf_str: (src), rf_str_buf: (src).str))

    /* rf_rune rf_str_get_rune(rf_str src, rf_int n)                                            */ #define rf_str_get_rune(src, n)                        rf_str_get_rune(rf_generic_str(src), n)

    /* rf_int     rf_str_len(rf_str src)                                                        */ #define rf_str_len(src)                                rf_str_len(rf_generic_str(src))
    /* rf_valid_t rf_str_valid(rf_str src)                                                      */ #define rf_str_valid(src)                              rf_str_valid(rf_generic_str(src))

    /* rf_str rf_str_advance_ascii(rf_str src, rf_int amount)                                   */ #define rf_str_advance_ascii(src, amount)              rf_str_advance_ascii(rf_generic_str(src), amount)
    /* rf_str rf_str_eat_spaces_ascii(rf_str src)                                               */ #define rf_str_eat_spaces_ascii(src)                   rf_str_eat_spaces_ascii(rf_generic_str(src))
    /* rf_str rf_str_get_char_ascii(rf_str src, rf_int n)                                       */ #define rf_str_get_char_ascii(src, n)                  rf_str_get_char_ascii(rf_generic_str(src), n)
    /* rf_str rf_str_sub_ascii(rf_str src, rf_int begin, rf_int end)                            */ #define rf_str_sub_ascii(src, begin, end)              rf_str_sub_ascii(rf_generic_str(src), begin, end)

    /* int     rf_str_cmp(rf_str a, rf_str b);                                                  */ #define rf_str_cmp(a, b)                               rf_str_cmp(rf_generic_str(a), rf_generic_str(b))
    /* bool    rf_str_match_prefix(rf_str a, rf_str b);                                         */ #define rf_str_match_prefix(a, b)                      rf_str_match_prefix(rf_generic_str(a), rf_generic_str(b))
    /* bool    rf_str_match_suffix(rf_str a, rf_str b);                                         */ #define rf_str_match_suffix(a, b)                      rf_str_match_suffix(rf_generic_str(a), rf_generic_str(b))
    /* bool    rf_str_match(rf_str a, rf_str b);                                                */ #define rf_str_match(a, b)                             rf_str_match(rf_generic_str(a), rf_generic_str(b))
    /* bool    rf_str_contains(rf_str a, rf_str b);                                             */ #define rf_str_contains(a, b)                          rf_str_contains(rf_generic_str(a), rf_generic_str(b))
    /* rf_int  rf_str_find_first(rf_str haystack, rf_str needle);                               */ #define rf_str_find_first(haystack, needle)            rf_str_find_first(rf_generic_str(haystack), rf_generic_str(needle))
    /* rf_int  rf_str_find_last(rf_str haystack, rf_str needle);                                */ #define rf_str_find_last(haystack, needle)             rf_str_find_last(rf_generic_str(haystack), rf_generic_str(needle))

    /* rf_str rf_str_eat_until(rf_str src, rf_str until)                                        */ #define rf_str_eat_until(src, until)                   rf_str_eat_until(rf_generic_str(src), rf_generic_str(until)
    /* rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by)                              */ #define rf_str_pop_first_split(src, split_by)          rf_str_pop_first_split(rf_generic_str(src), rf_generic_str(split_by)
    /* rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by)                               */ #define rf_str_pop_last_split(src, split_by)           rf_str_pop_last_split(rf_generic_str(src), rf_generic_str(split_by)
    /* rf_str rf_str_get_first_split(rf_str src, rf_str split_by)                               */ #define rf_str_get_first_split(src, split_by)          rf_str_get_first_split(rf_generic_str(src), rf_generic_str(split_by)
    /* rf_str rf_str_get_last_split(rf_str src, rf_str split_by)                                */ #define rf_str_get_last_split(src, split_by)           rf_str_get_last_split(rf_generic_str(src), rf_generic_str(split_by)
    /* rf_str rf_str_get_nth_split(rf_str src, rf_str split_by, rf_int n)                       */ #define rf_str_get_nth_split(src, split_by, n)         rf_str_get_nth_split(rf_generic_str(src), rf_generic_str(split_by), n)

    /* rf_str_to_int_result   rf_str_to_int_in_base(rf_str src, rf_int base)                    */ #define rf_str_to_int_in_base(src, base)               rf_str_to_int_in_base(rf_generic_str(src), base)
    /* rf_str_to_int_result   rf_str_to_int(rf_str src)                                         */ #define rf_str_to_int(src)                             rf_str_to_int(rf_generic_str(src))
    /* rf_str_to_float_result rf_str_to_float(rf_str src)                                       */ #define rf_str_to_float(src)                           rf_str_to_float(rf_generic_str(src))

    /* bool rf_char_is_ascii(char c) / bool rf_str_is_ascii(rf_str str)                         */ #define rf_is_ascii(src)                               rf_str_is_ascii(rf_generic_str(src))

#endif

#define rf_for_str_split(iter, src, split_by)                                         \
    for (                                                                             \
        rf_str rf_macro_var(src_) = (src),                                            \
        (iter) = rf_str_pop_first_split(&rf_macro_var(src_), split_by),               \
        rf_macro_var(split_by_) = split_by;                                           \
                                                                                      \
        rf_str_valid(rf_macro_var(src_));                                             \
                                                                                      \
        (iter) = rf_str_pop_first_split(&rf_macro_var(src_), rf_macro_var(split_by_)) \
        )

#endif // RAYFORK_FOUNDATION_STR_H