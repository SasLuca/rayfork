#include "rayfork-str.h"
#include "string.h"

#pragma region unicode
/*
   Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found or the length is exhausted
   When a invalid UTF8 rf_byte is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
   Total number of bytes processed are returned as a parameter
   NOTE: the standard says U+FFFD should be returned in case of errors
   but that character is not supported by the default font in raylib
   TODO: optimize this code for speed!!
*/
rf_public rf_decoded_rune rf_decode_utf8_char(const char* src, rf_int size)
{
    /*
    UTF8 specs from https://www.ietf.org/rfc/rfc3629.txt
    Char. number range  |        UTF-8 byte sequence
      (hexadecimal)     |              (binary)
    --------------------+---------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */

    if (size < 1)
    {
        return (rf_decoded_rune) { RF_INVALID_CODEPOINT };
    }

    // The first UTF8 byte
    const int byte = (unsigned char)(src[0]);

    if (byte <= 0x7f)
    {
        // Only one byte (ASCII range x00-7F)
        const int code = src[0];

        // Codepoints after U+10ffff are invalid
        const int valid = code > 0x10ffff;

        return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 1, .valid = valid };
    }
    else if ((byte & 0xe0) == 0xc0)
    {
        if (size < 2)
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 1, };
        }

        // Two bytes
        // [0]xC2-DF    [1]UTF8-tail(x80-BF)
        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        if ((byte >= 0xc2) && (byte <= 0xdf))
        {
            const int code = ((byte & 0x1f) << 6) | (byte1 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;

            return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 2, .valid = valid };
        }
    }
    else if ((byte & 0xf0) == 0xe0)
    {
        if (size < 2)
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 1 };
        }

        // Three bytes
        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || (size < 3) || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        const unsigned char byte2 = src[2];

        // Check for unexpected sequence
        if ((byte2 == '\0') || ((byte2 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 3 };
        }

        /*
            [0]xE0    [1]xA0-BF       [2]UTF8-tail(x80-BF)
            [0]xE1-EC [1]UTF8-tail    [2]UTF8-tail(x80-BF)
            [0]xED    [1]x80-9F       [2]UTF8-tail(x80-BF)
            [0]xEE-EF [1]UTF8-tail    [2]UTF8-tail(x80-BF)
        */
        if (((byte == 0xe0) && !((byte1 >= 0xa0) && (byte1 <= 0xbf))) ||
            ((byte == 0xed) && !((byte1 >= 0x80) && (byte1 <= 0x9f))))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        if ((byte >= 0xe0) && (byte <= 0xef))
        {
            const int code = ((byte & 0xf) << 12) | ((byte1 & 0x3f) << 6) | (byte2 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;
            return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 3, .valid = valid };
        }
    }
    else if ((byte & 0xf8) == 0xf0)
    {
        // Four bytes
        if (byte > 0xf4 || size < 2)
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 1 };
        }

        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || (size < 3) || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        const unsigned char byte2 = src[2];

        // Check for unexpected sequence
        if ((byte2 == '\0') || (size < 4) || ((byte2 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 3 };
        }

        const unsigned char byte3 = src[3];

        // Check for unexpected sequence
        if ((byte3 == '\0') || ((byte3 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 4 };
        }

        /*
            [0]xF0       [1]x90-BF       [2]UTF8-tail  [3]UTF8-tail
            [0]xF1-F3    [1]UTF8-tail    [2]UTF8-tail  [3]UTF8-tail
            [0]xF4       [1]x80-8F       [2]UTF8-tail  [3]UTF8-tail
        */

        // Check for unexpected sequence
        if (((byte == 0xf0) && !((byte1 >= 0x90) && (byte1 <= 0xbf))) ||
            ((byte == 0xf4) && !((byte1 >= 0x80) && (byte1 <= 0x8f))))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        if (byte >= 0xf0)
        {
            const int code = ((byte & 0x7) << 18) | ((byte1 & 0x3f) << 12) | ((byte2 & 0x3f) << 6) | (byte3 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;
            return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 4, .valid = valid };
        }
    }

    return (rf_decoded_rune) { .codepoint = RF_INVALID_CODEPOINT, .bytes_processed = 1 };
}

rf_public rf_utf8_stats rf_count_utf8_chars(const char* src, rf_int size)
{
    rf_utf8_stats result = {0};

    if (src && size > 0)
    {
        while (size > 0)
        {
            rf_decoded_rune decoded_rune = rf_decode_utf8_char(src, size);

            src += decoded_rune.bytes_processed;
            size  -= decoded_rune.bytes_processed;

            result.bytes_processed  += decoded_rune.bytes_processed;
            result.invalid_bytes    += decoded_rune.valid ? 0 : decoded_rune.bytes_processed;
            result.valid_rune_count += decoded_rune.valid ? 1 : 0;
            result.total_rune_count += 1;
        }
    }

    return result;
}

rf_public rf_utf8_stats rf_count_utf8_chars_til(const char* src, rf_int size, rf_int n)
{
    rf_utf8_stats result = {0};

    if (src && size > 0)
    {
        while (size > 0 && n > 0)
        {
            rf_decoded_rune decoded_rune = rf_decode_utf8_char(src, size);

            src  += decoded_rune.bytes_processed;
            size -= decoded_rune.bytes_processed;
            n    -= 1;

            result.bytes_processed  += decoded_rune.bytes_processed;
            result.invalid_bytes    += decoded_rune.valid ? 0 : decoded_rune.bytes_processed;
            result.valid_rune_count += decoded_rune.valid ? 1 : 0;
            result.total_rune_count += 1;
        }
    }

    return result;
}

rf_public rf_decoded_string rf_decode_utf8_to_buffer(const char* src, rf_int size, rf_rune* dst, rf_int dst_size)
{
    rf_decoded_string result = {0};

    result.codepoints = dst;

    if (src && size > 0 && dst && dst_size > 0)
    {
        int dst_i = 0;
        int invalid_bytes = 0;

        while (size > 0 && dst_i < dst_size)
        {
            rf_decoded_rune decoding_result = rf_decode_utf8_char(src, size);

            // Count the invalid bytes
            if (!decoding_result.valid)
            {
                invalid_bytes += decoding_result.bytes_processed;
            }

            src += decoding_result.bytes_processed;
            size  -= decoding_result.bytes_processed;

            dst[dst_i++] = decoding_result.codepoint;
        }

        result.size = dst_i;
        result.valid = 1;
        result.invalid_bytes_count = invalid_bytes;
    }

    return result;
}

rf_public rf_decoded_string rf_decode_utf8(const char* src, rf_int size, rf_allocator allocator)
{
    rf_decoded_string result = {0};

    rf_rune* dst = rf_alloc(allocator, sizeof(rf_rune) * size);

    result = rf_decode_utf8_to_buffer(src, size, dst, size);

    return result;
}
#pragma endregion

#pragma region ascii
rf_int rf_str_to_int(rf_str src)
{
    rf_int result = 0;
    rf_int sign   = 1;

    src = rf_str_eat_spaces(src);

    if (rf_str_valid(src) && src.data[0] == '-')
    {
        sign = -1;
        src = rf_str_advance_b(src, 1);
    }

    while (rf_str_valid(src) && rf_is_digit(src.data[0]))
    {
        result *= 10;
        result += rf_to_digit(src.data[0]);
        src     = rf_str_advance_b(src, 1);
    }

    result *= sign;

    return result;
}

float rf_str_to_float(rf_str src);

int rf_to_digit(char c)
{
    int result = c - '0';
    return result;
}

char rf_to_upper(char c)
{
    char result = c;
    if (rf_is_upper(c)) {
        result = c + 'A' - 'a';
    }
    return result;
}

char rf_to_lower(char c)
{
    char result = c;
    if (rf_is_lower(c)) {
        result = c + 'a' - 'A';
    }
    return result;
}

rf_bool rf_is_ascii(char c)
{
    rf_bool result = 0;
    return result;
}

rf_bool rf_is_lower(char c)
{
    rf_bool result = c >= 'a' && c <= 'z';
    return result;
}

rf_bool rf_is_upper(char c)
{
    rf_bool result = c >= 'A' && c <= 'Z';
    return result;
}

rf_bool rf_is_alpha(char c)
{
    rf_bool result = rf_is_lower(c) || rf_is_upper(c);
    return result;
}

rf_bool rf_is_digit(char c)
{
    rf_bool result = c >= '0' && c <= '9';
    return result;
}

rf_bool rf_is_alnum(char c)
{
    rf_bool result = rf_is_alpha(c) && rf_is_alnum(c);
    return result;
}

rf_bool rf_is_space(char c)
{
    rf_bool result = c == ' ' || c == '\t';
    return result;
}
#pragma endregion

#pragma region strbuf
rf_public rf_strbuf rf_strbuf_make_ex(rf_int initial_amount, rf_allocator allocator)
{
    rf_strbuf result = {0};

    void* data = rf_alloc(allocator, initial_amount);

    if (data)
    {
        result.data      = data;
        result.capacity  = initial_amount;
        result.allocator = allocator;
        result.valid     = 1;
    }

    return result;
}

rf_public rf_strbuf rf_strbuf_clone_ex(rf_strbuf this_buf, rf_allocator allocator)
{
    rf_strbuf result = {0};

    if (this_buf.valid)
    {
        result = rf_strbuf_make_ex(this_buf.capacity, allocator);
        rf_strbuf_append(&result, rf_strbuf_to_str(this_buf));
    }

    return result;
}

rf_public rf_str rf_strbuf_to_str(rf_strbuf src)
{
    rf_str result = {0};

    if (src.valid)
    {
        result.data = src.data;
        result.size = src.size;
    }

    return result;
}

rf_public rf_int rf_strbuf_remaining_capacity(const rf_strbuf* this_buf)
{
    rf_int result = this_buf->capacity - this_buf->size;
    return result;
}

rf_public void rf_strbuf_reserve(rf_strbuf* this_buf, rf_int new_capacity)
{
    if (new_capacity > this_buf->capacity)
    {
        char* new_buf = rf_realloc(this_buf->allocator, this_buf->data, new_capacity, this_buf->capacity);
        if (new_buf)
        {
            this_buf->data = new_buf;
            this_buf->valid = 1;
        }
        else
        {
            this_buf->valid = 0;
        }
    }
}

rf_public void rf_strbuf_ensure_capacity_for(rf_strbuf* this_buf, rf_int size)
{
    rf_int remaining_capacity = rf_strbuf_remaining_capacity(this_buf);
    if (remaining_capacity < size)
    {
        // We either increase the buffer to capacity * 2 or to the necessary size to fit the size plus one for the null terminator
        rf_int amount_to_reserve = rf_max_i(this_buf->capacity * 2, this_buf->capacity + (size - remaining_capacity) + 1);
        rf_strbuf_reserve(this_buf, amount_to_reserve);
    }
}

rf_public void rf_strbuf_append(rf_strbuf* this_buf, rf_str it)
{
    rf_strbuf_ensure_capacity_for(this_buf, it.size);

    memcpy(this_buf->data + this_buf->size, it.data, it.size);
    this_buf->size += it.size;
    this_buf->data[this_buf->size] = 0;
}

rf_public void rf_strbuf_prepend(rf_strbuf* this_buf, rf_str it)
{
    rf_strbuf_ensure_capacity_for(this_buf, it.size);

    memmove(this_buf->data + it.size, this_buf->data, this_buf->size);
    memcpy(this_buf->data, it.data, it.size);
    this_buf->size += it.size;
    this_buf->data[this_buf->size] = 0;
}

rf_public void rf_strbuf_insert_utf8(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at)
{
    rf_strbuf_ensure_capacity_for(this_buf, str_to_insert.size);

    // Iterate over utf8 until we find the byte to insert at
    rf_int insertion_point = rf_count_utf8_chars_til(this_buf->data, this_buf->size, insert_at).bytes_processed;

    if (insertion_point && insertion_point < this_buf->size)
    {
        // Move all bytes from the insertion point ahead by the size of the string we need to insert
        {
            char*  dst = this_buf->data + insertion_point + str_to_insert.size;
            char*  src = this_buf->data + insertion_point;
            rf_int src_size = this_buf->size - insertion_point;
            memmove(dst, src, src_size);
        }

        // Copy the string to insert
        {
            char* dst = this_buf->data + insertion_point;
            memmove(dst, str_to_insert.data, str_to_insert.size);
        }

        this_buf->size += str_to_insert.size;
        this_buf->data[this_buf->size] = 0;
    }
}

rf_public void rf_strbuf_insert_b(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at)
{
    if (rf_str_valid(str_to_insert) && insert_at > 0)
    {
        rf_strbuf_ensure_capacity_for(this_buf, str_to_insert.size);

        // Move all bytes from the insertion point ahead by the size of the string we need to insert
        {
            char* dst = this_buf->data + insert_at + str_to_insert.size;
            char* src = this_buf->data + insert_at;
            rf_int src_size = this_buf->size - insert_at;
            memmove(dst, src, src_size);
        }

        // Copy the string to insert
        {
            char* dst = this_buf->data + insert_at;
            memcpy(dst, str_to_insert.data, str_to_insert.size);
        }

        this_buf->size += str_to_insert.size;
        this_buf->data[this_buf->size] = 0;
    }
}

rf_public void rf_strbuf_remove_range(rf_strbuf*, rf_int begin, rf_int end);

rf_public void rf_strbuf_remove_range_b(rf_strbuf*, rf_int begin, rf_int end);

rf_public void rf_strbuf_free(rf_strbuf* this_buf)
{
    rf_free(this_buf->allocator, this_buf->data);

    this_buf->size = 0;
    this_buf->capacity = 0;
}
#pragma endregion

#pragma region str
rf_public rf_bool rf_str_valid(rf_str src)
{
    return src.size != 0 && src.data;
}

rf_public rf_int rf_str_len(rf_str src)
{
    rf_utf8_stats stats = rf_count_utf8_chars(src.data, src.size);

    return stats.total_rune_count;
}

rf_public rf_str rf_cstr(const char* src)
{
    rf_int size = strlen(src);
    rf_str result = {
        .data = (char*) src,
        .size = size,
    };

    return result;
}

rf_public rf_str rf_str_advance_b(rf_str src, rf_int amount)
{
    rf_str result = {0};
    result = rf_str_sub_b(src, amount, src.size);
    return result;
}

rf_public rf_str rf_str_eat_spaces(rf_str src)
{
    while (rf_str_valid(src) && rf_is_space(src.data[0]))
    {
        src = rf_str_advance_b(src, 1);
    }

    return src;
}

rf_public rf_rune rf_str_get_rune(rf_str src, rf_int n)
{
    rf_str target = rf_str_sub_utf8(src, n, 0);
    rf_rune result = rf_decode_utf8_char(target.data, target.size).codepoint;
    return result;
}

rf_public rf_utf8_char rf_str_get_utf8_n(rf_str src, rf_int n)
{
    rf_utf8_char result = 0;
    rf_str target = rf_str_sub_utf8(src, n, 0);
    rf_utf8_stats stats = rf_count_utf8_chars_til(target.data, target.size, 1);
    if (stats.bytes_processed > 0 && stats.bytes_processed < 4) {
        memcpy(&result, src.data, stats.bytes_processed);
    }
    return result;
}

rf_public rf_str rf_str_sub_utf8(rf_str src, rf_int begin, rf_int end)
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

rf_public rf_str rf_str_sub_b(rf_str src, rf_int begin, rf_int end)
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

rf_public int rf_str_cmp(rf_str a, rf_str b)
{
    int result = memcmp(a.data, b.data, rf_min_i(a.size, b.size));
    return result;
}

rf_public rf_bool rf_str_match(rf_str a, rf_str b)
{
    if (a.size != b.size) return 0;
    int cmp = memcmp(a.data, b.data, a.size);
    return cmp == 0;
}

rf_public rf_bool rf_str_match_prefix(rf_str str, rf_str prefix)
{
    if (str.size < prefix.size) return 0;
    int cmp = memcmp(str.data, prefix.data, prefix.size);
    return cmp == 0;
}

rf_public rf_bool rf_str_match_suffix(rf_str str, rf_str suffix)
{
    if (str.size < suffix.size) return 0;
    rf_int offset = str.size - suffix.size;
    int cmp = memcmp(str.data + offset, suffix.data, suffix.size);
    return cmp == 0;
}

rf_public rf_int rf_str_find_first(rf_str haystack, rf_str needle)
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

rf_public rf_int rf_str_find_last(rf_str haystack, rf_str needle)
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

rf_public rf_bool rf_str_contains(rf_str haystack, rf_str needle)
{
    rf_bool result = rf_str_find_first(haystack, needle) != rf_invalid_index;
    return result;
}

rf_public rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by)
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

rf_public rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by)
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

rf_public rf_utf8_char rf_rune_to_utf8_char(rf_rune src)
{
    rf_utf8_char result = 0;
    memcpy(&result, &src, sizeof(rf_rune));
    return result;
}

rf_public rf_rune rf_utf8_char_to_rune(rf_utf8_char src)
{
    rf_rune result = 0;
    int len = strlen((char*)&src);
    rf_decoded_rune r = rf_decode_utf8_char((char*)&src, len);
    result = r.codepoint;
    return result;
}
#pragma endregion