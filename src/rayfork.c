#define RAYFORK__IMPLEMENTATION_FLAG // This is used such that RF_API can be defined correctly if RAYFORK_DLL is defined.

#include "rayfork.h"

#include "rayfork-std.c"
#include "rayfork-context.c"

#include "rayfork-color.c"
#include "rayfork-unicode.c"
#include "rayfork-math.c"
#include "rayfork-camera.c"
//#include "rayfork-image.c"
#include "rayfork-texture.c"
#include "rayfork-font.c"
#include "rayfork-render-batch.c"
#include "rayfork-3d.c"
#include "rayfork-drawing.c"
//#include "rayfork_audio_loading.c"

#include "rayfork-gfx-backend-opengl.c"
//#include "rayfork_audio_backend_miniaudio.c"