#include "rayfork/foundation/arr.h"
#include "rayfork/foundation/min-max.h"

#include "string.h"
#include "math.h"

rf_extern rf_arr(char) rf_arr_make_impl(rf_int size_of_t, rf_int cap, rf_allocator allocator)
{
    rf_arr(char) result = 0;

    if (size_of_t > 0 && cap > 0)
    {
        rf_arr(char) arr = rf_alloc(allocator, size_of_t * cap + sizeof(rf_arr_header));

        if (arr)
        {
            arr += sizeof(rf_arr_header);
            rf_arr_size(arr)      = 0;
            rf_arr_allocator(arr) = allocator;
            rf_arr_capacity(arr)  = cap;
            result = arr;
        }
    }

    return result;
}

rf_extern void rf_arr_resize_impl(rf_arr(char)* this_arr, rf_int size_of_t, rf_int cap)
{
    if (cap < 0) return;

    rf_int cur_size = rf_arr_size(*this_arr);
    rf_int arr_cur_buf_size = sizeof(rf_arr_header) + (size_of_t * rf_arr_capacity(*this_arr));
    rf_int arr_new_buf_size = sizeof(rf_arr_header) + (size_of_t * cap);

    *this_arr  = rf_realloc_ex(rf_arr_allocator(*this_arr), rf_arr_internals(*this_arr), arr_new_buf_size, arr_cur_buf_size);
    *this_arr += sizeof(rf_arr_header);

    rf_arr_size(*this_arr) = rf_min_i(cur_size, cap);
}

rf_extern void rf_arr_ensure_space_for_impl(rf_arr(char)* this_arr, rf_int size_of_t, rf_int n)
{
    if (rf_arr_space_left(*this_arr) > n)
    {
        rf_int new_cap = rf_max_i(rf_arr_capacity(*this_arr) + n, (rf_int) ceilf(rf_arr_capacity(*this_arr) * 1.5f));
        rf_arr_resize_impl(this_arr, size_of_t, rf_arr_capacity(this_arr) + n);
    }
}

rf_extern void rf_arr_remove_ordered_impl(rf_arr(char)* this_arr, rf_int size_of_t, rf_int i)
{
    if (rf_arr_is_valid_index(*this_arr, i))
    {
        char* dst = *this_arr + (i * size_of_t);
        char* src = dst + 1;
        memmove(dst, src, rf_arr_size(*this_arr) - i);

        rf_arr_size(*this_arr) -= 1;
    }
}