#ifndef RAYFORK_FOUNDATION_IO_H
#define RAYFORK_FOUNDATION_IO_H

#include "basicdef.h"
#include "analysis-annotations.h"
#include "allocator.h"

#define rf_default_io (rf_lit(rf_io_callbacks) { 0, rf_libc_file_open_wrapper, rf_libc_file_close_wrapper, rf_libc_file_size_wrapper, rf_libc_file_tell_wrapper, rf_libc_file_seek_wrapper, rf_libc_file_read_wrapper, rf_libc_file_write_wrapper })

typedef enum rf_seek_mode
{
    rf_seek_mode_invalid,
    rf_seek_from_beginning,
    rf_seek_from_end,
    rf_seek_from_current_pos,
} rf_seek_mode;

typedef enum rf_file_mode
{
    rf_file_mode_invalid = 0x0,
    rf_file_mode_read    = 0x1,
    rf_file_mode_write   = 0x2,
} rf_file_mode;

struct rf_io_callbacks;
typedef void* rf_file_handle;

typedef rf_file_handle    (rf_file_open_proc)  (struct rf_io_callbacks* this_io, const char* filename, rf_file_mode file_mode);
typedef void              (rf_file_close_proc) (struct rf_io_callbacks* this_io, rf_file_handle file);
typedef rf_int            (rf_file_size_proc)  (struct rf_io_callbacks* this_io, rf_file_handle file);
typedef rf_int            (rf_file_tell_proc)  (struct rf_io_callbacks* this_io, rf_file_handle file);
typedef rf_success_status (rf_file_seek_proc)  (struct rf_io_callbacks* this_io, rf_file_handle file, rf_int seek_by, rf_seek_mode seek_mode);
typedef rf_int            (rf_file_read_proc)  (struct rf_io_callbacks* this_io, rf_file_handle file, rf_int bytes_to_read, void* dst, rf_int dst_size);
typedef rf_int            (rf_file_write_proc) (struct rf_io_callbacks* this_io, rf_file_handle file, const void* src, rf_int src_size, void* dst, rf_int dst_size);

typedef struct rf_io_callbacks
{
    void*               user_data;
    rf_file_open_proc*  open;
    rf_file_close_proc* close;
    rf_file_size_proc*  size;
    rf_file_tell_proc*  tell;
    rf_file_seek_proc*  seek;
    rf_file_read_proc*  read;
    rf_file_write_proc* write;
} rf_io_callbacks;

rf_extern rf_thread_local rf_io_callbacks rf_global_io_for_thirdparty_libraries;

rf_extern rf_file_handle    rf_libc_file_open_wrapper(rf_io_callbacks* this_io, const char* filename, rf_file_mode file_mode);
rf_extern void              rf_libc_file_close_wrapper(rf_io_callbacks* this_io, rf_file_handle file);
rf_extern rf_int            rf_libc_file_size_wrapper(rf_io_callbacks* this_io, rf_file_handle file);
rf_extern rf_int            rf_libc_file_tell_wrapper(rf_io_callbacks* this_io, rf_file_handle file);
rf_extern rf_success_status rf_libc_file_seek_wrapper(rf_io_callbacks* this_io, rf_file_handle file, rf_int seek_by, rf_seek_mode seek_mode);
rf_extern rf_int            rf_libc_file_read_wrapper(rf_io_callbacks* this_io, rf_file_handle file, rf_int bytes_to_read, void* dst, rf_int dst_size);
rf_extern rf_int            rf_libc_file_write_wrapper(rf_io_callbacks* this_io, rf_file_handle file, const void* src, rf_int src_size, void* dst, rf_int dst_size);

rf_inline rf_file_handle    rf_file_open(rf_io_callbacks* this_io, const char* filename, rf_file_mode file_mode) { return this_io->open(this_io, filename, file_mode); }
rf_inline void              rf_file_close(rf_io_callbacks* this_io, rf_file_handle file) { this_io->close(this_io, file); }
rf_inline rf_int            rf_file_size(rf_io_callbacks* this_io, rf_file_handle file) { return this_io->size(this_io, file); }
rf_inline rf_int            rf_file_tell(rf_io_callbacks* this_io, rf_file_handle file) { return this_io->tell(this_io, file); }
rf_inline rf_success_status rf_file_seek(rf_io_callbacks* this_io, rf_file_handle file, rf_int seek_by, rf_seek_mode seek_mode) { return this_io->seek(this_io, file, seek_by, seek_mode); }
rf_inline rf_int            rf_file_read(rf_io_callbacks* this_io, rf_file_handle file, rf_int bytes_to_read, void* dst, rf_int dst_size) { return this_io->read(this_io, file, bytes_to_read, dst, dst_size); }
rf_inline rf_int            rf_file_write(rf_io_callbacks* this_io, rf_file_handle file, const void* src, rf_int src_size, void* dst, rf_int dst_size) { return this_io->write(this_io, file, src, src_size, dst, dst_size); }

typedef struct rf_file_contents
{
    rf_field_range(0, rf_int_max) rf_int   size;
    rf_buffer_size(size)          uint8_t* data;
    rf_valid_t valid;
} rf_file_contents;

rf_extern void rf_file_contents_free(rf_file_contents it, rf_allocator allocator);
rf_extern rf_file_contents rf_read_entire_file(const char* filename, rf_allocator allocator, rf_io_callbacks* io);
rf_extern rf_file_contents rf_read_entire_file_from_handle(rf_file_handle file, rf_allocator allocator, rf_io_callbacks* io);
rf_extern rf_file_contents rf_read_entire_file_to_buffer(const char* filename, void* dst, rf_int dst_size, rf_io_callbacks* io);
rf_extern rf_file_contents rf_read_entire_file_to_buffer_from_handle(rf_file_handle file, void* dst, rf_int dst_size, rf_io_callbacks* io);

#endif // RAYFORK_FOUNDATION_IO_H