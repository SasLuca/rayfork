#include "rayfork.h"
#include "rayfork_common_internal.h"
#include "rayfork_gfx.h"
#include "math.h"

//Global pointer to context struct
rf_context* _rf_ctx;

//region time functions
#if defined(RF_CUSTOM_TIME)
    //Used to set custom functions
    RF_API void rf_set_time_functions(void (*wait_func)(float), double (*get_time_func)(void))
    {
        _rf_ctx.wait_func = wait_func;
        _rf_ctx.get_time_func = get_time_func;
    }

    // Wait for some milliseconds (pauses program execution)
    RF_API void rf_wait(float it)
    {
        if (_rf_ctx.wait_func) _rf_ctx.wait_func(it);
    }

    // Returns elapsed time in seconds since rf_context_init
    RF_API double rf_get_time(void)
    {
        if (_rf_ctx.get_time_func) return _rf_ctx.get_time_func(it);
        return 0;
    }
#else //if !defined(RF_CUSTOM_TIME)
    //Windows only
    #ifdef _WIN32
        static long long int _rf_global_performance_counter_frequency;
        static bool _rf_global_performance_counter_frequency_initialised;

        //If windows.h is not included
        #if !defined(_WINDOWS_)
            //Definition so that we don't have to include windows.h
            #ifdef __cplusplus
                extern "C" int __stdcall QueryPerformanceCounter(long long int* lpPerformanceCount);
                extern "C" int __stdcall QueryPerformanceFrequency(long long int* lpFrequency);
                extern "C" void __stdcall Sleep(int dwMilliseconds);
            #else
                extern int __stdcall QueryPerformanceCounter(long long int* lpPerformanceCount);
                extern int __stdcall QueryPerformanceFrequency(long long int* lpFrequency);
                extern void __stdcall Sleep(int dwMilliseconds);
            #endif
        #endif

        // Returns elapsed time in seconds since program started
        RF_API double rf_get_time(void)
        {
            if (!_rf_global_performance_counter_frequency_initialised)
            {
                #ifdef _WINDOWS_
                    RF_ASSERT(QueryPerformanceFrequency((LARGE_INTEGER*)&_rf_global_performance_counter_frequency) != false);
                #else
                    RF_ASSERT(QueryPerformanceFrequency(&_rf_global_performance_counter_frequency) != false);
                #endif
                _rf_global_performance_counter_frequency_initialised = true;
            }

            long long int qpc_result = {0};

            #ifdef _WINDOWS_
                RF_ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&qpc_result) != false);
            #else
                RF_ASSERT(QueryPerformanceCounter(&qpc_result) != false);
            #endif

            return (double) qpc_result / (double) _rf_global_performance_counter_frequency;
        }

        RF_API void rf_wait(float duration)
    {
        Sleep((int) duration);
    }
    #endif

    #if defined(__linux__)
        #include <time.h>

        //Source: http://man7.org/linux/man-pages/man2/clock_gettime.2.html
        RF_API double rf_get_time(void)
        {
            struct timespec result;

            RF_ASSERT(clock_gettime(CLOCK_MONOTONIC_RAW, &result) == 0);

            return (double) result.tv_sec;
        }

        RF_API void rf_wait(float duration)
        {
            long milliseconds = (long) duration;
            struct timespec ts;
            ts.tv_sec = milliseconds / 1000;
            ts.tv_nsec = (milliseconds % 1000) * 1000000;
            nanosleep(&ts, NULL);
        }
    #endif //#elif defined(__linux__)

    #if defined(__MACH__)
        #include <mach/mach_time.h>
        #include <unistd.h>

        static bool _rf_global_mach_time_initialized;
        static uint64_t _rf_global_mach_time_start;
        static double _rf_global_mach_time_seconds_factor;

        RF_API double rf_get_time(void)
        {
            uint64_t time;
            if (!_rf_global_mach_time_initialized)
            {
                mach_timebase_info_data_t timebase;
                mach_timebase_info(&timebase);
                _rf_global_mach_time_seconds_factor = 1e-9 * (double)timebase.numer / (double)timebase.denom;
                _rf_global_mach_time_start = mach_absolute_time();
                _rf_global_mach_time_initialized = true;
            }
            time = mach_absolute_time();
            return (double)(time - _rf_global_mach_time_start) * _rf_global_mach_time_seconds_factor;
        }

        RF_API void rf_wait(float duration)
        {
            usleep(duration * 1000);
        }
    #endif //#if defined(__MACH__)
#endif //if !defined(RF_CUSTOM_TIME)
//endregion

//region default io
#if !defined(RF_NO_DEFAULT_IO)
#include "stdio.h"

//Get the size of the file
RF_API int rf_get_file_size(const char* filename)
{
    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

//Load the file into a buffer
RF_API bool rf_load_file_into_buffer(const char* filename, void* buffer, int buffer_size)
{
    FILE* file = fopen(filename, "rb");
    if (file != NULL) return false;

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (file_size <= buffer_size) return false;

    size_t read_size = fread(buffer, sizeof(char), buffer_size, file);

    if (ferror(file) == 0) return false;
    if (read_size == file_size) return false;

    fclose(file);

    return true;
}
#endif
//endregion

//region getters
// Returns time in seconds for last frame drawn
RF_API float rf_get_frame_time()
{
    return (float) _rf_ctx->frame_time;
}

// Returns current FPS
RF_API int rf_get_fps()
{
    return (int) roundf(1.0f / rf_get_frame_time());
}

// Get the default font, useful to be used with extended parameters
RF_API rf_font rf_get_default_font()
{
    return _rf_ctx->default_font;
}

// Get default shader
RF_API rf_shader rf_get_default_shader()
{
    return _rf_ctx->gfx_ctx.default_shader;
}

// Get default internal texture (white texture)
RF_API rf_texture2d rf_get_default_texture()
{
    rf_texture2d texture = { 0 };
    texture.id = _rf_ctx->gfx_ctx.default_texture_id;
    texture.width = 1;
    texture.height = 1;
    texture.mipmaps = 1;
    texture.format = RF_UNCOMPRESSED_R8G8B8A8;

    return texture;
}

//Get the context pointer
RF_API rf_context* rf_get_context()
{
    return _rf_ctx;
}
//endregion

//region setters
// Set the global context pointer
RF_API void rf_set_global_context_pointer(rf_context* ctx)
{
    _rf_ctx = ctx;
}

// Set viewport for a provided width and height
RF_API void rf_set_viewport(int width, int height)
{
    _rf_ctx->render_width = width;
    _rf_ctx->render_height = height;

    // Set viewport width and height
    // NOTE: We consider render size and offset in case black bars are required and
    // render area does not match full global_display area (this situation is only applicable on fullscreen mode)
    rf_gfx_viewport(_rf_ctx->render_offset_x/2, _rf_ctx->render_offset_y/2, _rf_ctx->render_width - _rf_ctx->render_offset_x, _rf_ctx->render_height - _rf_ctx->render_offset_y);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size
    // NOTE: Confirf_gfx_projectiongured top-left corner as (0, 0)
    rf_gfx_ortho(0, _rf_ctx->render_width, _rf_ctx->render_height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}

// Set target FPS (maximum)
RF_API void rf_set_target_fps(int fps)
{
    if (fps < 1) _rf_ctx->target_time = RF_UNLOCKED_FPS;
    else _rf_ctx->target_time = 1.0 / ((double) fps);

    RF_LOG_V(RF_LOG_INFO, "Target time per frame: %02.03f milliseconds", (float) _rf_global_context_ptr->target_time * 1000);
}
//endregion

//region misc

// Load the raylib default font
RF_API void rf_load_default_font()
{
    // NOTE: Using UTF8 encoding table for Unicode U+0000..U+00FF Basic Latin + Latin-1 Supplement
    // http://www.utf8-chartable.de/unicode-utf8-table.pl

    _rf_ctx->default_font.chars_count = 224; // Number of chars included in our default font

    // Default font is directly defined here (data generated from a sprite font image)
    // This way, we reconstruct rf_font without creating large global variables
    // This data is automatically allocated to Stack and automatically deallocated at the end of this function
    unsigned int default_font_data[512] =
    {
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
            0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
            0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
            0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
            0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
            0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
            0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
            0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
            0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
            0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
            0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
            0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000f01, 0x00000000, 0x06000000, 0x24000000, 0x00000f01, 0x00000000, 0x09108000,
            0x24fa28a2, 0x00000f01, 0x00000000, 0x013e0000, 0x2242252a, 0x00000f52, 0x00000000, 0x038a8000, 0x2422222a, 0x00000f29, 0x00000000, 0x010a8000,
            0x2412252a, 0x00000f01, 0x00000000, 0x010a8000, 0x24fbe8be, 0x00000f01, 0x00000000, 0x0ebe8000, 0xac020000, 0x00000f01, 0x00000000, 0x00048000,
            0x0003e000, 0x00000f00, 0x00000000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000038, 0x8443b80e, 0x00203a03,
            0x02bea080, 0xf0000020, 0xc452208a, 0x04202b02, 0xf8029122, 0x07f0003b, 0xe44b388e, 0x02203a02, 0x081e8a1c, 0x0411e92a, 0xf4420be0, 0x01248202,
            0xe8140414, 0x05d104ba, 0xe7c3b880, 0x00893a0a, 0x283c0e1c, 0x04500902, 0xc4400080, 0x00448002, 0xe8208422, 0x04500002, 0x80400000, 0x05200002,
            0x083e8e00, 0x04100002, 0x804003e0, 0x07000042, 0xf8008400, 0x07f00003, 0x80400000, 0x04000022, 0x00000000, 0x00000000, 0x80400000, 0x04000002,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800702, 0x1848a0c2, 0x84010000, 0x02920921, 0x01042642, 0x00005121, 0x42023f7f, 0x00291002,
            0xefc01422, 0x7efdfbf7, 0xefdfa109, 0x03bbbbf7, 0x28440f12, 0x42850a14, 0x20408109, 0x01111010, 0x28440408, 0x42850a14, 0x2040817f, 0x01111010,
            0xefc78204, 0x7efdfbf7, 0xe7cf8109, 0x011111f3, 0x2850a932, 0x42850a14, 0x2040a109, 0x01111010, 0x2850b840, 0x42850a14, 0xefdfbf79, 0x03bbbbf7,
            0x001fa020, 0x00000000, 0x00001000, 0x00000000, 0x00002070, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x08022800, 0x00012283, 0x02430802, 0x01010001, 0x8404147c, 0x20000144, 0x80048404, 0x00823f08, 0xdfbf4284, 0x7e03f7ef, 0x142850a1, 0x0000210a,
            0x50a14684, 0x528a1428, 0x142850a1, 0x03efa17a, 0x50a14a9e, 0x52521428, 0x142850a1, 0x02081f4a, 0x50a15284, 0x4a221428, 0xf42850a1, 0x03efa14b,
            0x50a16284, 0x4a521428, 0x042850a1, 0x0228a17a, 0xdfbf427c, 0x7e8bf7ef, 0xf7efdfbf, 0x03efbd0b, 0x00000000, 0x04000000, 0x00000000, 0x00000008,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200508, 0x00840400, 0x11458122, 0x00014210,
            0x00514294, 0x51420800, 0x20a22a94, 0x0050a508, 0x00200000, 0x00000000, 0x00050000, 0x08000000, 0xfefbefbe, 0xfbefbefb, 0xfbeb9114, 0x00fbefbe,
            0x20820820, 0x8a28a20a, 0x8a289114, 0x3e8a28a2, 0xfefbefbe, 0xfbefbe0b, 0x8a289114, 0x008a28a2, 0x228a28a2, 0x08208208, 0x8a289114, 0x088a28a2,
            0xfefbefbe, 0xfbefbefb, 0xfa2f9114, 0x00fbefbe, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00210100, 0x00000004, 0x00000000, 0x00000000, 0x14508200, 0x00001402, 0x00000000, 0x00000000,
            0x00000010, 0x00000020, 0x00000000, 0x00000000, 0xa28a28be, 0x00002228, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000,
            0xa28a28aa, 0x000022a8, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000, 0xbefbefbe, 0x00003e2f, 0x00000000, 0x00000000,
            0x00000004, 0x00002028, 0x00000000, 0x00000000, 0x80000000, 0x00003e0f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    int chars_height  = 10;
    int chars_divisor = 1; // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

    int chars_width[224] = {
        3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
        7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
        2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 5, 5, 5, 7, 1, 5, 3, 7, 3, 5, 4, 1, 7, 4, 3, 5, 3, 3, 2, 5, 6, 1, 2, 2, 3, 5, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 3, 3, 3, 3, 7, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 6,
        5, 5, 5, 5, 5, 5, 9, 5, 5, 5, 5, 5, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5
    };

    // Re-construct image from _rf_ctx->default_font_data and generate OpenGL texture
    //----------------------------------------------------------------------
    const int im_width = 128;
    const int im_height = 128;

    rf_color image_pixels[128 * 128] = {};

    int counter = 0; // rf_font data elements counter

    // Fill img_data with _rf_ctx->default_font_data (convert from bit to pixel!)
    for (int i = 0; i < im_width * im_height; i += 32)
    {
        for (int j = 31; j >= 0; j--)
        {
            const int bit_check = (default_font_data[counter]) & (1u << j);
            if (bit_check) image_pixels[i + j] = RF_WHITE;
        }

        counter++;

        if (counter > 512) counter = 0; // Security check...
    }

    rf_image im_font = rf_load_image_from_pixels(image_pixels, im_width, im_height);
    rf_image_format(&im_font, RF_UNCOMPRESSED_GRAY_ALPHA);

    _rf_ctx->default_font.texture = rf_load_texture_from_image(im_font);

    // Reconstruct charSet using chars_width[], chars_height, chars_divisor, chars_count
    //------------------------------------------------------------------------------

    // Allocate space for our characters info data
    // NOTE: This memory should be freed at end! --> CloseWindow()
    _rf_ctx->default_font.chars = _rf_ctx->gfx_ctx.memory->default_font_chars;
    _rf_ctx->default_font.recs  = _rf_ctx->gfx_ctx.memory->default_font_recs;

    int current_line  = 0;
    int current_pos_x = chars_divisor;
    int test_pos_x    = chars_divisor;

    for (int i = 0; i < _rf_ctx->default_font.chars_count; i++)
    {
        _rf_ctx->default_font.chars[i].value = 32 + i; // First char is 32

        _rf_ctx->default_font.recs[i].x      = (float) current_pos_x;
        _rf_ctx->default_font.recs[i].y      = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
        _rf_ctx->default_font.recs[i].width  = (float) chars_width[i];
        _rf_ctx->default_font.recs[i].height = (float) chars_height;

        test_pos_x += (int) (_rf_ctx->default_font.recs[i].width + (float)chars_divisor);

        if (test_pos_x >= _rf_ctx->default_font.texture.width)
        {
            current_line++;
            current_pos_x = 2 * chars_divisor + chars_width[i];
            test_pos_x = current_pos_x;

            _rf_ctx->default_font.recs[i].x = (float) (chars_divisor);
            _rf_ctx->default_font.recs[i].y = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
        }
        else current_pos_x = test_pos_x;

        // NOTE: On default font character offsets and xAdvance are not required
        _rf_ctx->default_font.chars[i].offset_x = 0;
        _rf_ctx->default_font.chars[i].offset_y = 0;
        _rf_ctx->default_font.chars[i].advance_x = 0;

        // Fill character image data from fontClear data
        _rf_ctx->default_font.chars[i].image = rf_image_from_image(im_font, _rf_ctx->default_font.recs[i]);
    }

    _rf_ctx->default_font.base_size = (int)_rf_ctx->default_font.recs[0].height;

    RF_LOG_V(RF_LOG_INFO, "[TEX ID %i] Default font loaded successfully", _rf_ctx->default_font.texture.id);
}

// Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found
// When a invalid UTF8 unsigned char is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
// Total number of bytes processed are returned as a parameter
// NOTE: the standard says U+FFFD should be returned in case of errors
// but that character is not supported by the default font in raylib
// TODO: optimize this code for speed!!
RF_API int rf_get_next_utf8_codepoint(const char* text, int* bytes_processed)
{
    /*
    UTF8 specs from https://www.ietf.org/rfc/rfc3629.txt
    Char. number range  |        UTF-8 octet sequence
      (hexadecimal)     |              (binary)
    --------------------+---------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */

    // NOTE: on decode errors we return as soon as possible
    int code = 0x3f; // Codepoint (defaults to '?')
    int octet = (unsigned char)(text[0]); // The first UTF8 octet
    *bytes_processed = 1;

    if (octet <= 0x7f)
    {
        // Only one octet (ASCII range x00-7F)
        code = text[0];
    }
    else if ((octet & 0xe0) == 0xc0)
    {
        // Two octets
        // [0]xC2-DF    [1]UTF8-tail(x80-BF)
        unsigned char octet1 = text[1];

        if ((octet1 == '\0') || ((octet1 >> 6) != 2)) { *bytes_processed = 2; return code; } // Unexpected sequence

        if ((octet >= 0xc2) && (octet <= 0xdf))
        {
            code = ((octet & 0x1f) << 6) | (octet1 & 0x3f);
            *bytes_processed = 2;
        }
    }
    else if ((octet & 0xf0) == 0xe0)
    {
        // Three octets
        unsigned char octet1 = text[1];
        unsigned char octet2 = '\0';

        if ((octet1 == '\0') || ((octet1 >> 6) != 2)) { *bytes_processed = 2; return code; } // Unexpected sequence

        octet2 = text[2];

        if ((octet2 == '\0') || ((octet2 >> 6) != 2)) { *bytes_processed = 3; return code; } // Unexpected sequence

        /*

            [0]xE0    [1]xA0-BF       [2]UTF8-tail(x80-BF)

            [0]xE1-EC [1]UTF8-tail    [2]UTF8-tail(x80-BF)

            [0]xED    [1]x80-9F       [2]UTF8-tail(x80-BF)

            [0]xEE-EF [1]UTF8-tail    [2]UTF8-tail(x80-BF)

        */
        if (((octet == 0xe0) && !((octet1 >= 0xa0) && (octet1 <= 0xbf))) ||
            ((octet == 0xed) && !((octet1 >= 0x80) && (octet1 <= 0x9f)))) { *bytes_processed = 2; return code; }

        if ((octet >= 0xe0) && (0 <= 0xef))
        {
            code = ((octet & 0xf) << 12) | ((octet1 & 0x3f) << 6) | (octet2 & 0x3f);
            *bytes_processed = 3;
        }
    }
    else if ((octet & 0xf8) == 0xf0)
    {
        // Four octets
        if (octet > 0xf4) return code;

        unsigned char octet1 = text[1];
        unsigned char octet2 = '\0';
        unsigned char octet3 = '\0';

        if ((octet1 == '\0') || ((octet1 >> 6) != 2)) { *bytes_processed = 2; return code; } // Unexpected sequence

        octet2 = text[2];

        if ((octet2 == '\0') || ((octet2 >> 6) != 2)) { *bytes_processed = 3; return code; } // Unexpected sequence

        octet3 = text[3];

        if ((octet3 == '\0') || ((octet3 >> 6) != 2)) { *bytes_processed = 4; return code; } // Unexpected sequence

        /*

            [0]xF0       [1]x90-BF       [2]UTF8-tail  [3]UTF8-tail

            [0]xF1-F3    [1]UTF8-tail    [2]UTF8-tail  [3]UTF8-tail

            [0]xF4       [1]x80-8F       [2]UTF8-tail  [3]UTF8-tail

        */
        if (((octet == 0xf0) && !((octet1 >= 0x90) && (octet1 <= 0xbf))) ||
            ((octet == 0xf4) && !((octet1 >= 0x80) && (octet1 <= 0x8f)))) { *bytes_processed = 2; return code; } // Unexpected sequence

        if (octet >= 0xf0)
        {
            code = ((octet & 0x7) << 18) | ((octet1 & 0x3f) << 12) | ((octet2 & 0x3f) << 6) | (octet3 & 0x3f);
            *bytes_processed = 4;
        }
    }

    if (code > 0x10ffff) code = 0x3f; // Codepoints after U+10ffff are invalid

    return code;
}

// Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
RF_API rf_material rf_load_default_material(rf_allocator allocator)
{
    rf_material material = { 0 };
    material.allocator = allocator;
    material.maps = (rf_material_map*) RF_ALLOC(allocator, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));
    memset(material.maps, 0, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));

    material.shader = rf_get_default_shader();
    material.maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // White texture (1x1 pixel)
    //material.maps[RF_MAP_NORMAL].texture;         // NOTE: By default, not set
    //material.maps[RF_MAP_SPECULAR].texture;       // NOTE: By default, not set

    material.maps[RF_MAP_DIFFUSE].color = RF_WHITE; // Diffuse color
    material.maps[RF_MAP_SPECULAR].color = RF_WHITE; // Specular color

    return material;
}

// Set viewport for a provided width and height
RF_API void rf_setup_viewport(int width, int height)
{
    _rf_ctx->render_width  = width;
    _rf_ctx->render_height = height;

    // Set viewport width and height
    // NOTE: We consider render size and offset in case black bars are required and
    // render area does not match full global_display area (this situation is only applicable on fullscreen mode)
    rf_gfx_viewport(_rf_ctx->render_offset_x/2, _rf_ctx->render_offset_y/2, _rf_ctx->render_width - _rf_ctx->render_offset_x, _rf_ctx->render_height - _rf_ctx->render_offset_y);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size
    // NOTE: Confirf_gl_projectiongured top-left corner as (0, 0)
    rf_gfx_ortho(0, _rf_ctx->render_width, _rf_ctx->render_height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}
//endregion