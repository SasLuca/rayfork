// This is used such that RF_API can be defined correctly if RAYFORK_DLL is defined.
#define RAYFORK_IMPLEMENTATION

#include "rayfork.h"

#include "rayfork_internal_assert.h"
#include "rayfork_internal_logger.h"
#include "rayfork_internal_utils.h"
#include "rayfork_internal_context.h"

#include "rayfork_dependencies.c"

#include "rayfork_3d.c"
#include "rayfork_allocator.c"
#include "rayfork_audio.c"
#include "rayfork_camera.c"
#include "rayfork_color.c"
#include "rayfork_context.c"
#include "rayfork_drawing.c"
#include "rayfork_ez.c"
#include "rayfork_font.c"
#include "rayfork_image.c"
#include "rayfork_internal_utils.c"
#include "rayfork_io.c"
#include "rayfork_logger.c"
#include "rayfork_math.c"
#include "rayfork_render_batch.c"
#include "rayfork_texture.c"
#include "rayfork_unicode.c"

#include "rayfork_gfx_backend_opengl.c"