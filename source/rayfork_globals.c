// Useful internal macros
#define rf_ctx   (*rf__ctx)
#define rf_gfx   (rf_ctx.gfx_ctx)
#define rf_gl    (rf_gfx.gl)
#define rf_batch (*(rf_ctx.current_batch))

// Global pointer to context struct
RF_INTERNAL rf_context* rf__ctx;
RF_INTERNAL RF_THREAD_LOCAL rf_recorded_error rf__last_error;