#include "rayfork/model/materials.h"

#include "rayfork/gfx/gfx.h"
#include "rayfork/gfx/context.h"
#include "tinyobj_loader_c/tinyobj_loader_c.h"

// Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
rf_extern rf_material rf_load_default_material(rf_allocator allocator)
{
    rf_material material = {0};
    material.maps = (rf_material_map*) rf_alloc(allocator, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));
    memset(material.maps, 0, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));

    material.shader = rf_get_default_shader();
    material.maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // White texture (1x1 pixel)
    //material.maps[RF_MAP_NORMAL].texture;         // NOTE: By default, not set
    //material.maps[RF_MAP_SPECULAR].texture;       // NOTE: By default, not set

    material.maps[RF_MAP_DIFFUSE].color = rf_white; // Diffuse color
    material.maps[RF_MAP_SPECULAR].color = rf_white; // Specular color

    return material;
}

rf_extern void rf_material_free(rf_material material, rf_allocator allocator)
{
    // Unload material shader (avoid unloading default shader, managed by raylib)
    if (material.shader.id != rf_get_default_shader().id)
    {
        rf_gfx_unload_shader(material.shader);
    }

    // Unload loaded texture maps (avoid unloading default texture, managed by raylib)
    for (rf_int i = 0; i < RF_MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id != rf_get_default_texture().id)
        {
            rf_gfx_delete_textures(material.maps[i].texture.id);
        }
    }

    rf_free(allocator, material.maps);
}

rf_extern void rf_material_set_texture(rf_material* material, rf_material_map_type map_type, rf_texture2d texture); // Set texture for a material map type (rf_map_diffuse, rf_map_specular...)