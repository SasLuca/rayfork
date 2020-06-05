#ifndef RAYFORK_HPP_HEADER_GUARD
#define RAYFORK_HPP_HEADER_GUARD

#include <array>
#include <type_traits>
#include <cstdint>

#ifdef __glad_h_
#error Macros defined in glad will overwrite opengl proc names in rayforh.h. Include rayfork.h before glad.h to fix this issue.
#endif

namespace rf
{
    enum class graphics_backend
    {
        gl33,
        gles3,
        metal,
        directx,
    };

    constexpr int unlocked_fps = 0;
    constexpr int glyph_not_found = -1;
    constexpr int default_sdf_char_padding = 4;
    constexpr int default_sdf_on_edge_value = 128;
    constexpr float default_sdf_pixel_dist_scale = 64.0f;
    constexpr int default_bitmap_alpha_threshold = 80;
    constexpr int default_font_size = 64;
    constexpr char32_t default_charset_first = ' ';
    constexpr char32_t default_charset_last = '~';
    constexpr int default_codepoints_size = static_cast<int>(default_charset_last - default_charset_first + 1);
    constexpr auto default_codepoints = std::array<char32_t, default_codepoints_size> { ' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~', };
    constexpr char32_t default_codepoint = '?';

    constexpr const char* default_attrib_position_name  = "vertex_position";    // shader-location = 0
    constexpr const char* default_attrib_texcoord_name  = "vertex_tex_coord";   // shader-location = 1
    constexpr const char* default_attrib_normal_name    = "vertex_normal";      // shader-location = 2
    constexpr const char* default_attrib_color_name     = "vertex_color";       // shader-location = 3
    constexpr const char* default_attrib_tangent_name   = "vertex_tangent";     // shader-location = 4
    constexpr const char* default_attrib_texcoord2_name = "vertex_tex_coord2";  // shader-location = 5

    enum class error_type
    {
        bad_argument,
        bad_alloc,
        bad_io,
        bad_buffer_size,
        bad_format,
        stbi_failed,
        stbtt_failed,
        unsupported,
    };

    enum class log_type
    {
        debug,
        info,
        warning,
        error,
    };

    enum class matrix_mode
    {
        modelview  = 0x1700,  // GL_MODELVIEW
        projection = 0x1701,  // GL_PROJECTION
        texture    = 0x1702,  // GL_TEXTURE
    };

    enum class drawing_mode
    {
        lines     = 0x0001, // GL_LINES
        triangles = 0x0004, // GL_TRIANGLES
        quads     = 0x0007, // GL_QUADS
    };

    enum class shader_location_index
    {
        vertex_position = 0,
        vertex_texcoord1 = 1,
        vertex_texcoord2 = 2,
        vertex_normal = 3,
        vertex_tangent = 4,
        vertex_color = 5,
        
        matrix_mvp = 6,
        matrix_model = 7,
        matrix_view = 8,
        matrix_projection = 9,
        
        vector_view = 10,
        
        color_diffuse = 11,
        color_specular = 12,
        color_ambient = 13,
    
        // These 2 are the same
        map_albedo = 14,
        map_diffuse = 14,
    
        // These 2 are the same
        map_metalness = 15,
        map_specular = 15,
    
        map_normal = 16,
        map_roughness = 17,
        map_occlusion = 18,
        map_emission = 19,
        map_height = 20,
        map_cubemap = 21,
        map_irradiance = 22,
        map_prefilter = 23,
        map_brdf = 24,
    };
    
    enum class shader_uniform_type
    {
        floating_point = 0,
        vec2,
        vec3,
        vec4,
        integer,
        ivec2,
        ivec3,
        ivec4,
        sampler2d,
    };
    
    enum class material_map_type
    {
        // These 2 are the same
        map_albedo = 0,
        map_diffuse = 0,
    
        // These 2 are the same
        map_metalness = 1,
        map_specular = 1,
    
        map_normal = 2,
        map_roughness = 3,
        map_occlusion = 4,
        map_emission = 5,
        map_height = 6,
        map_cubemap = 7,     // NOTE: Uses GL_TEXTURE_CUBE_MAP
        map_irradiance = 8,  // NOTE: Uses GL_TEXTURE_CUBE_MAP
        map_prefilter = 9,   // NOTE: Uses GL_TEXTURE_CUBE_MAP
        map_brdf = 10
    };
    
    enum class text_wrap_mode
    {
        char_wrap,
        word_wrap,
    };
    
    enum class pixel_format
    {
        grayscale = 1, // 8 bits per pixel (no alpha)
        gray_alpha, // 8 * 2 bpp (2 channels)
        r5g6b5, // 16 bpp
        r8g8b8, // 24 bpp
        r5g5b5a1, // 16 bpp (1 bit alpha)
        r4g4b4a4, // 16 bpp (4 bit alpha)
        r8g8b8a8, // 32 bpp
        rgba32 = r8g8b8a8, //32 bpp
        r32, // 32 bpp (1 channel - float)
        r32g32b32, // 32 * 3 bpp (3 channels - float)
        r32g32b32a32, // 32 * 4 bpp (4 channels - float)
        normalized = r32g32b32a32, // 32 * 4 bpp (4 channels - float)

        dxt1_rgb, // 4 bpp (no alpha)
        dxt1_rgba, // 4 bpp (1 bit alpha)
        dxt3_rgba, // 8 bpp
        dxt5_rgba, // 8 bpp
        etc1_rgb, // 4 bpp
        etc2_rgb, // 4 bpp
        etc2_eac_rgba, // 8 bpp
        pvrt_rgb, // 4 bpp
        pvrt_rgba, // 4 bpp
        astc_4x4_rgba, // 8 bpp
        astc_8x8_rgba // 2 bpp
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::grayscale> {
        using value_type = uint8_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::gray_alpha> {
        using value_type = uint16_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r5g6b5> {
        using value_type = uint16_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r8g8b8> {
        using value_type = uint32_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r5g5b5a1> {
        using value_type = uint16_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r4g4b4a4> {
        using value_type = uint16_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r8g8b8a8> {
        using value_type = uint32_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r8g8b8a8> {
        using value_type = uint32_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r32> {
        using value_type = uint32_t;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r32g32b32> {
        using value_type = std::array<float, 3>;
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };

    template<pixel_format format> struct pixel_format_details;
    template<> struct pixel_format_details<pixel_format::r32g32b32a32> {
        using value_type = std::array<float, 4>;
        static constexpr int bits_per_pixel = 32 * 4;
        static constexpr int bytes_per_pixel = sizeof(value_type);
        static constexpr bool is_uncompressed = true;
        static constexpr bool is_compressed = false;
    };
}

#endif // RAYFORK_HPP_HEADER_GUARD