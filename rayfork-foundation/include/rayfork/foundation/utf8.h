#ifndef RAYFORK_FOUNDATION_UTF8_H
#define RAYFORK_FOUNDATION_UTF8_H

#include <rayfork/foundation/str.h>
#include <rayfork/foundation/allocator.h>

rf_extern void rf_str_to_upper_utf8(rf_str_buf* src);
rf_extern void rf_str_to_lower_utf8(rf_str_buf* src);

rf_extern rf_str rf_str_advance_utf8(rf_str src, rf_int amount);
rf_extern rf_str rf_str_eat_spaces_utf8(rf_str src);
rf_extern rf_str rf_str_get_char_utf8(rf_str src, rf_int n);
rf_extern rf_str rf_str_sub_utf8(rf_str, rf_int begin, rf_int end);

rf_extern void rf_str_buf_insert_utf8(rf_str_buf* src, rf_str str_to_insert, rf_int insert_at);
rf_extern void rf_str_buf_remove_range_utf8(rf_str_buf* src, rf_int begin, rf_int end);

rf_extern rf_rune           rf_utf8_to_rune(rf_str src, rf_int char_index);
rf_extern rf_utf8_char      rf_rune_to_utf8(rf_rune src);
rf_extern rf_str_stats      rf_check_utf8(rf_str src);
rf_extern rf_str_stats      rf_check_utf8_til(rf_str src, rf_int n);
rf_extern rf_decoded_string rf_utf8_to_runes_in_buffer(rf_str src, rf_rune* dst, rf_int dst_size);
rf_extern rf_decoded_string rf_utf8_to_runes(rf_str src, rf_allocator allocator);

rf_extern rf_int  rf_str_cmp_utf8(rf_str a, rf_str b);
rf_extern bool    rf_str_match_prefix_utf8(rf_str a, rf_str b);
rf_extern bool    rf_str_match_suffix_utf8(rf_str a, rf_str b);
rf_extern bool    rf_str_match_utf8(rf_str a, rf_str b);
rf_extern bool    rf_str_contains_utf8(rf_str a, rf_str b);
rf_extern rf_int  rf_str_find_first_utf8(rf_str haystack, rf_str needle);
rf_extern rf_int  rf_str_find_last_utf8(rf_str haystack, rf_str needle);

rf_extern rf_str rf_str_eat_until_utf8(rf_str src, rf_str until);
rf_extern rf_str rf_str_pop_first_split_utf8(rf_str* src, rf_str split_by);
rf_extern rf_str rf_str_pop_last_split_utf8(rf_str* src, rf_str split_by);
rf_extern rf_str rf_str_get_first_split_utf8(rf_str src, rf_str split_by);
rf_extern rf_str rf_str_get_last_split_utf8(rf_str src, rf_str split_by);
rf_extern rf_str rf_str_get_nth_split_utf8(rf_str src, rf_str split_by, rf_int n);

#endif // RAYFORK_FOUNDATION_UTF8_H