#ifndef RAYFORK_GFX_H
#define RAYFORK_GFX_H

#include "rayfork_gfx_base.h"
#include "rayfork_gfx_backend_opengl.h"

typedef struct rf_render_batch
{
    int vertex_buffers_count;
    int current_buffer;
    rf_vertex_buffer* vertex_buffers;

    int draw_calls_size;
    int draw_calls_counter;
    rf_draw_call* draw_calls;
    float current_depth; // Current depth value for next draw

    bool valid;
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

RF_API rf_render_batch rf_create_custom_render_batch_from_buffers(rf_vertex_buffer* vertex_buffers, int vertex_buffers_count, rf_draw_call* draw_calls, int draw_calls_count);
RF_API rf_render_batch rf_create_custom_render_batch(int vertex_buffers_count, int draw_calls_count, int vertex_buffer_elements_count, rf_allocator allocator);
RF_API rf_render_batch rf_create_default_render_batch(rf_allocator allocator);

RF_API void rf_set_active_render_batch(rf_render_batch* batch);
RF_API void rf_unload_render_batch(rf_render_batch batch, rf_allocator allocator);

#endif // RAYFORK_GFX_H