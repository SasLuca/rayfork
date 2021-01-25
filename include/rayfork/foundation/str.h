#ifndef RAYFORK_FOUNDATION_STR_H
#define RAYFORK_FOUNDATION_STR_H

#include "basicdef.h"
#include "chars.h"

typedef struct rf_str
{
    char* data;
    rf_int size;
} rf_str;

rf_extern rf_valid_t rf_str_valid(rf_str src);

rf_extern rf_int rf_str_len(rf_str src);

rf_extern rf_str rf_cstr(const char* src);

rf_extern rf_str rf_str_advance_b(rf_str src, rf_int amount);

rf_extern rf_str rf_str_eat_spaces(rf_str src);

rf_extern rf_rune rf_str_get_rune(rf_str src, rf_int n);

rf_extern rf_utf8_char rf_str_get_utf8_char(rf_str src, rf_int n);

rf_extern rf_str rf_str_sub_utf8(rf_str, rf_int begin, rf_int end);

rf_extern rf_str rf_str_sub_b(rf_str, rf_int begin, rf_int end);

rf_extern int rf_str_cmp(rf_str a, rf_str b);

rf_extern bool rf_str_match_prefix(rf_str, rf_str);

rf_extern bool rf_str_match_suffix(rf_str, rf_str);

rf_extern bool rf_str_match(rf_str, rf_str);

rf_extern rf_int rf_str_find_first(rf_str haystack, rf_str needle);

rf_extern rf_int rf_str_find_last(rf_str haystack, rf_str needle);

rf_extern bool rf_str_contains(rf_str, rf_str);

rf_extern rf_utf8_char rf_rune_to_utf8_char(rf_rune src);

rf_extern rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by);

rf_extern rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by);

rf_extern rf_int rf_str_to_int(rf_str src);

rf_extern float rf_str_to_float(rf_str src);

#define rf_for_str_split(iter, src, split_by) \
    rf_str rf_macro_var(src_) = src;\
    rf_str iter = rf_str_pop_first_split(&rf_macro_var(src_), split_by);\
    rf_str rf_macro_var(split_by_) = split_by;\
    for (;rf_str_valid(rf_macro_var(src_)); iter = rf_str_pop_first_split(&rf_macro_var(src_), rf_macro_var(split_by_)))

#endif // RAYFORK_FOUNDATION_STR_H