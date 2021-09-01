#if 0
#include "rayfork/foundation/str.h"
#include "rayfork/foundation/min-max.h"
#include "string.h"

rf_extern rf_strbuf rf_strbuf_make_ex(rf_int initial_amount, rf_allocator allocator)
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

rf_extern rf_strbuf rf_strbuf_clone_ex(rf_strbuf this_buf, rf_allocator allocator)
{
    rf_strbuf result = {0};

    if (this_buf.valid)
    {
        result = rf_strbuf_make_ex(this_buf.capacity, allocator);
        rf_strbuf_append(&result, rf_strbuf_to_str(this_buf));
    }

    return result;
}

rf_extern rf_str rf_strbuf_to_str(rf_strbuf src)
{
    rf_str result = {0};

    if (src.valid)
    {
        result.data = src.data;
        result.size = src.size;
    }

    return result;
}

rf_extern rf_int rf_strbuf_remaining_capacity(const rf_strbuf* this_buf)
{
    rf_int result = this_buf->capacity - this_buf->size;
    return result;
}

rf_extern void rf_strbuf_reserve(rf_strbuf* this_buf, rf_int new_capacity)
{
    if (new_capacity > this_buf->capacity)
    {
        char* new_buf = rf_realloc_ex(this_buf->allocator, this_buf->data, new_capacity, this_buf->capacity);
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

rf_extern void rf_strbuf_ensure_capacity_for(rf_strbuf* this_buf, rf_int size)
{
    rf_int remaining_capacity = rf_strbuf_remaining_capacity(this_buf);
    if (remaining_capacity < size)
    {
        // We either increase the buffer to capacity * 2 or to the necessary size to fit the size plus one for the null terminator
        rf_int amount_to_reserve = rf_max_i(this_buf->capacity * 2, this_buf->capacity + (size - remaining_capacity) + 1);
        rf_strbuf_reserve(this_buf, amount_to_reserve);
    }
}

rf_extern void rf_strbuf_append(rf_strbuf* this_buf, rf_str it)
{
    rf_strbuf_ensure_capacity_for(this_buf, it.size);

    memcpy(this_buf->data + this_buf->size, it.data, it.size);
    this_buf->size += it.size;
    this_buf->data[this_buf->size] = 0;
}

rf_extern void rf_strbuf_prepend(rf_strbuf* this_buf, rf_str it)
{
    rf_strbuf_ensure_capacity_for(this_buf, it.size);

    memmove(this_buf->data + it.size, this_buf->data, this_buf->size);
    memcpy(this_buf->data, it.data, it.size);
    this_buf->size += it.size;
    this_buf->data[this_buf->size] = 0;
}

rf_extern void rf_strbuf_insert_utf8(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at)
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

rf_extern void rf_strbuf_insert_b(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at)
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

rf_extern void rf_strbuf_remove_range(rf_strbuf*, rf_int begin, rf_int end);

rf_extern void rf_strbuf_remove_range_b(rf_strbuf*, rf_int begin, rf_int end);

rf_extern void rf_strbuf_free(rf_strbuf* this_buf)
{
    rf_free(this_buf->allocator, this_buf->data);

    this_buf->size = 0;
    this_buf->capacity = 0;
}
#endif