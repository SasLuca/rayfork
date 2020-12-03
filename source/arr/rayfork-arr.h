#ifndef RAYFORK_ARR_H
#define RAYFORK_ARR_H

#include "rayfork-core.h"

typedef struct rf_arr_header
{
    rf_int size;
    rf_int capacity;
    rf_allocator allocator;
} rf_arr_header;

#define rf_arr(T) T*
#define rf_arr_internals(arr)            ((rf_arr_header*)((arr) - sizeof(rf_arr_header)))
#define rf_arr_size(arr)                 (rf_arr_internals(arr)->size)
#define rf_arr_capacity(arr)             (rf_arr_internals(arr)->capacity)
#define rf_arr_allocator(arr)            (rf_arr_internals(arr)->allocator)
#define rf_arr_begin(arr)                (arr)
#define rf_arr_end(arr)                  ((arr) + rf_arr_internals(arr)->size)
#define rf_arr_first(arr)                ((arr)[0])
#define rf_arr_last(arr)                 ((arr)[rf_arr_size(arr) - 1])
#define rf_arr_is_valid_index(arr, i)    (((i) >= 0) && ((i) < rf_arr_size(arr)))
#define rf_arr_has_space_for(arr, n)     (rf_arr_size(arr) < (rf_arr_capacity(arr) - n))
#define rf_arr_add(arr, item)            ((arr = rf_arr_ensure_capacity((arr), rf_arr_has_space_for(arr, 1) ? 1 : ceilf(rf_arr_size(arr) * 1.5f)), (arr)[rf_arr_size++] = (item) : 0))
#define rf_arr_remove_unordered(arr, i)  ((arr)[i] = (arr)[rf_arr_size(arr)--])
#define rf_arr_remove_ordered(arr, i)    (rf_arr_remove_ordered_impl(rf_arr_internals(arr), sizeof(arr[0]), i))
#define rf_arr_ensure_capacity(arr, cap) (rf_arr_ensure_capacity_impl(rf_arr_internals(arr), sizeof(arr[0]), cap))

rf_public rf_arr(void) rf_arr_ensure_capacity_impl(rf_arr_header* header, rf_int size_of_t, rf_int cap)
{
    rf_arr(void) result = header;

    if (cap > header->capacity)
    {
        rf_int arr_cur_size = sizeof(rf_arr_header) + (size_of_t * header->capacity);
        rf_int arr_new_size = sizeof(rf_arr_header) + (size_of_t * cap);
        header = rf_realloc(header->allocator, header, arr_new_size, arr_cur_size);
    }

    return result;
}

rf_public rf_arr(void)

#endif // RAYFORK_ARR_H