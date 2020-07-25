#ifndef RAYFORK_IO_H
#define RAYFORK_IO_H

#include "rayfork_common.h"

#define RF_NULL_IO                                (RF_LIT(rf_io_callbacks) {0})
#define RF_FILE_SIZE(io, filename)                ((io).file_size_proc((io).user_data, filename))
#define RF_READ_FILE(io, filename, dst, dst_size) ((io).read_file_proc((io).user_data, filename, dst, dst_size))
#define RF_DEFAULT_IO (RF_LIT(rf_io_callbacks) { NULL, rf_libc_get_file_size, rf_libc_load_file_into_buffer })

typedef struct rf_io_callbacks
{
    void* user_data;
    int  (*file_size_proc) (void* user_data, const char* filename);
    bool (*read_file_proc) (void* user_data, const char* filename, void* dst, int dst_size); // Returns true if operation was successful
} rf_io_callbacks;

RF_API int  rf_libc_get_file_size(void* user_data, const char* filename);
RF_API bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, int dst_size);

#endif // RAYFORK_IO_H