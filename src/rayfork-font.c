#pragma region dependencies

#pragma region stb_truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(sz, u) RF_ALLOC(rf__global_allocator_for_dependencies, sz)
#define STBTT_free(p, u)    RF_FREE(rf__global_allocator_for_dependencies, p)
#define STBTT_assert(it)    RF_ASSERT(it)
#define STBTT_STATIC
#include "stb_truetype.h"
#pragma endregion

#pragma endregion

#pragma region ttf font

RF_API rf_ttf_font_info rf_parse_ttf_font(const void* ttf_data, rf_int font_size)
{
    rf_ttf_font_info result = {0};

    if (ttf_data && font_size > 0)
    {
        stbtt_fontinfo font_info = {0};
        if (stbtt_InitFont(&font_info, ttf_data, 0))
        {
            // Calculate font scale factor
            float scale_factor = stbtt_ScaleForPixelHeight(&font_info, (float)font_size);

            // Calculate font basic metrics
            // NOTE: ascent is equivalent to font baseline
            int ascent, descent, line_gap;
            stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

            result = (rf_ttf_font_info)
            {
                .ttf_data = ttf_data,
                .font_size = font_size,
                .scale_factor = scale_factor,
                .ascent = ascent,
                .descent = descent,
                .line_gap = line_gap,
                .valid = true,
            };

            RF_ASSERT(sizeof(stbtt_fontinfo) == sizeof(result.internal_stb_font_info));
            memcpy(&result.internal_stb_font_info, &font_info, sizeof(stbtt_fontinfo));
        }
        else RF_LOG_ERROR(RF_STBTT_FAILED, "STB failed to parse ttf font.");
    }

    return result;
}

RF_API void rf_compute_ttf_font_glyph_metrics(rf_ttf_font_info* font_info, const int* codepoints, rf_int codepoints_count, rf_glyph_info* dst, rf_int dst_count)
{
    if (font_info && font_info->valid)
    {
        if (dst && dst_count >= codepoints_count)
        {
            // The stbtt functions called here should not require any allocations
            RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(RF_NULL_ALLOCATOR);

            font_info->largest_glyph_size = 0;

            float required_area = 0;

            // NOTE: Using simple packaging, one char after another
            for (rf_int i = 0; i < codepoints_count; i++)
            {
                stbtt_fontinfo* stbtt_ctx = (stbtt_fontinfo*) &font_info->internal_stb_font_info;

                dst[i].codepoint = codepoints[i];

                int begin_x = 0;
                int begin_y = 0;
                int end_x   = 0;
                int end_y   = 0;
                stbtt_GetCodepointBitmapBox(stbtt_ctx, dst[i].codepoint, font_info->scale_factor, font_info->scale_factor, &begin_x, &begin_y, &end_x, &end_y);

                dst[i].width  = end_x - begin_x;
                dst[i].height = end_y - begin_y;
                dst[i].offset_x = begin_x;
                dst[i].offset_y = begin_y;
                dst[i].offset_y += (int) ((float)font_info->ascent * font_info->scale_factor);

                stbtt_GetCodepointHMetrics(stbtt_ctx, dst[i].codepoint, &dst[i].advance_x, NULL);
                dst[i].advance_x *= font_info->scale_factor;

                const int char_size = dst[i].width * dst[i].height;
                if (char_size > font_info->largest_glyph_size) font_info->largest_glyph_size = char_size;
            }
        }
    }
}

// Note: the atlas is square and this value is the width of the atlas
RF_API int rf_compute_ttf_font_atlas_width(int padding, rf_glyph_info* glyph_metrics, rf_int glyphs_count)
{
    int result = 0;

    for (rf_int i = 0; i < glyphs_count; i++)
    {
        // Calculate the area of all glyphs + padding
        // The padding is applied both on the (left and right) and (top and bottom) of the glyph, which is why we multiply by 2
        result += ((glyph_metrics[i].width + 2 * padding) * (glyph_metrics[i].height + 2 * padding));
    }

    // Calculate the width required for a square atlas containing all glyphs
    result = rf_next_pot(sqrtf(result) * 1.3f);

    return result;
}

RF_API rf_image rf_generate_ttf_font_atlas(rf_ttf_font_info* font_info, int atlas_width, int padding, rf_glyph_info* glyphs, rf_int glyphs_count, rf_font_antialias antialias, unsigned short* dst, rf_int dst_count, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (font_info && font_info->valid)
    {
        int atlas_pixel_count = atlas_width * atlas_width;

        if (dst && dst_count >= atlas_pixel_count)
        {
            memset(dst, 0, atlas_pixel_count * 2);

            int largest_glyph_size = 0;
            for (rf_int i = 0; i < glyphs_count; i++)
            {
                int area = glyphs[i].width * glyphs[i].height;
                if (area > largest_glyph_size)
                {
                    largest_glyph_size = area;
                }
            }

            // Allocate a grayscale buffer large enough to store the largest glyph
            unsigned char* glyph_buffer = RF_ALLOC(temp_allocator, largest_glyph_size);

            // Use basic packing algorithm to generate the atlas
            if (glyph_buffer)
            {
                // We update these for every pixel in the loop
                int offset_x = RF_BUILTIN_FONT_PADDING;
                int offset_y = RF_BUILTIN_FONT_PADDING;

                // Set the allocator for stbtt
                RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(temp_allocator);
                {
                    // Using simple packaging, one char after another
                    for (rf_int i = 0; i < glyphs_count; i++)
                    {
                        // Extract these variables to shorter names
                        stbtt_fontinfo* stbtt_ctx = (stbtt_fontinfo*) &font_info->internal_stb_font_info;

                        // Get glyph bitmap
                        stbtt_MakeCodepointBitmap(stbtt_ctx, glyph_buffer, glyphs[i].width, glyphs[i].height, glyphs[i].width, font_info->scale_factor, font_info->scale_factor, glyphs[i].codepoint);

                        // Copy pixel data from fc.data to atlas
                        for (rf_int y = 0; y < glyphs[i].height; y++)
                        {
                            for (rf_int x = 0; x < glyphs[i].width; x++)
                            {
                                unsigned char glyph_pixel = glyph_buffer[y * ((int)glyphs[i].width) + x];
                                if (antialias == RF_FONT_NO_ANTIALIAS && glyph_pixel > RF_BITMAP_ALPHA_THRESHOLD) glyph_pixel = 0;

                                int dst_index = (offset_y + y) * atlas_width + (offset_x + x);
                                // dst is in RF_UNCOMPRESSED_GRAY_ALPHA which is 2 bytes
                                // for fonts we write the glyph_pixel in the alpha channel which is byte 2
                                ((unsigned char*)(&dst[dst_index]))[0] = 255;
                                ((unsigned char*)(&dst[dst_index]))[1] = glyph_pixel;
                            }
                        }

                        // Fill chars rectangles in atlas info
                        glyphs[i].x = (float)offset_x;
                        glyphs[i].y = (float)offset_y;

                        // Move atlas position X for next character drawing
                        offset_x += (glyphs[i].width + 2 * padding);
                        if (offset_x >= (atlas_width - glyphs[i].width - 2 * padding))
                        {
                            offset_x  = padding;
                            offset_y += font_info->font_size + padding;

                            if (offset_y > (atlas_width - font_info->font_size - padding)) break;
                            //else error dst buffer is not big enough
                        }
                    }
                }
                RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(RF_NULL_ALLOCATOR);

                result.data   = dst;
                result.width  = atlas_width;
                result.height = atlas_width;
                result.format = RF_UNCOMPRESSED_GRAY_ALPHA;
                result.valid  = true;
            }

            RF_FREE(temp_allocator, glyph_buffer);
        }
    }

    return result;
}

RF_API rf_font rf_ttf_font_from_atlas(int font_size, rf_image atlas, rf_glyph_info* glyph_metrics, rf_int glyphs_count)
{
    rf_font result = {0};

    rf_texture2d texture = rf_load_texture_from_image(atlas);

    if (texture.valid)
    {
        result = (rf_font)
        {
            .glyphs = glyph_metrics,
            .glyphs_count = glyphs_count,
            .texture = texture,
            .base_size = font_size,
            .valid = true,
        };
    }

    return result;
}

// Load rf_font from TTF font file with generation parameters
// NOTE: You can pass an array with desired characters, those characters should be available in the font
// if array is NULL, default char set is selected 32..126
RF_API rf_font rf_load_ttf_font_from_data(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, rf_int char_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_font result = {0};

    rf_ttf_font_info font_info = rf_parse_ttf_font(font_file_data, font_size);

    // Get the glyph metrics
    rf_glyph_info* glyph_metrics = RF_ALLOC(allocator, char_count * sizeof(rf_glyph_info));
    rf_compute_ttf_font_glyph_metrics(&font_info, chars, char_count, glyph_metrics, char_count);

    // Build the atlas and font
    int atlas_size = rf_compute_ttf_font_atlas_width(RF_BUILTIN_FONT_PADDING, glyph_metrics, char_count);
    int atlas_pixel_count = atlas_size * atlas_size;
    unsigned short* atlas_buffer = RF_ALLOC(temp_allocator, atlas_pixel_count * sizeof(unsigned short));
    rf_image atlas = rf_generate_ttf_font_atlas(&font_info, atlas_size, RF_BUILTIN_FONT_PADDING, glyph_metrics, char_count, antialias, atlas_buffer, atlas_pixel_count, temp_allocator);

    // Get the font
    result = rf_ttf_font_from_atlas(font_size, atlas, glyph_metrics, char_count);

    // Free the atlas bitmap
    RF_FREE(temp_allocator, atlas_buffer);

    return result;
}

// Load rf_font from file into GPU memory (VRAM)
RF_API rf_font rf_load_ttf_font_from_file(const char* filename, int font_size, rf_font_antialias antialias, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_font font = {0};

    if (rf_is_file_extension(filename, ".ttf") || rf_is_file_extension(filename, ".otf"))
    {
        int file_size = RF_FILE_SIZE(io, filename);
        void* data = RF_ALLOC(temp_allocator, file_size);

        if (RF_READ_FILE(io, filename, data, file_size))
        {
            font = rf_load_ttf_font_from_data(data, RF_DEFAULT_FONT_SIZE, antialias, (int[]) RF_BUILTIN_FONT_CHARS, RF_BUILTIN_CODEPOINTS_COUNT, allocator, temp_allocator);

            // By default we set point filter (best performance)
            rf_set_texture_filter(font.texture, RF_FILTER_POINT);
        }

        RF_FREE(temp_allocator, data);
    }

    return font;
}

#pragma endregion

#pragma region image font

RF_API bool rf_compute_glyph_metrics_from_image(rf_image image, rf_color key, const int* codepoints, rf_glyph_info* dst, rf_int codepoints_and_dst_count)
{
    bool result = false;

    if (image.valid && codepoints && dst && codepoints_and_dst_count > 0)
    {
        const int bpp = rf_bytes_per_pixel(image.format);
        const int image_data_size = rf_image_size(image);

        // This macro uses `bpp` and returns the pixel from the image at the index provided by calling rf_format_one_pixel_to_rgba32.
        #define RF_GET_PIXEL(index) rf_format_one_pixel_to_rgba32((char*)image.data + ((index) * bpp), image.format)

        // Parse image data to get char_spacing and line_spacing
        int char_spacing = 0;
        int line_spacing = 0;
        {
            int x = 0;
            int y = 0;
            for (y = 0; y < image.height; y++)
            {
                rf_color pixel = {0};
                for (x = 0; x < image.width; x++)
                {
                    pixel = RF_GET_PIXEL(y * image.width + x);
                    if (!rf_color_match(pixel, key)) break;
                }

                if (!rf_color_match(pixel, key)) break;
            }
            char_spacing = x;
            line_spacing = y;
        }

        // Compute char height
        int char_height = 0;
        {
            while (!rf_color_match(RF_GET_PIXEL((line_spacing + char_height) * image.width + char_spacing), key))
            {
                char_height++;
            }
        }

        // Check array values to get characters: value, x, y, w, h
        int index         = 0;
        int line_to_read  = 0;
        int x_pos_to_read = char_spacing;

        // Parse image data to get rectangle sizes
        while ((line_spacing + line_to_read * (char_height + line_spacing)) < image.height && index < codepoints_and_dst_count)
        {
            while (x_pos_to_read < image.width && !rf_color_match(RF_GET_PIXEL((line_spacing + (char_height + line_spacing) * line_to_read) * image.width + x_pos_to_read), key))
            {
                int char_width = 0;
                while (!rf_color_match(RF_GET_PIXEL(((line_spacing + (char_height + line_spacing) * line_to_read) * image.width + x_pos_to_read + char_width)), key)) {
                    char_width++;
                }

                dst[index].codepoint = codepoints[index];
                dst[index].x         = (float) x_pos_to_read;
                dst[index].y         = (float) (line_spacing + line_to_read * (char_height + line_spacing));
                dst[index].width     = (float) char_width;
                dst[index].height    = (float) char_height;

                // On image based fonts (XNA style), character offsets and x_advance are not required (set to 0)
                dst[index].offset_x  = 0;
                dst[index].offset_y  = 0;
                dst[index].advance_x = 0;

                index++;

                x_pos_to_read += (char_width + char_spacing);
            }

            line_to_read++;
            x_pos_to_read = char_spacing;
        }

        result = true;

        #undef RF_GET_PIXEL
    }

    return result;
}

RF_API rf_font rf_load_image_font_from_data(rf_image image, rf_glyph_info* glyphs, rf_int glyphs_count)
{
    rf_font result = {
        .texture      = rf_load_texture_from_image(image),
        .glyphs       = glyphs,
        .glyphs_count = glyphs_count,
    };

    if (image.valid && glyphs && glyphs_count > 0)
    {
        result.base_size = glyphs[0].height;
        result.valid     = true;
    }

    return result;
}

RF_API rf_font rf_load_image_font(rf_image image, rf_color key, rf_allocator allocator)
{
    rf_font result = {0};

    if (image.valid)
    {
        const int codepoints[]     = RF_BUILTIN_FONT_CHARS;
        const int codepoints_count = RF_BUILTIN_CODEPOINTS_COUNT;

        rf_glyph_info* glyphs = RF_ALLOC(allocator, codepoints_count * sizeof(rf_glyph_info));

        rf_compute_glyph_metrics_from_image(image, key, codepoints, glyphs, codepoints_count);

        result = rf_load_image_font_from_data(image, glyphs, codepoints_count);
    }

    return result;
}

RF_API rf_font rf_load_image_font_from_file(const char* path, rf_color key, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_font result = {0};

    rf_image image = rf_load_image_from_file(path, temp_allocator, temp_allocator, io);

    result = rf_load_image_font(image, key, allocator);

    rf_unload_image(image, temp_allocator);

    return result;
}

#pragma endregion

// Unload rf_font from GPU memory (VRAM)
RF_API void rf_unload_font(rf_font font, rf_allocator allocator)
{
    if (font.valid)
    {
        rf_unload_texture(font.texture);
        RF_FREE(allocator, font.glyphs);
    }
}

// Returns index position for a unicode character on spritefont
RF_API rf_glyph_index rf_get_glyph_index(rf_font font, int character)
{
    rf_glyph_index result = RF_GLYPH_NOT_FOUND;

    for (rf_int i = 0; i < font.glyphs_count; i++)
    {
        if (font.glyphs[i].codepoint == character)
        {
            result = i;
            break;
        }
    }

    return result;
}

RF_API int rf_font_height(rf_font font, float font_size)
{
    float scale_factor = font_size / font.base_size;
    return (font.base_size + font.base_size / 2) * scale_factor;
}

RF_API rf_sizef rf_measure_text(rf_font font, const char* text, float font_size, float extra_spacing)
{
    rf_sizef result = rf_measure_string(font, text, strlen(text), font_size, extra_spacing);
    return result;
}

RF_API rf_sizef rf_measure_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float extra_spacing, bool wrap)
{
    rf_sizef result = rf_measure_string_rec(font, text, strlen(text), rec, font_size, extra_spacing, wrap);
    return result;
}

RF_API rf_sizef rf_measure_string(rf_font font, const char* text, int len, float font_size, float extra_spacing)
{
    rf_sizef result = {0};

    if (font.valid)
    {
        int temp_len = 0; // Used to count longer text line num chars
        int len_counter = 0;

        float text_width = 0.0f;
        float temp_text_width = 0.0f; // Used to count longer text line width

        float text_height  = (float)font.base_size;
        float scale_factor = font_size/(float)font.base_size;

        int letter = 0; // Current character
        int index  = 0; // Index position in sprite font

        for (rf_int i = 0; i < len; i++)
        {
            len_counter++;

            rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], len - i);
            index = rf_get_glyph_index(font, decoded_rune.codepoint);

            // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
            // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
            if (letter == 0x3f) { decoded_rune.bytes_processed = 1; }
            i += decoded_rune.bytes_processed - 1;

            if (letter != '\n')
            {
                if (font.glyphs[index].advance_x != 0) { text_width += font.glyphs[index].advance_x; }
                else { text_width += (font.glyphs[index].width + font.glyphs[index].offset_x); }
            }
            else
            {
                if (temp_text_width < text_width) { temp_text_width = text_width; }

                len_counter = 0;
                text_width = 0;
                text_height += ((float)font.base_size*1.5f); // NOTE: Fixed line spacing of 1.5 lines
            }

            if (temp_len < len_counter) { temp_len = len_counter; }
        }

        if (temp_text_width < text_width) temp_text_width = text_width;

        result.width  = temp_text_width * scale_factor + (float)((temp_len - 1) * extra_spacing); // Adds chars spacing to measure
        result.height = text_height * scale_factor;
    }

    return result;
}

RF_API rf_sizef rf_measure_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float extra_spacing, bool wrap)
{
    rf_sizef result = {0};

    if (font.valid)
    {
        int text_offset_x = 0; // Offset between characters
        int text_offset_y = 0; // Required for line break!
        float scale_factor = 0.0f;

        int letter = 0; // Current character
        int index = 0; // Index position in sprite font

        scale_factor = font_size / font.base_size;

        enum
        {
            MEASURE_WRAP_STATE = 0,
            MEASURE_REGULAR_STATE = 1
        };

        int state = wrap ? MEASURE_WRAP_STATE : MEASURE_REGULAR_STATE;
        int start_line = -1; // Index where to begin drawing (where a line begins)
        int end_line = -1; // Index where to stop drawing (where a line ends)
        int lastk = -1; // Holds last value of the character position

        int max_y   = 0;
        int first_y = 0;
        bool first_y_set = false;

        for (rf_int i = 0, k = 0; i < text_len; i++, k++)
        {
            int glyph_width = 0;

            rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], text_len - i);
            letter = decoded_rune.codepoint;
            index = rf_get_glyph_index(font, letter);

            // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
            // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
            if (letter == 0x3f) decoded_rune.bytes_processed = 1;
            i += decoded_rune.bytes_processed - 1;

            if (letter != '\n')
            {
                glyph_width = (font.glyphs[index].advance_x == 0) ?
                              (int)(font.glyphs[index].width * scale_factor + extra_spacing) :
                              (int)(font.glyphs[index].advance_x * scale_factor + extra_spacing);
            }

            // NOTE: When word_wrap is ON we first measure how much of the text we can draw before going outside of the rec container
            // We store this info in start_line and end_line, then we change states, draw the text between those two variables
            // and change states again and again recursively until the end of the text (or until we get outside of the container).
            // When word_wrap is OFF we don't need the measure state so we go to the drawing state immediately
            // and begin drawing on the next line before we can get outside the container.
            if (state == MEASURE_WRAP_STATE)
            {
                // TODO: there are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
                // See: http://jkorpela.fi/chars/spaces.html
                if ((letter == ' ') || (letter == '\t') || (letter == '\n')) { end_line = i; }

                if ((text_offset_x + glyph_width + 1) >= rec.width)
                {
                    end_line = (end_line < 1) ? i : end_line;
                    if (i == end_line) { end_line -= decoded_rune.bytes_processed; }
                    if ((start_line + decoded_rune.bytes_processed) == end_line) { end_line = i - decoded_rune.bytes_processed; }
                    state = !state;
                }
                else if ((i + 1) == text_len)
                {
                    end_line = i;
                    state = !state;
                }
                else if (letter == '\n')
                {
                    state = !state;
                }

                if (state == MEASURE_REGULAR_STATE)
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
                if (letter == '\n')
                {
                    if (!wrap)
                    {
                        text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                        text_offset_x = 0;
                    }
                }
                else
                {
                    if (!wrap && (text_offset_x + glyph_width + 1) >= rec.width)
                    {
                        text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                        text_offset_x = 0;
                    }

                    if ((text_offset_y + (int)(font.base_size*scale_factor)) > rec.height) break;

                    // The right side expression is the offset of the latest character plus its width (so the end of the line)
                    // We want the highest value of that expression by the end of the function
                    result.width  = rf_max_f(result.width,  rec.x + text_offset_x - 1 + glyph_width);

                    if (!first_y_set)
                    {
                        first_y = rec.y + text_offset_y;
                        first_y_set = true;
                    }

                    max_y = rf_max_i(max_y, rec.y + text_offset_y + font.base_size * scale_factor);
                }

                if (wrap && i == end_line)
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

        result.height = max_y - first_y;
    }

    return result;
}