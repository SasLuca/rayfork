#ifndef RAYFORK_BACKEND_H
#define RAYFORK_BACKEND_H

#if !defined(rayfork_graphics_backend_gl33) && !defined(rayfork_graphics_backend_gl_es3) && !defined(rayfork_graphics_backend_metal) && !defined(rayfork_graphics_backend_directx)
    #define rayfork_no_graphics_backend_selected_by_the_user (1)
#endif

// If no graphics backend was set, choose OpenGL33 on desktop and OpenGL ES3 on mobile
#if rayfork_no_graphics_backend_selected_by_the_user
    #if defined(rayfork_platform_windows) || defined(rayfork_platform_linux) || defined(rayfork_platform_macos)
        #define rayfork_graphics_backend_gl33 (1)
    #else // if on mobile
        #define rayfork_graphics_backend_gl_es3 (1)
    #endif
#endif

// Check to make sure only one graphics backend was selected
#if (defined(rayfork_graphics_backend_gl33) + defined(rayfork_graphics_backend_gl_es3) + defined(rayfork_graphics_backend_metal) + defined(rayfork_graphics_backend_directx)) != 1
    #error rayfork error: you can only set one graphics backend but 2 or more were detected.
#endif

#if defined(rayfork_graphics_backend_gl33) || defined(rayfork_graphics_backend_gl_es3)
    #include "rayfork/gfx/backends/gl/backend-opengl.h"
#endif

#if defined(rayfork_graphics_backend_metal) || defined(rayfork_graphics_backend_directx)
    #include "rayfork/gfx/backends/sokol/backend-sokol.h"
#endif

#endif // RAYFORK_BACKEND_H