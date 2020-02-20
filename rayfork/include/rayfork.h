#ifndef RAYFORK_H
#define RAYFORK_H

#include "rayfork_common.h"

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
RF_API void rf_init_context(rf_context* rf_ctx, rf_memory* memory, int width, int height, rf_opengl_procs gl);
#endif

//region time functions
#if defined(RF_CUSTOM_TIME)
    RF_API void rf_set_time_functions(void (*wait_func)(float), double (*get_time_func)(void)); //Used to set custom functions
#endif

RF_API double rf_get_time(void); // Wait for some milliseconds (pauses program execution)
RF_API void rf_wait(float duration); // Returns elapsed time in seconds since rf_context_init
//endregion

//region default io
#if !defined(RF_NO_DEFAULT_IO)
    RF_API int rf_get_file_size(const char* filename); //Get the size of the file
    RF_API bool rf_load_file_into_buffer(const char* filename, void* buffer, int buffer_size); //Load the file into a buffer

    #define RF_DEFAULT_IO (RF_LIT(rf_io_callbacks) { rf_get_file_size, rf_load_file_into_buffer })
#endif
//endregion

//region getters
RF_API float rf_get_frame_time(); // Returns time in seconds for last frame drawn. Returns 0 if time functions are set to NULL
RF_API int rf_get_fps(); // Returns current FPS. Returns 0 if time functions are set to NULL
RF_API rf_font rf_get_default_font(); // Get the default font, useful to be used with extended parameters
RF_API rf_shader rf_get_default_shader(); // Get default shader
RF_API rf_texture2d rf_get_default_texture(); // Get default internal texture (white texture)
RF_API rf_context* rf_get_context(); //Get the context pointer
//endregion

//region setters
RF_API void rf_set_global_context_pointer(rf_context* ctx); // Set the global context pointer
RF_API void rf_set_viewport(int width, int height); // Set viewport for a provided width and height
RF_API void rf_set_target_fps(int fps); // Set target FPS (maximum). Ignored if time functions are set to NULL
RF_API void rf_set_shapes_texture(rf_texture2d texture, rf_rec source); // Define default texture used to draw shapes
//endregion

RF_API void rf_setup_viewport(int width, int height); // Set viewport for a provided width and height
RF_API void rf_load_default_font(); // Load the raylib default font
RF_API rf_material rf_load_default_material(rf_allocator allocator); // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)

#endif //#ifndef RAYFORK_H