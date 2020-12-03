#ifndef RAYFORK_FONT_H
#define RAYFORK_FONT_H

#include "rayfork-low-level-renderer.h"

#define RF_SDF_CHAR_PADDING         (4)
#define RF_SDF_ON_EDGE_VALUE        (128)
#define RF_SDF_PIXEL_DIST_SCALE     (64.0f)

#define RF_BITMAP_ALPHA_THRESHOLD   (80)
#define RF_DEFAULT_FONT_SIZE        (64)

#define RF_BUILTIN_FONT_CHARS       { ' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~', }
#define RF_BUILTIN_FONT_FIRST_CHAR  (32)
#define RF_BUILTIN_FONT_LAST_CHAR   (126)
#define RF_BUILTIN_CODEPOINTS_COUNT (96) // ASCII 32 up to 126 is 96 glyphs (note that the range is inclusive)
#define RF_BUILTIN_FONT_PADDING     (2)

#define RF_GLYPH_NOT_FOUND          (-1)

#define RF_BUILTIN_FONT_CHARS_COUNT (224) // Number of characters in the raylib font

typedef enum rf_font_antialias
{
    RF_FONT_ANTIALIAS = 0, // Default font generation, anti-aliased
    RF_FONT_NO_ANTIALIAS,  // Bitmap font generation, no anti-aliasing
} rf_font_antialias;

typedef struct rf_glyph_info
{
    rf_rec rec;      // Characters rectangles in texture
    int codepoint;   // Character value (Unicode)
    int offset_x;    // Character offset X when drawing
    int offset_y;    // Character offset Y when drawing
    int advance_x;   // Character advance position X
} rf_glyph_info;

typedef struct rf_ttf_font_info
{
    // Font details
    const void* ttf_data;
    int         font_size;
    int         largest_glyph_size;

    // Font metrics
    float scale_factor;
    int   ascent;
    int   descent;
    int   line_gap;

    // Take directly from stb_truetype because we don't want to include it's header in our public API
    struct
    {
        void* userdata;
        unsigned char* data;
        int fontstart;
        int numGlyphs;
        int loca, head, glyf, hhea, hmtx, kern, gpos, svg;
        int index_map;
        int indexToLocFormat;

        struct
        {
            unsigned char* data;
            int cursor;
            int size;
        } cff, charstrings, gsubrs, subrs, fontdicts, fdselect;
    } internal_stb_font_info;

    rf_bool valid;
} rf_ttf_font_info;

typedef struct rf_font
{
    int            base_size;
    rf_texture2d   texture;
    rf_glyph_info* glyphs;
    rf_int         glyphs_count;
    rf_bool        valid;
} rf_font;

typedef int rf_glyph_index;

#pragma region ttf font
rf_public rf_ttf_font_info rf_parse_ttf_font(const void* ttf_data, rf_int font_size);
rf_public void rf_compute_ttf_font_glyph_metrics(rf_ttf_font_info* font_info, const int* codepoints, rf_int codepoints_count, rf_glyph_info* dst, rf_int dst_count);
rf_public int rf_compute_ttf_font_atlas_width(int padding, rf_glyph_info* glyph_metrics, rf_int glyphs_count);
rf_public rf_image rf_generate_ttf_font_atlas(rf_ttf_font_info* font_info, int atlas_width, int padding, rf_glyph_info* glyphs, rf_int glyphs_count, rf_font_antialias antialias, unsigned short* dst, rf_int dst_count, rf_allocator temp_allocator);
rf_public rf_font rf_ttf_font_from_atlas(int font_size, rf_image atlas, rf_glyph_info* glyph_metrics, rf_int glyphs_count);

rf_public rf_font rf_load_ttf_font_from_data(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, rf_int char_count, rf_allocator allocator, rf_allocator temp_allocator);
rf_public rf_font rf_load_ttf_font_from_file(const char* filename, int font_size, rf_font_antialias antialias, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region image font
rf_public rf_bool rf_compute_glyph_metrics_from_image(rf_image image, rf_color key, const int* codepoints, rf_glyph_info* dst, rf_int codepoints_and_dst_count);
rf_public rf_font rf_load_image_font_from_data(rf_image image, rf_glyph_info* glyphs, rf_int glyphs_count);
rf_public rf_font rf_load_image_font(rf_image image, rf_color key, rf_allocator allocator);
rf_public rf_font rf_load_image_font_from_file(const char* path, rf_color key, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region font utils
rf_public void rf_unload_font(rf_font font, rf_allocator allocator);
rf_public rf_glyph_index rf_get_glyph_index(rf_font font, int character);
rf_public int rf_font_height(rf_font font, float font_size);

rf_public rf_sizef rf_measure_text(rf_font font, const char* text, float font_size, float extra_spacing);
rf_public rf_sizef rf_measure_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float extra_spacing, rf_bool wrap);

rf_public rf_sizef rf_measure_string(rf_font font, const char* text, int len, float font_size, float extra_spacing);
rf_public rf_sizef rf_measure_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float extra_spacing, rf_bool wrap);
#pragma endregion

#endif // RAYFORK_FONT_H