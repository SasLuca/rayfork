#ifndef RAYFORK_RENDER_BATCH_H
#define RAYFORK_RENDER_BATCH_H

#include "rayfork/foundation/basicdef.h"
#include "rayfork/image/pixel-format.h"
#include "rayfork/image/image.h"
#include "gfx-types.h"
#include "selected-backend.h"

typedef struct rf_render_batch
{
    rf_int vertex_buffers_count;
    rf_int current_buffer;
    rf_vertex_buffer* vertex_buffers;

    rf_int draw_calls_size;
    rf_int draw_calls_counter;
    rf_draw_call* draw_calls;
    float current_depth; // Current depth value for next draw

    rf_valid_t valid;
} rf_render_batch;

typedef struct rf_one_element_vertex_buffer
{
    rf_gfx_vertex_data_type       vertices  [1 * RF_GFX_VERTEX_COMPONENT_COUNT      ];
    rf_gfx_texcoord_data_type     texcoords [1 * RF_GFX_TEXCOORD_COMPONENT_COUNT    ];
    rf_gfx_color_data_type        colors    [1 * RF_GFX_COLOR_COMPONENT_COUNT       ];
    rf_gfx_vertex_index_data_type indices   [1 * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT];
} rf_one_element_vertex_buffer;

typedef struct rf_default_vertex_buffer
{
    rf_gfx_vertex_data_type       vertices  [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_VERTEX_COMPONENT_COUNT      ];
    rf_gfx_texcoord_data_type     texcoords [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_TEXCOORD_COMPONENT_COUNT    ];
    rf_gfx_color_data_type        colors    [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_COLOR_COMPONENT_COUNT       ];
    rf_gfx_vertex_index_data_type indices   [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT];
} rf_default_vertex_buffer;

typedef struct rf_default_render_batch
{
    rf_vertex_buffer         vertex_buffers        [RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT];
    rf_draw_call             draw_calls            [RF_DEFAULT_BATCH_DRAW_CALLS_COUNT    ];
    rf_default_vertex_buffer vertex_buffers_memory [RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT];
} rf_default_render_batch;

rf_extern rf_render_batch rf_create_custom_render_batch_from_buffers(rf_vertex_buffer* vertex_buffers, rf_int vertex_buffers_count, rf_draw_call* draw_calls, rf_int draw_calls_count);
rf_extern rf_render_batch rf_create_custom_render_batch(rf_int vertex_buffers_count, rf_int draw_calls_count, rf_int vertex_buffer_elements_count, rf_allocator allocator);
rf_extern rf_render_batch rf_create_default_render_batch(rf_allocator allocator);

rf_extern void rf_set_active_render_batch(rf_render_batch* batch);
rf_extern void rf_render_batch_free(rf_render_batch batch, rf_allocator allocator);

#endif // RAYFORK_RENDER_BATCH_H