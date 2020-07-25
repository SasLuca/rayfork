#ifndef RAYFORK_COMMON_INTERNAL_H
#define RAYFORK_COMMON_INTERNAL_H

// Useful internal macros
#define rf_ctx   (*rf__ctx)
#define rf_gfx   (rf_ctx.gfx_ctx)
#define rf_gl    (rf_gfx.gl)
#define rf_batch (*(rf_ctx.current_batch))

// Global pointer to context struct
RF_INTERNAL rf_context* rf__ctx;
RF_INTERNAL RF_THREAD_LOCAL rf_error_type rf__last_error;

RF_INTERNAL void rf_gfx_backend_internal_init(rf_gfx_backend_data* gfx_data);

#endif // RAYFORK_COMMON_INTERNAL_H