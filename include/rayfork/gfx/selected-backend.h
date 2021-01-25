#ifndef RAYFORK_SELECTED_BACKEND_H
#define RAYFORK_SELECTED_BACKEND_H

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    #include "rayfork/internal/gfx/backend-opengl3.h"
#endif

#if defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    //#include "rayfork/internal/gfx/backend-sokol.h"
#endif

#endif // RAYFORK_SELECTED_BACKEND_H