#ifndef RAYFORK_INTERNAL_UTILS_H
#define RAYFORK_INTERNAL_UTILS_H

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN (1024)
#endif

RF_INTERNAL bool rf_match_str_n(const char* a, int a_len, const char* b, int b_len);

RF_INTERNAL bool rf_match_str_cstr(const char* a, int a_len, const char* b);

RF_INTERNAL void* rf_realloc_wrapper(rf_allocator allocator, void* source, int old_size, int new_size);

RF_INTERNAL bool rf_is_file_extension(const char* filename, const char* ext);

RF_INTERNAL const char* rf_str_find_last(const char* s, const char* charset);

RF_INTERNAL const char* rf_get_directory_path_from_file_path(const char* file_path);

RF_INTERNAL inline int rf_min_i(int a, int b) { return ((a) < (b) ? (a) : (b)); }
RF_INTERNAL inline int rf_max_i(int a, int b) { return ((a) > (b) ? (a) : (b)); }

RF_INTERNAL inline int rf_min_f(float a, float b) { return ((a) < (b) ? (a) : (b)); }
RF_INTERNAL inline int rf_max_f(float a, float b) { return ((a) > (b) ? (a) : (b)); }

#endif // RAYFORK_INTERNAL_UTILS_H