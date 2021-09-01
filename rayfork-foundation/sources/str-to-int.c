#if 0
#include "rayfork/foundation/str.h"
#include "rayfork/foundation/numeric-limits.h"

static inline unsigned char rf_digit_from_char(unsigned char c)
{
    // convert ['0', '9'] ['A', 'Z'] ['a', 'z'] to [0, 35], everything else to 255
    static unsigned char digit_from_byte[] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255,
        255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 255, 255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255
    };

    return digit_from_byte[(unsigned char)c];
}

// Taken from msvc (https://github.com/microsoft/STL/blob/721e3ad8952fce3447916ecdbff3863043686fa1/stl/inc/charconv#L224
rf_extern rf_str_to_int_result rf_str_to_int(rf_str str, int base)
{
     bool minus_sign = false;

     const char* first = str.data;
     const char* next  = first;
     const char* last  = str.data + str.size;

     if (next != last && *next == '-')
     {
         minus_sign = true;
        ++next;
    }

    size_t risky_val; // @intentionally-uninit
    size_t max_digit; // @intentionally-uninit

    if (minus_sign)
    {
        risky_val = (size_t)(rf_int_abs_min / base);
        max_digit = (size_t)(rf_int_abs_min % base);
    }
    else
    {
        risky_val = (size_t)(rf_int_max / base);
        max_digit = (size_t)(rf_int_max % base);
    }

    size_t value = 0;

    bool overflowed = false;

    for (; next != last; ++next)
    {
        const unsigned char digit = rf_digit_from_char(*next);

        if (digit >= base)
        {
            break;
        }

        bool never_overflows = value < risky_val;
        bool overflows_for_certain_digits = value == risky_val && digit <= max_digit;
        if (never_overflows || overflows_for_certain_digits)
        {
            value = (size_t)(value * base + digit);
        }
        else // value > risky_val always overflows
        {
            overflowed  = true; // keep going, next still needs to be updated, value is now irrelevant
        }
    }

    if (next - first == (rf_int)(minus_sign))
    {
        // invalid
        return (rf_str_to_int_result) { value, rf_invalid };
    }

    if (overflowed)
    {
        // out of range
        return (rf_str_to_int_result) { value, rf_invalid };
    }

    if (minus_sign)
    {
        value = (size_t)(0 - value);
    }

    return (rf_str_to_int_result) { value, rf_valid };
}

rf_extern float rf_str_to_float(rf_str src)
{
}
#endif