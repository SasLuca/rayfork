RF_API rf_render_batch rf_create_custom_render_batch_from_buffers(rf_vertex_buffer* vertex_buffers, rf_int vertex_buffers_count, rf_draw_call* draw_calls, rf_int draw_calls_count)
{
    if (!vertex_buffers || !draw_calls || vertex_buffers_count < 0 || draw_calls_count < 0) {
        return (rf_render_batch) {0};
    }

    rf_render_batch batch = {0};
    batch.vertex_buffers = vertex_buffers;
    batch.vertex_buffers_count = vertex_buffers_count;
    batch.draw_calls = draw_calls;
    batch.draw_calls_size = draw_calls_count;

    for (rf_int i = 0; i < vertex_buffers_count; i++)
    {
        memset(vertex_buffers[i].vertices,  0, RF_GFX_VERTEX_COMPONENT_COUNT   * vertex_buffers[i].elements_count);
        memset(vertex_buffers[i].texcoords, 0, RF_GFX_TEXCOORD_COMPONENT_COUNT * vertex_buffers[i].elements_count);
        memset(vertex_buffers[i].colors,    0, RF_GFX_COLOR_COMPONENT_COUNT    * vertex_buffers[i].elements_count);

        int k = 0;

        // Indices can be initialized right now
        for (rf_int j = 0; j < (RF_GFX_VERTEX_INDEX_COMPONENT_COUNT * vertex_buffers[i].elements_count); j += 6)
        {
            vertex_buffers[i].indices[j + 0] = 4 * k + 0;
            vertex_buffers[i].indices[j + 1] = 4 * k + 1;
            vertex_buffers[i].indices[j + 2] = 4 * k + 2;
            vertex_buffers[i].indices[j + 3] = 4 * k + 0;
            vertex_buffers[i].indices[j + 4] = 4 * k + 2;
            vertex_buffers[i].indices[j + 5] = 4 * k + 3;

            k++;
        }

        vertex_buffers[i].v_counter  = 0;
        vertex_buffers[i].tc_counter = 0;
        vertex_buffers[i].c_counter  = 0;

        rf_gfx_init_vertex_buffer(&vertex_buffers[i]);
    }

    for (rf_int i = 0; i < RF_DEFAULT_BATCH_DRAW_CALLS_COUNT; i++)
    {
        batch.draw_calls[i] = (rf_draw_call) {
            .mode = RF_QUADS,
            .texture_id = rf_ctx.default_texture_id,
        };
    }

    batch.draw_calls_counter = 1; // Reset draws counter
    batch.current_depth = -1.0f;  // Reset depth value
    batch.valid = true;

    return batch;
}

// TODO: Not working yet
RF_API rf_render_batch rf_create_custom_render_batch(rf_int vertex_buffers_count, rf_int draw_calls_count, rf_int vertex_buffer_elements_count, rf_allocator allocator)
{
    if (vertex_buffers_count < 0 || draw_calls_count < 0 || vertex_buffer_elements_count < 0) {
        return (rf_render_batch) {0};
    }

    rf_render_batch result = {0};

    rf_int vertex_buffer_array_size = sizeof(rf_vertex_buffer) * vertex_buffers_count;
    rf_int vertex_buffers_memory_size = (sizeof(rf_one_element_vertex_buffer) * vertex_buffer_elements_count) * vertex_buffers_count;
    rf_int draw_calls_array_size = sizeof(rf_draw_call) * draw_calls_count;
    rf_int allocation_size = vertex_buffer_array_size + draw_calls_array_size + vertex_buffers_memory_size;

    char* memory = RF_ALLOC(allocator, allocation_size);

    if (memory)
    {
        rf_vertex_buffer* buffers = (rf_vertex_buffer*) memory;
        rf_draw_call* draw_calls = (rf_draw_call*) (memory + vertex_buffer_array_size);
        char* buffers_memory = memory + vertex_buffer_array_size + draw_calls_array_size;

        RF_ASSERT(((char*)draw_calls - memory) == draw_calls_array_size + vertex_buffers_memory_size);
        RF_ASSERT((buffers_memory - memory) == vertex_buffers_memory_size);
        RF_ASSERT((buffers_memory - memory) == sizeof(rf_one_element_vertex_buffer) * vertex_buffer_elements_count * vertex_buffers_count);

        for (rf_int i = 0; i < vertex_buffers_count; i++)
        {
            rf_int one_vertex_buffer_memory_size = sizeof(rf_one_element_vertex_buffer) * vertex_buffer_elements_count;
            rf_int vertices_size = sizeof(rf_gfx_vertex_data_type) * vertex_buffer_elements_count;
            rf_int texcoords_size = sizeof(rf_gfx_texcoord_data_type) * vertex_buffer_elements_count;
            rf_int colors_size = sizeof(rf_gfx_color_data_type) * vertex_buffer_elements_count;
            rf_int indices_size = sizeof(rf_gfx_vertex_index_data_type) * vertex_buffer_elements_count;

            char* this_buffer_memory = buffers_memory + one_vertex_buffer_memory_size * i;

            buffers[i].elements_count = vertex_buffer_elements_count;
            buffers[i].vertices       = (rf_gfx_vertex_data_type*)       this_buffer_memory;
            buffers[i].texcoords      = (rf_gfx_texcoord_data_type*)     this_buffer_memory + vertices_size;
            buffers[i].colors         = (rf_gfx_color_data_type*)        this_buffer_memory + vertices_size + texcoords_size;
            buffers[i].indices        = (rf_gfx_vertex_index_data_type*) this_buffer_memory + vertices_size + texcoords_size + colors_size;
        }

        result = rf_create_custom_render_batch_from_buffers(buffers, vertex_buffers_count, draw_calls, draw_calls_count);
    }

    return result;
}

RF_API rf_render_batch rf_create_default_render_batch_from_memory(rf_default_render_batch* memory)
{
    if (!memory) {
        return (rf_render_batch) {0};
    }

    for (rf_int i = 0; i < RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT; i++)
    {
        memory->vertex_buffers[i].elements_count = RF_DEFAULT_BATCH_ELEMENTS_COUNT;
        memory->vertex_buffers[i].vertices = memory->vertex_buffers_memory[i].vertices;
        memory->vertex_buffers[i].texcoords = memory->vertex_buffers_memory[i].texcoords;
        memory->vertex_buffers[i].colors = memory->vertex_buffers_memory[i].colors;
        memory->vertex_buffers[i].indices = memory->vertex_buffers_memory[i].indices;
    }

    return rf_create_custom_render_batch_from_buffers(memory->vertex_buffers, RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT, memory->draw_calls, RF_DEFAULT_BATCH_DRAW_CALLS_COUNT);
}

RF_API rf_render_batch rf_create_default_render_batch(rf_allocator allocator)
{
    rf_default_render_batch* memory = RF_ALLOC(allocator, sizeof(rf_default_render_batch));
    return rf_create_default_render_batch_from_memory(memory);
}

RF_API void rf_set_active_render_batch(rf_render_batch* batch)
{
    rf_ctx.current_batch = batch;
}

RF_API void rf_unload_render_batch(rf_render_batch batch, rf_allocator allocator)
{
    RF_FREE(allocator, batch.vertex_buffers);
}