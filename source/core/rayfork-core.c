#include "rayfork-core.h"

#include "string.h"
#include "malloc.h"
#include "stdio.h"

#pragma region error

rf_thread_local rf_recorded_error rf__last_error;

rf_public rf_recorded_error rf_get_last_recorded_error()
{
    return rf__last_error;
}

#pragma endregion

#pragma region allocator

rf_thread_local rf_allocator rf__global_allocator_for_dependencies;

rf_public void* rf_calloc_wrapper(rf_allocator allocator, rf_int amount, rf_int size)
{
    void* ptr = rf_alloc(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

rf_public void* rf_default_realloc(rf_allocator allocator, void* source, int old_size, int new_size)
{
    void* new_alloc = rf_alloc(allocator, new_size);
    if (new_alloc && source && old_size) { memcpy(new_alloc, source, old_size); }
    if (source) { rf_free(allocator, source); }
    return new_alloc;
}

rf_public void* rf_libc_allocator_wrapper(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args)
{
    rf_assert(this_allocator);
    (void)this_allocator;
    (void)source_location;

    void* result = 0;

    switch (mode)
    {
        case rf_allocator_mode_alloc:
            result = malloc(args.size_to_allocate_or_reallocate);
            break;

        case rf_allocator_mode_free:
            free(args.pointer_to_free_or_realloc);
            break;

        case rf_allocator_mode_realloc:
            result = realloc(args.pointer_to_free_or_realloc, args.size_to_allocate_or_reallocate);
            break;

        default: break;
    }

    return result;
}

#pragma endregion

#pragma region io

rf_public rf_int rf_libc_get_file_size(void* user_data, const char* filename)
{
    ((void)user_data);

    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

rf_public rf_bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, rf_int dst_size)
{
    ((void)user_data);
    rf_bool result = 0;

    FILE* file = fopen(filename, "rb");
    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        int file_size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        if (dst_size >= file_size)
        {
            int read_size = fread(dst, 1, file_size, file);
            int no_error = ferror(file) == 0;
            if (no_error && read_size == file_size)
            {
                result = 1;
            }
        }
        // else log_error buffer is not big enough
    }
    // else log error could not open file

    fclose(file);

    return result;
}

#pragma endregion

#pragma region logger

rf_internal rf_log_type rf__log_filter;
rf_internal rf_logger   rf__logger;

rf_public void rf_set_logger(rf_logger logger) { rf__logger = logger; }
rf_public void rf_set_logger_filter(rf_log_type filter) { rf__log_filter = filter; }

rf_public rf_logger rf_get_logger() { return rf__logger; }
rf_public rf_log_type rf_get_log_filter() { return rf__log_filter; }

rf_public void rf__internal_log(rf_source_location source_location, rf_log_type log_type, const char* msg, ...)
{
    if (!(log_type & rf__log_filter)) return;

    va_list args;

    va_start(args, msg);

    rf_error_type error_type = rf_no_error;

    // If the log type is an error then the error type must be the first arg
    if (log_type == rf_log_type_error)
    {
        error_type = va_arg(args, rf_error_type);
    }

    if (rf__logger.log_proc)
    {
        rf__logger.log_proc(&rf__logger, source_location, log_type, msg, error_type, args);
    }

    va_end(args);
}

rf_public const char* rf_log_type_string(rf_log_type log_type)
{
    switch (log_type)
    {
        case rf_log_type_none:    return "NONE";
        case rf_log_type_debug:   return "DEBUG";
        case rf_log_type_info:    return "INFO";
        case rf_log_type_warning: return "WARNING";
        case rf_log_type_error:   return "ERROR";
        default:                  return "RAYFORK_LOG_TYPE_UNKNOWN";
    }
}

rf_public void rf_libc_printf_logger(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args)
{
    ((void)logger); // unused
    printf("[RAYFORK %s]: ", rf_log_type_string(log_type));
    vprintf(msg, args);
    printf("\n");
}

#pragma endregion