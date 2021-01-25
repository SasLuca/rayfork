#ifndef RAYFORK_FOUNDATION_STRBUF_H
#define RAYFORK_FOUNDATION_STRBUF_H

#include "basicdef.h"
#include "allocator.h"
#include "str.h"

typedef struct rf_strbuf
{
    char*        data;
    rf_int       size;
    rf_int       capacity;
    rf_allocator allocator;
    rf_valid_t   valid;
} rf_strbuf;

rf_extern rf_strbuf rf_strbuf_make(rf_int initial_amount, rf_allocator allocator);

rf_extern rf_strbuf rf_strbuf_from_str(rf_str str, rf_allocator allocator);

rf_extern rf_strbuf rf_strbuf_clone(rf_strbuf buf, rf_allocator allocator);

rf_extern rf_str rf_strbuf_to_str(rf_strbuf src);

rf_extern rf_int rf_strbuf_remaining_capacity(const rf_strbuf* this_buf);

rf_extern void rf_strbuf_reserve(rf_strbuf* this_buf, rf_int size);

rf_extern void rf_strbuf_ensure_capacity_for(rf_strbuf* this_buf, rf_int size);

rf_extern void rf_strbuf_append(rf_strbuf* this_buf, rf_str it);

rf_extern void rf_strbuf_prepend(rf_strbuf* this_buf, rf_str it);

rf_extern void rf_strbuf_insert_utf8(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at);

rf_extern void rf_strbuf_insert_b(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at);

rf_extern void rf_strbuf_remove_range_utf8(rf_strbuf* this_buf, rf_int begin, rf_int end);

rf_extern void rf_strbuf_remove_range_b(rf_strbuf* this_buf, rf_int begin, rf_int end);

rf_extern void rf_strbuf_free(rf_strbuf* this_buf);

#endif // RAYFORK_FOUNDATION_STRBUF_H