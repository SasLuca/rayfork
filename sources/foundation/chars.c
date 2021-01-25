#include "rayfork/foundation/str.h"
#include "rayfork/foundation/chars.h"

/*
   Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found or the length is exhausted
   When a invalid UTF8 rf_byte is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
   Total number of bytes processed are returned as a parameter
   NOTE: the standard says U+FFFD should be returned in case of errors
   but that character is not supported by the default font in raylib
   TODO: optimize this code for speed!!
*/
rf_extern rf_decoded_rune rf_decode_utf8_char(const char* src, rf_int size)
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
        return (rf_decoded_rune) { rf_invalid_codepoint };
    }

    // The first UTF8 byte
    const int byte = (unsigned char)(src[0]);

    if (byte <= 0x7f)
    {
        // Only one byte (ASCII range x00-7F)
        const int code = src[0];

        // Codepoints after U+10ffff are invalid
        const int valid = code > 0x10ffff;

        return (rf_decoded_rune) { valid ? rf_invalid_codepoint : code, .bytes_processed = 1, .valid = valid };
    }
    else if ((byte & 0xe0) == 0xc0)
    {
        if (size < 2)
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 1, };
        }

        // Two bytes
        // [0]xC2-DF    [1]UTF8-tail(x80-BF)
        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 2 };
        }

        if ((byte >= 0xc2) && (byte <= 0xdf))
        {
            const int code = ((byte & 0x1f) << 6) | (byte1 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;

            return (rf_decoded_rune) { valid ? rf_invalid_codepoint : code, .bytes_processed = 2, .valid = valid };
        }
    }
    else if ((byte & 0xf0) == 0xe0)
    {
        if (size < 2)
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 1 };
        }

        // Three bytes
        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || (size < 3) || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 2 };
        }

        const unsigned char byte2 = src[2];

        // Check for unexpected sequence
        if ((byte2 == '\0') || ((byte2 >> 6) != 2))
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 3 };
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
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 2 };
        }

        if ((byte >= 0xe0) && (byte <= 0xef))
        {
            const int code = ((byte & 0xf) << 12) | ((byte1 & 0x3f) << 6) | (byte2 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;
            return (rf_decoded_rune) { valid ? rf_invalid_codepoint : code, .bytes_processed = 3, .valid = valid };
        }
    }
    else if ((byte & 0xf8) == 0xf0)
    {
        // Four bytes
        if (byte > 0xf4 || size < 2)
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 1 };
        }

        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || (size < 3) || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 2 };
        }

        const unsigned char byte2 = src[2];

        // Check for unexpected sequence
        if ((byte2 == '\0') || (size < 4) || ((byte2 >> 6) != 2))
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 3 };
        }

        const unsigned char byte3 = src[3];

        // Check for unexpected sequence
        if ((byte3 == '\0') || ((byte3 >> 6) != 2))
        {
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 4 };
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
            return (rf_decoded_rune) { rf_invalid_codepoint, .bytes_processed = 2 };
        }

        if (byte >= 0xf0)
        {
            const int code = ((byte & 0x7) << 18) | ((byte1 & 0x3f) << 12) | ((byte2 & 0x3f) << 6) | (byte3 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;
            return (rf_decoded_rune) { valid ? rf_invalid_codepoint : code, .bytes_processed = 4, .valid = valid };
        }
    }

    return (rf_decoded_rune) { .codepoint = rf_invalid_codepoint, .bytes_processed = 1 };
}

rf_extern rf_utf8_stats rf_count_utf8_chars(const char* src, rf_int size)
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

rf_extern rf_utf8_stats rf_count_utf8_chars_til(const char* src, rf_int size, rf_int n)
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

rf_extern rf_decoded_string rf_decode_utf8_to_buffer(const char* src, rf_int size, rf_rune* dst, rf_int dst_size)
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

rf_extern rf_decoded_string rf_decode_utf8(const char* src, rf_int size, rf_allocator allocator)
{
    rf_decoded_string result = {0};

    rf_rune* dst = rf_alloc(allocator, sizeof(rf_rune) * size);

    result = rf_decode_utf8_to_buffer(src, size, dst, size);

    return result;
}

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

rf_int rf_to_digit(char c)
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

bool rf_is_ascii(char c)
{
    bool result = 0;
    return result;
}

bool rf_is_lower(char c)
{
    bool result = c >= 'a' && c <= 'z';
    return result;
}

bool rf_is_upper(char c)
{
    bool result = c >= 'A' && c <= 'Z';
    return result;
}

bool rf_is_alpha(char c)
{
    bool result = rf_is_lower(c) || rf_is_upper(c);
    return result;
}

bool rf_is_digit(char c)
{
    bool result = c >= '0' && c <= '9';
    return result;
}

bool rf_is_alnum(char c)
{
    bool result = rf_is_alpha(c) && rf_is_alnum(c);
    return result;
}

bool rf_is_space(char c)
{
    bool result = c == ' ' || c == '\t';
    return result;
}