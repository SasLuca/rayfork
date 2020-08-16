#pragma region globals

// Useful internal macros
#define rf_ctx   (*rf__ctx)
#define rf_gfx   (rf_ctx.gfx_ctx)
#define rf_gl    (rf_gfx.gl)
#define rf_batch (*(rf_ctx.current_batch))

// Global pointer to context struct
RF_INTERNAL rf_context* rf__ctx;
RF_INTERNAL RF_THREAD_LOCAL rf_recorded_error rf__last_error;

#pragma endregion

#pragma region assert

#if !defined(RF_ASSERT) && defined(RAYFORK_ENABLE_ASSERTIONS)
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#else
    #define RF_ASSERT(condition)
#endif

#pragma endregion

#pragma region allocator

RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf__global_allocator_for_dependencies;
#define RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(allocator) rf__global_allocator_for_dependencies = (allocator)

RF_API void* rf_calloc_wrapper(rf_allocator allocator, int amount, int size)
{
    void* ptr = RF_ALLOC(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

RF_API void* rf_libc_allocator_wrapper(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args)
{
    RF_ASSERT(this_allocator);

    void* result = 0;

    switch (mode)
    {
        case RF_AM_ALLOC:
            result = malloc(args.size_to_allocate_or_reallocate);
            break;

        case RF_AM_FREE:
            free(args.pointer_to_free_or_realloc);
            break;

        case RF_AM_REALLOC:
            result = realloc(args.pointer_to_free_or_realloc, args.size_to_allocate_or_reallocate);
            break;

        default: break;
    }

    return result;
}

#pragma endregion

#pragma region io

RF_API int rf_libc_get_file_size(void* user_data, const char* filename)
{
    ((void)user_data);

    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

RF_API bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, int dst_size)
{
    ((void)user_data);
    bool result = false;

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
                result = true;
            }
        }
        // else log_error buffer is not big enough
    }
    // else log error could not open file

    fclose(file);

    return result;
}

#pragma endregion

#pragma region internal utils

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN (1024)
#endif

RF_INTERNAL bool rf_match_str_n(const char* a, int a_len, const char* b, int b_len)
{
    return a_len == b_len && strncmp(a, b, a_len) == 0;
}

RF_INTERNAL bool rf_match_str_cstr(const char* a, int a_len, const char* b)
{
    return rf_match_str_n(a, a_len, b, strlen(b));
}

RF_INTERNAL void* rf_realloc_wrapper(rf_allocator allocator, void* source, int old_size, int new_size)
{
    void* new_alloc = RF_ALLOC(allocator, new_size);
    if (new_alloc && source && old_size) { memcpy(new_alloc, source, old_size); }
    if (source) { RF_FREE(allocator, source); }
    return new_alloc;
}

RF_INTERNAL bool rf_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return false;
    }

    return rf_match_str_n(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

RF_INTERNAL const char* rf_str_find_last(const char* s, const char* charset)
{
    const char* latest_match = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latest_match = s++) { }
    return latest_match;
}

RF_INTERNAL const char* rf_get_directory_path_from_file_path(const char* file_path)
{
    static RF_THREAD_LOCAL char rf_global_dir_path[RF_MAX_FILEPATH_LEN];

    const char* last_slash = NULL;
    memset(rf_global_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = rf_str_find_last(file_path, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(rf_global_dir_path, file_path, strlen(file_path) - (strlen(last_slash) - 1));
    rf_global_dir_path[strlen(file_path) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return rf_global_dir_path;
}

RF_INTERNAL inline int rf_min_i(int a, int b) { return ((a) < (b) ? (a) : (b)); }
RF_INTERNAL inline int rf_max_i(int a, int b) { return ((a) > (b) ? (a) : (b)); }

RF_INTERNAL inline int rf_min_f(float a, float b) { return ((a) < (b) ? (a) : (b)); }
RF_INTERNAL inline int rf_max_f(float a, float b) { return ((a) > (b) ? (a) : (b)); }

#pragma endregion

#pragma region logger

#define RF_RECORDED_ERROR(error_type) (RF_LIT(rf_recorded_error) { RF_SOURCE_LOCATION, error_type })

/*
 Note(LucaSas): MSVC, clang and gcc all deal with __VA_ARGS__ differently.
 Normally you would expect that __VA_ARGS__ consume a trailing comma but it doesn't, this is why we must ##__VA_ARGS__.
 ##__VA_ARGS__ is a preprocessor black magic which achieves this goal, it's not standard but every compiler supports it, if
 this causes issues on some compiler just disable logs with RF_DISABLE_LOGGER.
 Also bear in mind that ##__VA_ARGS__ still works differently between compilers but this code seems to work on all major compilers.
*/
#define RF_LOG(log_type, msg, ...)         rf_log_impl(RF_SOURCE_LOCATION, (log_type), (msg), ##__VA_ARGS__)
#define RF_LOG_ERROR(error_type, msg, ...) (RF_LOG(RF_LOG_TYPE_ERROR, (msg), (error_type), ##__VA_ARGS__), rf__last_error = RF_RECORDED_ERROR(error_type))

RF_INTERNAL void rf_log_impl(rf_source_location source_location, rf_log_type log_type, const char* msg, ...)
{
    if (!(log_type & rf_ctx.logger_filter)) return;

    va_list args;

    va_start(args, msg);

    rf_error_type error_type = RF_NO_ERROR;

    // If the log type is an error then the error type must be the first arg
    if (log_type == RF_LOG_TYPE_ERROR)
    {
        error_type = va_arg(args, rf_error_type);
    }

    if (rf_ctx.logger.log_proc)
    {
        rf_ctx.logger.log_proc(&rf_ctx.logger, source_location, log_type, msg, error_type, args);
    }

    va_end(args);
}

RF_API const char* rf_log_type_string(rf_log_type log_type)
{
    switch (log_type)
    {
        case RF_LOG_TYPE_NONE:    return "NONE";
        case RF_LOG_TYPE_DEBUG:   return "DEBUG";
        case RF_LOG_TYPE_INFO:    return "INFO";
        case RF_LOG_TYPE_WARNING: return "WARNING";
        case RF_LOG_TYPE_ERROR:   return "ERROR";
        default:                  return "RAYFORK_LOG_TYPE_UNKNOWN";
    }
}

RF_API void rf_libc_printf_logger(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args)
{
    ((void)logger); // unused
    printf("[RAYFORK %s]: ", rf_log_type_string(log_type));
    vprintf(msg, args);
    printf("\n");
}

#pragma endregion