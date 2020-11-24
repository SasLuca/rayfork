#ifndef RAYFORK_GFX_INTERNAL_STRING_UTILS_H
#define RAYFORK_GFX_INTERNAL_STRING_UTILS_H

#include "rayfork-core.h"
#include "string.h"

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN (1024)
#endif

RF_INTERNAL inline rf_bool rf_match_str_n(const char* a, int a_len, const char* b, int b_len)
{
    return a_len == b_len && strncmp(a, b, a_len) == 0;
}

RF_INTERNAL inline rf_bool rf_match_str_cstr(const char* a, int a_len, const char* b)
{
    return rf_match_str_n(a, a_len, b, strlen(b));
}

RF_INTERNAL inline rf_bool rf_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return 0;
    }

    return rf_match_str_n(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

RF_INTERNAL inline const char* rf__str_find_last(const char* s, const char* charset)
{
    const char* latest_match = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latest_match = s++) { }
    return latest_match;
}

RF_INTERNAL inline const char* rf_get_directory_path_from_file_path(const char* file_path)
{
    static RF_THREAD_LOCAL char rf_global_dir_path[RF_MAX_FILEPATH_LEN];

    const char* last_slash = NULL;
    memset(rf_global_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = rf__str_find_last(file_path, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(rf_global_dir_path, file_path, strlen(file_path) - (strlen(last_slash) - 1));
    rf_global_dir_path[strlen(file_path) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return rf_global_dir_path;
}

#endif // RAYFORK_GFX_INTERNAL_STRING_UTILS_H