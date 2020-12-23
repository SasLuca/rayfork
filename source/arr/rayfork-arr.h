#ifndef RAYFORK_ARR_H
#define RAYFORK_ARR_H

#include "rayfork-core.h"
#include "string.h"

typedef struct rf_arr_header
{
    rf_int       size;
    rf_int       capacity;
    rf_allocator allocator;
} rf_arr_header;

#define rf_arr(T) T*

// rf_arr_header* rf_arr_internals(rf_arr(T) arr);
#define rf_arr_internals(arr) ((rf_arr_header*)((arr) - sizeof(rf_arr_header)))

// rf_int rf_arr_size(rf_arr(T) arr)
#define rf_arr_size(arr) (rf_arr_internals(arr)->size)

// rf_int rf_arr_capacity(rf_arr(T) arr)
#define rf_arr_capacity(arr) (rf_arr_internals(arr)->capacity)

// rf_allocator rf_arr_allocator(rf_arr(T) arr)
#define rf_arr_allocator(arr) (rf_arr_internals(arr)->allocator)

// T* rf_arr_begin(rf_arr(T) arr)
#define rf_arr_begin(arr) (arr)

// T* rf_arr_end(rf_arr(T) arr)
#define rf_arr_end(arr) ((arr) + rf_arr_internals(arr)->size)

// T& rf_arr_first(rf_arr(T) arr)
#define rf_arr_first(arr) ((arr)[0])

// T& rf_arr_last(rf_arr(T) arr)
#define rf_arr_last(arr) ((arr)[rf_arr_size(arr) - 1])

// bool rf_arr_is_valid_index(rf_arr(T) arr, rf_int i)
#define rf_arr_is_valid_index(arr, i) (((i) >= 0) && ((i) < rf_arr_size(arr)))

// rf_int rf_arr_index_of(rf_arr(T) arr, T* ptr)
#define rf_arr_index_of(arr, ptr) ((rf_int)(ptr - arr))

// rf_int rf_arr_space_left(rf_arr(T) arr)
#define rf_arr_space_left(arr) (rf_arr_capacity(arr) - rf_arr_size(arr))

// bool rf_arr_has_space_for(rf_arr(T), rf_int)
#define rf_arr_has_space_for(arr, n) (rf_arr_size(arr) < (rf_arr_capacity(arr) - n))

// rf_arr(T) rf_arr_make(T, rf_int cap, rf_allocator allocator)
#define rf_arr_make(T, cap, allocator) ((rf_arr(T)) rf_arr_make_impl(sizeof(T), cap, allocator))

// void rf_arr_free(rf_arr(T)*)
#define rf_arr_free(arr) (rf_free(rf_arr_allocator(*arr), rf_internals(*arr)), *arr = 0)

// void rf_arr_add(rf_arr(T)* arr, T item)
#define rf_arr_add(arr, item) ((rf_arr_ensure_space_for((rf_arr(char)*) arr, n), (*arr)[rf_arr_size(*arr)++] = (item)))

// void rf_arr_addn(rf_arr(T)* arr, T item, rf_int n)
#define rf_arr_addn(arr, item, n) ((rf_arr_ensure_space_for((rf_arr(char)*) &arr, n), ))

// void rf_arr_remove_unordered(rf_arr(T)* arr, rf_int index)
#define rf_arr_remove_unordered(arr, i) ((*arr)[i] = (*arr)[rf_arr_size(arr)--])

// void rf_arr_remove_ordered(rf_arr(T)* arr, rf_int index)
#define rf_arr_remove_ordered(arr, i) (rf_arr_remove_ordered_impl(arr, sizeof((*arr)[0]), i))

// void rf_arr_remove_ptr_unordered(rf_arr(T)* arr, T* ptr)
#define rf_arr_remove_ptr_unordered(arr, ptr) (rf_arr_remove_unordered(arr, rf_arr_index_of(*arr, ptr)))

// void rf_arr_remove_ptr_ordered(rf_arr(T)* arr, T* ptr)
#define rf_arr_remove_ptr_ordered(arr, ptr) (rf_arr_remove_ordered(arr, rf_arr_index_of(*arr, ptr)))

// void rf_arr_pop(rf_arr(T)* arr)
#define rf_arr_pop(arr) ((*arr)[rf_arr_size(*arr)--])

// void rf_arr_ensure_space_for(rf_arr(T)* arr, rf_int cap)
#define rf_arr_ensure_space_for(arr, cap) (rf_arr_ensure_space_for_impl(arr, sizeof((*arr)[0]), cap))

// void rf_arr_resize(rf_arr(T)* arr, rf_int cap)
#define rf_arr_resize(arr, cap) (rf_arr_resize_impl((rf_arr(char)*)arr, sizeof((*arr)[0]), cap)

rf_public rf_arr(char) rf_arr_make_impl(rf_int size_of_t, rf_int cap, rf_allocator allocator)
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

rf_public void rf_arr_resize_impl(rf_arr(char)* this_arr, rf_int size_of_t, rf_int cap)
{
    if (cap < 0) return;

    rf_int cur_size = rf_arr_size(*this_arr);
    rf_int arr_cur_buf_size = sizeof(rf_arr_header) + (size_of_t * rf_arr_capacity(*this_arr));
    rf_int arr_new_buf_size = sizeof(rf_arr_header) + (size_of_t * cap);

    *this_arr  = rf_realloc(rf_arr_allocator(*this_arr), rf_arr_internals(*this_arr), arr_new_buf_size, arr_cur_buf_size);
    *this_arr += sizeof(rf_arr_header);

    rf_arr_size(*this_arr) = rf_min_i(cur_size, cap);
}

rf_public void rf_arr_ensure_space_for_impl(rf_arr(char)* this_arr, rf_int size_of_t, rf_int n)
{
    if (rf_arr_space_left(*this_arr) > n)
    {
        rf_int new_cap = rf_max_i(rf_arr_capacity(*this_arr) + n, ceilf(rf_arr_capacity(*this_arr) * 1.5f));
        rf_arr_resize_impl(this_arr, size_of_t, rf_arr_capacity(this_arr) + n);
    }
}

rf_public void rf_arr_remove_ordered_impl(rf_arr(char)* this_arr, rf_int size_of_t, rf_int i)
{
    if (rf_arr_is_valid_index(*this_arr, i))
    {
        char* dst = *this_arr + (i * size_of_t);
        char* src = dst + 1;
        memmove(dst, src, rf_arr_size(*this_arr) - i);

        rf_arr_size(*this_arr) -= 1;
    }
}

#endif // RAYFORK_ARR_H