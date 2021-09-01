#include "rayfork/foundation/str.h"
#include "rayfork/foundation/utf8.h"
#include "rayfork/foundation/min-max.h"

#include "string.h"

rf_extern rf_valid_t rf_str_valid(rf_str src)
{
    rf_valid_t result = src.size != 0 && src.data; 
    return result;
}

rf_extern rf_int rf_str_len(rf_str src)
{
    rf_utf8_stats stats = rf_count_utf8_chars(src.data, src.size);

    return stats.total_rune_count;
}

rf_extern rf_str rf_cstr(const char* src)
{
    rf_int size = strlen(src);
    rf_str result = {
        .data = (char*) src,
        .size = size,
    };

    return result;
}

rf_extern rf_str rf_str_advance_b(rf_str src, rf_int amount)
{
    rf_str result = {0};
    result = rf_str_sub_b(src, amount, src.size);
    return result;
}

rf_extern rf_str rf_str_eat_spaces(rf_str src)
{
    while (rf_str_valid(src) && rf_ascii_is_space(src.data[0]))
    {
        src = rf_str_advance_b(src, 1);
    }

    return src;
}

rf_extern rf_rune rf_str_get_rune(rf_str src, rf_int n)
{
    rf_str target = rf_str_sub_utf8(src, n, 0);
    rf_rune result = rf_decode_utf8_char(target.data, target.size).codepoint;
    return result;
}

rf_extern rf_utf8_char rf_str_get_utf8_n(rf_str src, rf_int n)
{
    rf_utf8_char result = 0;
    rf_str target = rf_str_sub_utf8(src, n, 0);
    rf_utf8_stats stats = rf_count_utf8_chars_til(target.data, target.size, 1);
    if (stats.bytes_processed > 0 && stats.bytes_processed < 4) {
        memcpy(&result, src.data, stats.bytes_processed);
    }
    return result;
}

rf_extern rf_str rf_str_sub_utf8(rf_str src, rf_int begin, rf_int end)
{
    rf_str result = {0};

    if (begin < 0) {
        begin = src.size + begin;
    }

    if (end <= 0) {
        end = src.size + end;
    }

    if (rf_str_valid(src) && begin > 0 && begin < end && end <= src.size)
    {
        rf_utf8_stats stats = {0};

        // Find the begin utf8 position
        stats = rf_count_utf8_chars_til(src.data, src.size, begin);
        src.data += stats.bytes_processed;
        src.size -= stats.bytes_processed;

        // Find the end utf8 position
        stats = rf_count_utf8_chars_til(src.data, src.size, end - begin);

        result.data = src.data;
        result.size = stats.bytes_processed;
    }

    return result;
}

rf_extern rf_str rf_str_sub_b(rf_str src, rf_int begin, rf_int end)
{
    rf_str result = {0};

    if (begin < 0) {
        begin = src.size + begin;
    }

    if (end <= 0) {
        end = src.size + end;
    }

    if (rf_str_valid(src) && begin > 0 && begin < end && end <= src.size)
    {
        result.data = src.data + begin;
        result.size = end - begin;
    }

    return result;
}

rf_extern int rf_str_cmp(rf_str a, rf_str b)
{
    int result = memcmp(a.data, b.data, rf_min_i(a.size, b.size));
    return result;
}

rf_extern bool rf_str_match(rf_str a, rf_str b)
{
    if (a.size != b.size) return 0;
    int cmp = memcmp(a.data, b.data, a.size);
    return cmp == 0;
}

rf_extern bool rf_str_match_prefix(rf_str str, rf_str prefix)
{
    if (str.size < prefix.size) return 0;
    int cmp = memcmp(str.data, prefix.data, prefix.size);
    return cmp == 0;
}

rf_extern bool rf_str_match_suffix(rf_str str, rf_str suffix)
{
    if (str.size < suffix.size) return 0;
    rf_int offset = str.size - suffix.size;
    int cmp = memcmp(str.data + offset, suffix.data, suffix.size);
    return cmp == 0;
}

rf_extern rf_int rf_str_find_first(rf_str haystack, rf_str needle)
{
    rf_int result = rf_invalid_index;
    if (needle.size <= haystack.size)
    {
        rf_int char_ct = 0;
        for (rf_str sub = haystack; sub.size >= needle.size; sub = rf_str_sub_b(sub, 1, 0))
        {
            if (rf_str_match_prefix(sub, needle))
            {
                result = char_ct;
                break;
            }
            else
            {
                char_ct++;
            }
        }
    }
    return result;
}

rf_extern rf_int rf_str_find_last(rf_str haystack, rf_str needle)
{
    rf_int result = rf_invalid_index;
    if (needle.size <= haystack.size)
    {
        rf_int char_ct = haystack.size - 1;
        for (rf_str sub = haystack; rf_str_valid(sub); sub = rf_str_sub_b(sub, 0, -1))
        {
            if (rf_str_match_suffix(sub, needle))
            {
                result = char_ct;
                break;
            }
            else
            {
                char_ct--;
            }
        }
    }
    return result;
}

rf_extern bool rf_str_contains(rf_str haystack, rf_str needle)
{
    bool result = rf_str_find_first(haystack, needle) != rf_invalid_index;
    return result;
}

rf_extern rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by)
{
    rf_str result = {0};

    rf_int i = rf_str_find_first(*src, split_by);
    if (i != rf_invalid_index)
    {
        result.data  = src->data;
        result.size  = i;
        src->data   += i + split_by.size;
        src->size   -= i + split_by.size;
    }
    else
    {
        result = *src;
        *src = (rf_str){0};
    }

    return result;
}

rf_extern rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by)
{
    rf_str result = {0};

    rf_int i = rf_str_find_last(*src, split_by);
    if (i != rf_invalid_index)
    {
        result.data  = src->data + i;
        result.size  = src->size - i;
        src->size   -= src->size - i - split_by.size;
    }
    else
    {
        result = *src;
        *src = (rf_str){0};
    }

    return result;
}

rf_extern rf_str rf_str_get_first_split(rf_str src, rf_str split_by)
{
    rf_str result = rf_str_pop_first_split(&src, split_by);
    return result;
}

rf_extern rf_str rf_str_get_last_split(rf_str src, rf_str split_by)
{
    rf_str result = rf_str_pop_last_split(&src, split_by);
    return result;
}