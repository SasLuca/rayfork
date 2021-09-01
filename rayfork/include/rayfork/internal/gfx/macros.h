#ifndef RAYFORK_INTERNAL_GFX_MACROS_H
#define RAYFORK_INTERNAL_GFX_MACROS_H

#define rf_ctx   (*rf_global_gfx_context)
#define rf_batch (*(rf_ctx.current_batch))
#define rf_gfx   (rf_ctx.gfx_backend_data)
#define rf_gl    (rf_gfx.gl)

#endif // RAYFORK_INTERNAL_GFX_MACROS_H