#include "rayfork/foundation/io.h"
#include "rayfork/foundation/min-max.h"

#include "stdio.h"

rf_extern rf_thread_local rf_io_callbacks rf_global_io_for_thirdparty_libraries;

typedef const char* rf_libc_file_mode;
typedef int         rf_libc_seek_mode;
typedef FILE*       rf_libc_file_handle;

/* @reference(LucaSas): http://www.cplusplus.com/reference/cstdio/fopen/ */
rf_internal rf_libc_file_mode rf_file_mode_to_libc_file_mode(rf_file_mode mode)
{
    switch (mode)
    {
        case rf_file_mode_read:
            return "rb";
        case rf_file_mode_write:
            return "wb+";
        case rf_file_mode_write | rf_file_mode_read:
            return "wb+";
        default:
            return NULL;
    }
}

rf_internal rf_libc_seek_mode rf_seek_mode_to_libc_seek_mode(rf_seek_mode mode)
{
    switch (mode)
    {
        case rf_seek_from_beginning:   return SEEK_SET;
        case rf_seek_from_end:         return SEEK_END;
        case rf_seek_from_current_pos: return SEEK_CUR;
        default:                       return -1;
    }
}

rf_extern rf_file_handle rf_libc_file_open_wrapper(rf_io_callbacks* this_io, const char* filename, rf_file_mode file_mode)
{
    rf_unused(this_io);
    rf_libc_file_mode fm = rf_file_mode_to_libc_file_mode(file_mode);
    rf_file_handle result = fopen(filename, fm);
    return result;
}

rf_extern void rf_libc_file_close_wrapper(rf_io_callbacks* this_io, rf_file_handle file)
{
    rf_unused(this_io);

    fclose((rf_libc_file_handle)file);
}

rf_extern rf_int rf_libc_file_size_wrapper(rf_io_callbacks* this_io, rf_file_handle file)
{
    rf_unused(this_io);

    rf_int result = -1;

    if (file)
    {
        rf_int cur_pos = ftell(file);
        fseek(file, 0L, SEEK_END);
        result = ftell(file);
        fseek(file, cur_pos, SEEK_SET);
    }

    return result;
}

rf_extern rf_int rf_libc_file_tell_wrapper(rf_io_callbacks* this_io, rf_file_handle file)
{
    rf_unused(this_io);

    rf_int result = ftell(file);

    return result;
}

rf_extern rf_success_status rf_libc_file_seek_wrapper(rf_io_callbacks* this_io, rf_file_handle file, rf_int seek_by, rf_seek_mode seek_mode)
{
    rf_unused(this_io);

    rf_success_status result = 0;
    rf_libc_seek_mode sm = rf_seek_mode_to_libc_seek_mode(seek_mode);

    if (file && sm)
    {
        int fseek_result = fseek(file, seek_by, sm);
        result = fseek_result == 0;
    }

    return result;
}

rf_extern rf_int rf_libc_file_read_wrapper(rf_io_callbacks* this_io, rf_file_handle file, rf_int bytes_to_read, void* dst, rf_int dst_size)
{
    rf_unused(this_io);

    rf_int result = 0;

    if (file && dst)
    {
        result = fread(dst, 1, rf_min_i(bytes_to_read, dst_size), file);
    }

    return result;
}

rf_extern rf_int rf_libc_file_write_wrapper(rf_io_callbacks* this_io, rf_file_handle file, const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_unused(this_io);

    rf_int result = 0;

    if (file && dst)
    {
        result = fwrite(src, rf_min_i(src_size, dst_size), 1, file);
    }

    return result;
}

rf_extern void rf_file_contents_free(rf_file_contents it, rf_allocator allocator)
{
    rf_free(allocator, it.data);
}

rf_extern rf_file_contents rf_read_entire_file(const char* filename, rf_allocator allocator, rf_io_callbacks* io)
{
    rf_file_handle file = rf_file_open(io, filename, rf_file_mode_read);

    rf_file_contents result = rf_read_entire_file_from_handle(file, allocator, io);

    rf_file_close(io, file);

    return result;
}

rf_extern rf_file_contents rf_read_entire_file_from_handle(rf_file_handle file, rf_allocator allocator, rf_io_callbacks* io)
{
    rf_int file_size = rf_file_size(io, file);

    uint8_t* dst = rf_alloc(allocator, file_size);
    rf_int dst_size = file_size;

    rf_file_contents result = rf_read_entire_file_to_buffer_from_handle(file, dst, dst_size, io);
    if (!result.valid)
    {
        rf_free(allocator, dst);
    }

    return result;
}

rf_extern rf_file_contents rf_read_entire_file_to_buffer(const char* filename, void* dst, rf_int dst_size, rf_io_callbacks* io)
{
    rf_file_handle file = rf_file_open(io, filename, rf_file_mode_read);
    rf_file_contents result = rf_read_entire_file_to_buffer_from_handle(file, dst, dst_size, io);
    rf_file_close(io, file);

    return result;
}

rf_extern rf_file_contents rf_read_entire_file_to_buffer_from_handle(rf_file_handle file, void* dst, rf_int dst_size, rf_io_callbacks* io)
{
    rf_file_contents result = {0};

    rf_int file_size = rf_file_size(io, file);

    rf_int bytes_affected = rf_file_read(io, file, file_size, dst, dst_size);

    if (bytes_affected == file_size)
    {
        result = (rf_file_contents) {
            .data = dst,
            .size = dst_size,
            .valid = rf_valid
        };
    }

    return result;
}