#include "rayfork-gfx.h"

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    #include "rayfork-backend-opengl.c"
#endif

#if defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    #include "rayfork-backend-sokol.c"
#endif

#include "rayfork-camera.c"
#include "rayfork-colors.c"
#include "rayfork-image.c"
#include "rayfork-texture.c"
#include "rayfork-font.c"
#include "rayfork-model.c"
#include "rayfork-high-level-renderer.c"
#include "rayfork-render-batch.c"
#include "rayfork-context.c"