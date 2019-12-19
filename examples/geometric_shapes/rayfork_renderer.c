//This is the translation unit for the rayfork implementation
//Here we also implement the platform specific functions

#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "rayfork_renderer.h"
#include <stdio.h>
#include <stdbool.h>

// We undef these since they are defined in glad.h and windows.h redefines them and we get warnings
#ifdef _WIN32
#undef APIENTRY
#undef ARRAYSIZE

#include <windows.h>

static LARGE_INTEGER global_performance_counter_frequency;
static bool global_performance_counter_frequency_initialised;

// Returns elapsed time in seconds since InitWindow()
double rf_get_time(void)
{
    if (!global_performance_counter_frequency_initialised)
    {
        assert(QueryPerformanceFrequency(&global_performance_counter_frequency) != FALSE);
        global_performance_counter_frequency_initialised = true;
    }

    LARGE_INTEGER qpc_result = {0};
    assert(QueryPerformanceCounter(&qpc_result) != FALSE);
    return (double)qpc_result.QuadPart / global_performance_counter_frequency.QuadPart;
}
#elif
double rf_get_time(void)
{
    //To implement on linux
    return 0;
}
#endif

void rf_wait(float ms)
{
    //Can be left unimplemented
}

void rf_swap_buffers(void)
{
    //Can be left unimplemented
}

// Files management functions
int rf_get_file_size(const char* filename)
{
    FILE* file = fopen(filename, "r");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);
    return size;
}

void rf_load_file_into_buffer(const char* filename, uint8_t* buffer, int bufferSize)
{
    FILE* file = fopen(filename, "r");
    assert(file != NULL);

    size_t newLen = fread(buffer, sizeof(char), bufferSize, file);

    assert(ferror(file) == 0);

    fclose(file);
}

bool rf_file_exists(const char* filename)
{
    FILE* file = fopen(filename, "r");
    bool result = file != NULL;
    fclose(file);
    return result;
}

bool rf_write_to_file(const char* filename, uint8_t* buffer, int buffer_size)
{
    FILE* file = fopen(filename, "r");
    bool result = fwrite(buffer, 1, buffer_size, file);
    fclose(file);
    return result;
}

// Show trace log messages (rf_log_info, rf_log_warning, LOG_ERROR, LOG_DEBUG)
void rf_trace_log(const rf_context* rf_ctx, int logType, const char* text, ...)
{
    //Can be left unimplemented
}