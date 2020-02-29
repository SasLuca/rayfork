/**********************************************************************************************
*
*   rayfork_renderer - A single header library 2D/3D rendering
*   rayfork - a set of header only libraries that are forked from the amazing Raylib (https://www.raylib.com/)
*
*   FEATURES:
*       - Cross-platform with no default platform layer
*       - 2D shapes, textures and ninepatch rendering
*       - 3D rendering
*       - Text rendering
*       - Async asset loading
*
*   DEPENDENCIES:
*       - stb_image
*       - stb_image_write
*       - stb_image_resize
*       - stb_perlin
*       - stb_rect_pack
*       - stb_truetype
*       - cgltf
*       - par_shapes
*       - tinyobj_loader_c
*
***********************************************************************************************/

// Load animated GIF data
//  - rf_image.data buffer includes all frames: [image#0][image#1][image#2][...]
//  - Number of frames is returned through 'frames' parameter
//  - Frames delay is returned through 'delays' parameter (int array)
//  - All frames are returned in RGBA format

//region interface implementation
// Measure string width for default font
// Todo(LucaSas): Merge with rf_measure_text
// @Deprecated
RF_API float _rf_measure_height_of_wrapped_text(rf_font font, float font_size, const char* text, int length, float container_width)
{
    rf_sizef unwrapped_string_size = rf_measure_text(font, text, length, font_size, 1.0f);
    if (unwrapped_string_size.width <= container_width)
    {
        return unwrapped_string_size.height;
    }

    rf_rec rec = { 0, 0, container_width, FLT_MAX };
    int text_offset_x = 0; // Offset between characters
    int text_offset_y = 0; // Required for line break!
    float scale_factor = 0.0f;
    float spacing = 1.0f;

    int letter = 0; // Current character
    int index = 0; // Index position in sprite font

    scale_factor = font_size/font.base_size;

    enum
    {
        MEASURE_STATE = 0,
        DRAW_STATE = 1
    };

    int state = MEASURE_STATE;
    int start_line = -1; // Index where to begin drawing (where a line begins)
    int end_line = -1; // Index where to stop drawing (where a line ends)
    int lastk = -1; // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        int glyph_width = 0;
        int next = 0;
        letter = _rf_get_next_utf8_codepoint(&text[i], &next);
        index = rf_get_glyph_index(font, letter);

        // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) next = 1;
        i += next - 1;

        if (letter != '\n')
        {
            glyph_width = (font.chars[index].advance_x == 0)?
                          (int)(font.recs[index].width*scale_factor + spacing):
                          (int)(font.chars[index].advance_x*scale_factor + spacing);
        }

        // NOTE: When word_wrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in start_line and end_line, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When word_wrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            // TODO: there are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // See: http://jkorpela.fi/chars/spaces.html
            if ((letter == ' ') || (letter == '\t') || (letter == '\n')) end_line = i;

            if ((text_offset_x + glyph_width + 1) >= rec.width)
            {
                end_line = (end_line < 1)? i : end_line;
                if (i == end_line) end_line -= next;
                if ((start_line + next) == end_line) end_line = i - next;
                state = !state;
            }
            else if ((i + 1) == length)
            {
                end_line = i;
                state = !state;
            }
            else if (letter == '\n')
            {
                state = !state;
            }

            if (state == DRAW_STATE)
            {
                text_offset_x = 0;
                i = start_line;
                glyph_width = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if ((text_offset_y + (int)(font.base_size*scale_factor)) > rec.height) break;

            if (i == end_line)
            {
                text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                text_offset_x = 0;
                start_line = end_line;
                end_line = -1;
                glyph_width = 0;
                k = lastk;
                state = !state;
            }
        }

        text_offset_x += glyph_width;
    }

    return text_offset_y;
}

/***********************************************************************************
    Function implemented by Wilhem Barbier (@wbrbr)

    Features:
      - Supports .gltf and .glb files
      - Supports embedded (base64) or external textures
      - Loads the albedo/diffuse texture (other maps could be added)
      - Supports multiple mesh per model and multiple primitives per model

    Some restrictions (not exhaustive):
      - Triangle-only meshes
      - Not supported node hierarchies or transforms
      - Only loads the diffuse texture... but not too hard to support other maps (normal, roughness/metalness...)
      - Only supports unsigned short indices (no unsigned char/unsigned int)
      - Only supports float for texture coordinates (no unsigned char/unsigned short)
    *************************************************************************************/
RF_API rf_model rf_load_model_from_gltf(const char* data, int data_size)
{
    #define rf_load_accessor(type, nbcomp, acc, dst)\
    { \
        int n = 0;\
        type* buf = (type*)acc->buffer_view->buffer->data+acc->buffer_view->offset/sizeof(type)+acc->offset/sizeof(type);\
        for (int k = 0; k < acc->count; k++) {\
            for (int l = 0; l < nbcomp; l++) {\
                dst[nbcomp*k+l] = buf[n+l];\
            }\
            n += acc->stride/sizeof(type);\
        }\
    }

    rf_model model = { 0 };

    void* buffer = RF_MALLOC(data_size);
    memcpy(buffer, data, data_size);

    // glTF data loading
    cgltf_options options = { cgltf_file_type_invalid };
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse(&options, buffer, size, &data);

    if (result == cgltf_result_success)
    {
        RF_LOG(RF_LOG_INFO, "[%s][%s] rf_model meshes/materials: %i/%i", file_name, (data->file_type == 2)? "glb" : "gltf", data->meshes_count, data->materials_count);

        // Read data buffers
        result = cgltf_load_buffers(&options, data, file_name);
        if (result != cgltf_result_success) RF_LOG(RF_LOG_INFO, "[%s][%s] Error loading mesh/material buffers", file_name, (data->file_type == 2)? "glb" : "gltf");

        int primitivesCount = 0;

        for (int i = 0; i < data->meshes_count; i++) primitivesCount += (int)data->meshes[i].primitives_count;

        // Process glTF data and map to model
        model.mesh_count = primitivesCount;
        model.meshes = (rf_mesh*) RF_MALLOC(model.mesh_count * sizeof(rf_mesh));
        memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
        model.material_count = data->materials_count + 1;
        model.materials = (rf_material*) RF_MALLOC(model.material_count * sizeof(rf_material));
        model.mesh_material = (int*) RF_MALLOC(model.mesh_count * sizeof(int));

        for (int i = 0; i < model.mesh_count; i++)
        {
            model.meshes[i].vbo_id = (unsigned int* )RF_MALLOC(RF_MAX_MESH_VBO * sizeof(unsigned int));
            memset(model.meshes[i].vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));
        }

        //For each material
        for (int i = 0; i < model.material_count - 1; i++)
        {
            model.materials[i] = rf_load_default_material();
            rf_color tint = (rf_color){ 255, 255, 255, 255 };
            const char* tex_path = _rf_get_directory_path(file_name);

            //Ensure material follows raylib support for PBR (metallic/roughness flow)
            if (data->materials[i].has_pbr_metallic_roughness)
            {
                float roughness = data->materials[i].pbr_metallic_roughness.roughness_factor;
                float metallic = data->materials[i].pbr_metallic_roughness.metallic_factor;

                // NOTE: rf_material name not used for the moment
                //if (model.materials[i].name && data->materials[i].name) strcpy(model.materials[i].name, data->materials[i].name);

                // TODO: REview: shouldn't these be *255 ???
                tint.r = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[0]*255);
                tint.g = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[1]*255);
                tint.b = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[2]*255);
                tint.a = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[3]*255);

                model.materials[i].maps[RF_MAP_ROUGHNESS].color = tint;

                if (data->materials[i].pbr_metallic_roughness.base_color_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_ALBEDO].texture = _rf_load_texture_from_cgltf_image(data->materials[i].pbr_metallic_roughness.base_color_texture.texture->image, tex_path, tint);
                }

                // NOTE: Tint isn't need for other textures.. pass null or clear?
                // Just set as white, multiplying by white has no effect
                tint = RF_WHITE;

                if (data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_ROUGHNESS].texture = _rf_load_texture_from_cgltf_image(data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture->image, tex_path, tint);
                }
                model.materials[i].maps[RF_MAP_ROUGHNESS].value = roughness;
                model.materials[i].maps[RF_MAP_METALNESS].value = metallic;

                if (data->materials[i].normal_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_NORMAL].texture = _rf_load_texture_from_cgltf_image(data->materials[i].normal_texture.texture->image, tex_path, tint);
                }

                if (data->materials[i].occlusion_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_OCCLUSION].texture = _rf_load_texture_from_cgltf_image(data->materials[i].occlusion_texture.texture->image, tex_path, tint);
                }
            }
        }

        model.materials[model.material_count - 1] = rf_load_default_material();

        int primitiveIndex = 0;

        for (int i = 0; i < data->meshes_count; i++)
        {
            for (int p = 0; p < data->meshes[i].primitives_count; p++)
            {
                for (int j = 0; j < data->meshes[i].primitives[p].attributes_count; j++)
                {
                    if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_position)
                    {
                        cgltf_accessor *acc = data->meshes[i].primitives[p].attributes[j].data;
                        model.meshes[primitiveIndex].vertex_count = acc->count;
                        model.meshes[primitiveIndex].vertices = (float*) RF_MALLOC(sizeof(float)*model.meshes[primitiveIndex].vertex_count * 3);

                        rf_load_accessor(float, 3, acc, model.meshes[primitiveIndex].vertices)
                    }
                    else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_normal)
                    {
                        cgltf_accessor *acc = data->meshes[i].primitives[p].attributes[j].data;
                        model.meshes[primitiveIndex].normals = (float*) RF_MALLOC(sizeof(float)*acc->count * 3);

                        rf_load_accessor(float, 3, acc, model.meshes[primitiveIndex].normals)
                    }
                    else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_texcoord)
                    {
                        cgltf_accessor *acc = data->meshes[i].primitives[p].attributes[j].data;

                        if (acc->component_type == cgltf_component_type_r_32f)
                        {
                            model.meshes[primitiveIndex].texcoords = (float*) RF_MALLOC(sizeof(float)*acc->count * 2);
                            rf_load_accessor(float, 2, acc, model.meshes[primitiveIndex].texcoords)
                        }
                        else
                        {
                            // TODO: Support normalized unsigned unsigned char/unsigned short texture coordinates
                            RF_LOG(RF_LOG_WARNING, "[%s] rf_texture coordinates must be float", file_name);
                        }
                    }
                }

                cgltf_accessor *acc = data->meshes[i].primitives[p].indices;

                if (acc)
                {
                    if (acc->component_type == cgltf_component_type_r_16u)
                    {
                        model.meshes[primitiveIndex].triangle_count = acc->count/3;
                        model.meshes[primitiveIndex].indices = (unsigned short*) RF_MALLOC(sizeof(unsigned short)*model.meshes[primitiveIndex].triangle_count * 3);
                        rf_load_accessor(unsigned short, 1, acc, model.meshes[primitiveIndex].indices)
                    }
                    else
                    {
                        // TODO: Support unsigned unsigned char/unsigned int
                        RF_LOG(RF_LOG_WARNING, "[%s] Indices must be unsigned short", file_name);
                    }
                }
                else
                {
                    // Unindexed mesh
                    model.meshes[primitiveIndex].triangle_count = model.meshes[primitiveIndex].vertex_count/3;
                }

                if (data->meshes[i].primitives[p].material)
                {
                    // Compute the offset
                    model.mesh_material[primitiveIndex] = data->meshes[i].primitives[p].material - data->materials;
                }
                else
                {
                    model.mesh_material[primitiveIndex] = model.material_count - 1;
                }

                primitiveIndex++;
            }
        }

        cgltf_free(data);
    }
    else RF_LOG(RF_LOG_WARNING, "[%s] glTF data could not be loaded", file_name);

    RF_FREE(buffer);

    return model;
}


// Load materials from model file
// @Incomplete
// Note: Depends on IO stuff, fix later
/*
RF_API rf_material* rf_load_materials_from_mtl(const char* data, int data_size, int* material_count)
{
    rf_material* materials = NULL;
    unsigned int count = 0;

    // TODO: Support IQM and GLTF for materials parsing

    tinyobj_material_t *mats;

    int result = tinyobj_parse_mtl_file(&mats, (size_t*) &count, file_name);
    if (result != TINYOBJ_SUCCESS) {
        RF_LOG(RF_LOG_WARNING, "[%s] Could not parse Materials file", file_name);
    }

    // TODO: Process materials to return

    tinyobj_materials_free(mats, count);

    // Set materials shader to default (DIFFUSE, SPECULAR, NORMAL)
    for (int i = 0; i < count; i++) materials[i].shader = rf_get_default_shader();

    *material_count = count;
    return materials;
}
*/
//endregion