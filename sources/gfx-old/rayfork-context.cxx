#include "rayfork-gfx-internal.h"
#include "rayfork-texture.hxx"

#pragma region getters

// Get the default font, useful to be used with extended parameters
rf_extern rf_bitmap_font rf_get_default_font()
{
    return rf_ctx.default_font;
}

rf_extern rf_log_type rf_get_current_log_filter()
{
    return rf_ctx.logger_filter;
}

#pragma endregion