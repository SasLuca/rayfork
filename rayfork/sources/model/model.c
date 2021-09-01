#include "rayfork/gfx/context.h"
#include "rayfork/math/base64.h"
#include "rayfork/internal/thirdparty/cgltf-utils.h"
#include "rayfork/internal/foundation/assert.h"
#include "rayfork/foundation/strbuf.h"
#include "rayfork/math/collision-detection.h"
#include "rayfork/model/model.h"
#include "rayfork/math/maths.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/model/mesh-gen.h"
#include "rayfork/gfx/gfx.h"
#include "rayfork/model/materials.h"
#include "rayfork/internal/thirdparty/tinyobj-utils.h"
#include "rayfork/image/image-manipulation.h"
#include "rayfork/gfx/texture.h"
#include "rayfork/image/image-loading.h"
#include "tinyobj_loader_c/tinyobj_loader_c.h"
#include "cgltf/cgltf.h"

rf_internal rf_model rf_load_meshes_and_materials_for_model(rf_model model, rf_allocator allocator, rf_allocator temp_allocator)
{
    // Make sure model transform is set to identity matrix!
    model.transform = rf_mat_identity();

    if (model.mesh_count == 0)
    {
        rf_log(rf_log_type_warning, "No meshes can be loaded, default to cube mesh.");

        model.mesh_count = 1;
        model.meshes = rf_alloc(allocator, sizeof(rf_mesh));
        memset(model.meshes, 0, sizeof(rf_mesh));
        model.meshes[0] = rf_gen_mesh_cube(1.0f, 1.0f, 1.0f, allocator, temp_allocator);
    }
    else
    {
        // Upload vertex data to GPU (static mesh)
        for (rf_int i = 0; i < model.mesh_count; i++)
        {
            rf_gfx_load_mesh(&model.meshes[i], false);
        }
    }

    if (model.material_count == 0)
    {
        rf_log(rf_log_type_warning, "No materials can be loaded, default to white material.");

        model.material_count = 1;
        model.materials = rf_alloc(allocator, sizeof(rf_material));
        memset(model.materials, 0, sizeof(rf_material));
        model.materials[0] = rf_load_default_material(allocator);

        if (model.mesh_material == NULL)
        {
            model.mesh_material = rf_alloc(allocator, model.mesh_count * sizeof(int));
            memset(model.mesh_material, 0, model.mesh_count * sizeof(int));
        }
    }

    return model;
}

// Compute mesh bounding box limits. Note: min_vertex and max_vertex should be transformed by model transform matrix
rf_extern rf_bounding_box rf_mesh_bounding_box(rf_mesh mesh)
{
    // Get min and max vertex to construct bounds (AABB)
    rf_vec3 min_vertex = {0};
    rf_vec3 max_vertex = {0};

    if (mesh.vertices != NULL)
    {
        min_vertex = (rf_vec3){mesh.vertices[0], mesh.vertices[1], mesh.vertices[2] };
        max_vertex = (rf_vec3){mesh.vertices[0], mesh.vertices[1], mesh.vertices[2] };

        for (rf_int i = 1; i < mesh.vertex_count; i++)
        {
            min_vertex = rf_vec3_min(min_vertex, (rf_vec3) {mesh.vertices[i * 3], mesh.vertices[i * 3 + 1], mesh.vertices[i * 3 + 2]});
            max_vertex = rf_vec3_max(max_vertex, (rf_vec3) {mesh.vertices[i * 3], mesh.vertices[i * 3 + 1], mesh.vertices[i * 3 + 2]});
        }
    }

    // Create the bounding box
    rf_bounding_box box = {0};
    box.min = min_vertex;
    box.max = max_vertex;

    return box;
}

// Compute mesh tangents
// NOTE: To calculate mesh tangents and binormals we need mesh vertex positions and texture coordinates
// Implementation base don: https://answers.unity.com/questions/7789/calculating-tangents-vector4.html
rf_extern void rf_mesh_compute_tangents(rf_mesh* mesh, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (mesh->tangents == NULL)
    {
        mesh->tangents = (float*) rf_alloc(allocator, mesh->vertex_count * 4 * sizeof(float));
    }
    else
    {
        rf_log(rf_log_type_warning, "rf_mesh tangents already exist");
    }

    rf_vec3* tan1 = rf_alloc(temp_allocator, mesh->vertex_count * sizeof(rf_vec3));
    rf_vec3* tan2 = rf_alloc(temp_allocator, mesh->vertex_count * sizeof(rf_vec3));

    for (rf_int i = 0; i < mesh->vertex_count; i += 3)
    {
        // Get triangle vertices
        rf_vec3 v1 = { mesh->vertices[(i + 0) * 3 + 0], mesh->vertices[(i + 0) * 3 + 1], mesh->vertices[(i + 0) * 3 + 2] };
        rf_vec3 v2 = { mesh->vertices[(i + 1) * 3 + 0], mesh->vertices[(i + 1) * 3 + 1], mesh->vertices[(i + 1) * 3 + 2] };
        rf_vec3 v3 = { mesh->vertices[(i + 2) * 3 + 0], mesh->vertices[(i + 2) * 3 + 1], mesh->vertices[(i + 2) * 3 + 2] };

        // Get triangle texcoords
        rf_vec2 uv1 = { mesh->texcoords[(i + 0) * 2 + 0], mesh->texcoords[(i + 0) * 2 + 1] };
        rf_vec2 uv2 = { mesh->texcoords[(i + 1) * 2 + 0], mesh->texcoords[(i + 1) * 2 + 1] };
        rf_vec2 uv3 = { mesh->texcoords[(i + 2) * 2 + 0], mesh->texcoords[(i + 2) * 2 + 1] };

        float x1 = v2.x - v1.x;
        float y1 = v2.y - v1.y;
        float z1 = v2.z - v1.z;
        float x2 = v3.x - v1.x;
        float y2 = v3.y - v1.y;
        float z2 = v3.z - v1.z;

        float s1 = uv2.x - uv1.x;
        float t1 = uv2.y - uv1.y;
        float s2 = uv3.x - uv1.x;
        float t2 = uv3.y - uv1.y;

        float div = s1 * t2 - s2 * t1;
        float r = (div == 0.0f) ? (0.0f) : (1.0f / div);

        rf_vec3 sdir = {(t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
        rf_vec3 tdir = {(s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

        tan1[i + 0] = sdir;
        tan1[i + 1] = sdir;
        tan1[i + 2] = sdir;

        tan2[i + 0] = tdir;
        tan2[i + 1] = tdir;
        tan2[i + 2] = tdir;
    }

    // Compute tangents considering normals
    for (rf_int i = 0; i < mesh->vertex_count; ++i)
    {
        rf_vec3 normal = { mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2] };
        rf_vec3 tangent = tan1[i];

        // TODO: Review, not sure if tangent computation is right, just used reference proposed maths...
        rf_vec3_ortho_normalize(&normal, &tangent);
        mesh->tangents[i * 4 + 0] = tangent.x;
        mesh->tangents[i * 4 + 1] = tangent.y;
        mesh->tangents[i * 4 + 2] = tangent.z;
        mesh->tangents[i * 4 + 3] = (rf_vec3_dot_product(rf_vec3_cross_product(normal, tangent), tan2[i]) < 0.0f) ? -1.0f : 1.0f;
    }

    rf_free(temp_allocator, tan1);
    rf_free(temp_allocator, tan2);

    // Load a new tangent attributes buffer
    mesh->vbo_id[RF_LOC_VERTEX_TANGENT] = rf_gfx_load_attrib_buffer(mesh->vao_id, RF_LOC_VERTEX_TANGENT, mesh->tangents, mesh->vertex_count * 4 * sizeof(float), false);

    rf_log(rf_log_type_info, "Tangents computed for mesh");
}

// Compute mesh binormals (aka bitangent)
rf_extern void rf_mesh_compute_binormals(rf_mesh* mesh)
{
    for (rf_int i = 0; i < mesh->vertex_count; i++)
    {
        rf_vec3 normal = {mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2] };
        rf_vec3 tangent = {mesh->tangents[i * 4 + 0], mesh->tangents[i * 4 + 1], mesh->tangents[i * 4 + 2] };
        float tangent_w = mesh->tangents[i * 4 + 3];

        // TODO: Register computed binormal in mesh->binormal?
        // rf_vec3 binormal = rf_vec3_mul(rf_vec3_cross_product(normal, tangent), tangent_w);
    }
}

// Unload mesh from memory (RAM and/or VRAM)
rf_extern void rf_unload_mesh(rf_mesh mesh, rf_allocator allocator)
{
    rf_gfx_unload_mesh(mesh);

    rf_free(allocator, mesh.vertices);
    rf_free(allocator, mesh.texcoords);
    rf_free(allocator, mesh.normals);
    rf_free(allocator, mesh.colors);
    rf_free(allocator, mesh.tangents);
    rf_free(allocator, mesh.texcoords2);
    rf_free(allocator, mesh.indices);

    rf_free(allocator, mesh.anim_vertices);
    rf_free(allocator, mesh.anim_normals);
    rf_free(allocator, mesh.bone_weights);
    rf_free(allocator, mesh.bone_ids);
    rf_free(allocator, mesh.vbo_id);
}

rf_extern rf_model rf_load_model(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_model model = {0};

    if (rf_str_match_suffix(rf_cstr(filename), rf_cstr(".obj")))
    {
        model = rf_load_model_from_obj(filename, allocator, temp_allocator, io);
    }

    if (rf_str_match_suffix(rf_cstr(filename), rf_cstr(".iqm")))
    {
        model = rf_load_model_from_iqm(filename, allocator, temp_allocator, io);
    }

    if (rf_str_match_suffix(rf_cstr(filename), rf_cstr(".gltf")) || rf_str_match_suffix(rf_cstr(filename), rf_cstr(".glb")))
    {
        model = rf_load_model_from_gltf(filename, allocator, temp_allocator, io);
    }

    // Make sure model transform is set to identity matrix!
    model.transform = rf_mat_identity();
    allocator = allocator;

    if (model.mesh_count == 0)
    {
        rf_log(rf_log_type_warning, "No meshes can be loaded, default to cube mesh. Filename: %s", filename);

        model.mesh_count = 1;
        model.meshes = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
        memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
        model.meshes[0] = rf_gen_mesh_cube(1.0f, 1.0f, 1.0f, allocator, temp_allocator);
    }
    else
    {
        // Upload vertex data to GPU (static mesh)
        for (rf_int i = 0; i < model.mesh_count; i++)
        {
            rf_gfx_load_mesh(&model.meshes[i], false);
        }
    }

    if (model.material_count == 0)
    {
        rf_log(rf_log_type_warning, "No materials can be loaded, default to white material. Filename: %s", filename);

        model.material_count = 1;
        model.materials = (rf_material*) rf_alloc(allocator, model.material_count * sizeof(rf_material));
        memset(model.materials, 0, model.material_count * sizeof(rf_material));
        model.materials[0] = rf_load_default_material(allocator);

        if (model.mesh_material == NULL)
        {
            model.mesh_material = (int*) rf_alloc(allocator, model.mesh_count * sizeof(int));
        }
    }

    return model;
}

// Load OBJ mesh data. Note: This calls into a library to do io, so we need to ask the user for IO callbacks
rf_extern rf_model rf_load_model_from_obj(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_model model  = {0};
    allocator = allocator;

    tinyobj_attrib_t attrib     = {0};
    tinyobj_shape_t* meshes     = NULL;
    size_t           mesh_count = 0;

    tinyobj_material_t* materials      = NULL;
    size_t              material_count = 0;

    rf_global_allocator_for_thirdparty_libraries = (temp_allocator); // Set to NULL at the end of the function
    rf_global_io_for_thirdparty_libraries = *io;
    {
        unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
        int ret            = tinyobj_parse_obj(&attrib, &meshes, (size_t*) &mesh_count, &materials, &material_count, filename, rf_tinyobj_file_reader_callback, flags);

        if (ret != TINYOBJ_SUCCESS)
        {
            rf_log(rf_log_type_warning, "Model data could not be loaded. Filename %s", filename);
        }
        else
        {
            rf_log(rf_log_type_info, "Model data loaded successfully: %i meshes / %i materials, filename: %s", mesh_count, material_count, filename);
        }

        // Init model meshes array
        {
            // TODO: Support multiple meshes... in the meantime, only one mesh is returned
            //model.mesh_count = mesh_count;
            model.mesh_count = 1;
            model.meshes     = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
            memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
        }

        // Init model materials array
        if (material_count > 0)
        {
            model.material_count = material_count;
            model.materials      = (rf_material*) rf_alloc(allocator, model.material_count * sizeof(rf_material));
            memset(model.materials, 0, model.material_count * sizeof(rf_material));
        }

        model.mesh_material = (int*) rf_alloc(allocator, model.mesh_count * sizeof(int));
        memset(model.mesh_material, 0, model.mesh_count * sizeof(int));

        // Init model meshes
        for (rf_int m = 0; m < 1; m++)
        {
            rf_mesh mesh = (rf_mesh)
            {
                .vertex_count   = attrib.num_faces * 3,
                .triangle_count = attrib.num_faces,

                .vertices  = (float*)        rf_alloc(allocator, (attrib.num_faces * 3) * 3 * sizeof(float)),
                .texcoords = (float*)        rf_alloc(allocator, (attrib.num_faces * 3) * 2 * sizeof(float)),
                .normals   = (float*)        rf_alloc(allocator, (attrib.num_faces * 3) * 3 * sizeof(float)),
                .vbo_id    = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int)),
            };

            memset(mesh.vertices,  0, mesh.vertex_count * 3 * sizeof(float));
            memset(mesh.texcoords, 0, mesh.vertex_count * 2 * sizeof(float));
            memset(mesh.normals,   0, mesh.vertex_count * 3 * sizeof(float));
            memset(mesh.vbo_id,    0, RF_MAX_MESH_VBO       * sizeof(unsigned int));

            int vCount  = 0;
            int vtCount = 0;
            int vnCount = 0;

            for (rf_int f = 0; f < attrib.num_faces; f++)
            {
                // Get indices for the face
                tinyobj_vertex_index_t idx0 = attrib.faces[3 * f + 0];
                tinyobj_vertex_index_t idx1 = attrib.faces[3 * f + 1];
                tinyobj_vertex_index_t idx2 = attrib.faces[3 * f + 2];

                // RF_LOG(RF_LOG_TYPE_DEBUG, "Face %i index: v %i/%i/%i . vt %i/%i/%i . vn %i/%i/%i\n", f, idx0.v_idx, idx1.v_idx, idx2.v_idx, idx0.vt_idx, idx1.vt_idx, idx2.vt_idx, idx0.vn_idx, idx1.vn_idx, idx2.vn_idx);

                // Fill vertices buffer (float) using vertex index of the face
                for (rf_int v = 0; v < 3; v++) { mesh.vertices[vCount + v] = attrib.vertices[idx0.v_idx * 3 + v]; }
                vCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.vertices[vCount + v] = attrib.vertices[idx1.v_idx * 3 + v]; }
                vCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.vertices[vCount + v] = attrib.vertices[idx2.v_idx * 3 + v]; }
                vCount +=3;

                // Fill texcoords buffer (float) using vertex index of the face
                // NOTE: Y-coordinate must be flipped upside-down
                mesh.texcoords[vtCount + 0] = attrib.texcoords[idx0.vt_idx * 2 + 0];
                mesh.texcoords[vtCount + 1] = 1.0f - attrib.texcoords[idx0.vt_idx * 2 + 1]; vtCount += 2;
                mesh.texcoords[vtCount + 0] = attrib.texcoords[idx1.vt_idx * 2 + 0];
                mesh.texcoords[vtCount + 1] = 1.0f - attrib.texcoords[idx1.vt_idx * 2 + 1]; vtCount += 2;
                mesh.texcoords[vtCount + 0] = attrib.texcoords[idx2.vt_idx * 2 + 0];
                mesh.texcoords[vtCount + 1] = 1.0f - attrib.texcoords[idx2.vt_idx * 2 + 1]; vtCount += 2;

                // Fill normals buffer (float) using vertex index of the face
                for (rf_int v = 0; v < 3; v++) { mesh.normals[vnCount + v] = attrib.normals[idx0.vn_idx * 3 + v]; }
                vnCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.normals[vnCount + v] = attrib.normals[idx1.vn_idx * 3 + v]; }
                vnCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.normals[vnCount + v] = attrib.normals[idx2.vn_idx * 3 + v]; }
                vnCount +=3;
            }

            model.meshes[m] = mesh; // Assign mesh data to model

            // Assign mesh material for current mesh
            model.mesh_material[m] = attrib.material_ids[m];

            // Set unfound materials to default
            if (model.mesh_material[m] == -1) { model.mesh_material[m] = 0; }
        }

        // Init model materials
        for (rf_int m = 0; m < material_count; m++)
        {
            // Init material to default
            // NOTE: Uses default shader, only RF_MAP_DIFFUSE supported
            model.materials[m] = rf_load_default_material(allocator);
            model.materials[m].maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // Get default texture, in case no texture is defined

            if (materials[m].diffuse_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_DIFFUSE].texture = rf_load_texture_from_file(materials[m].diffuse_texname, temp_allocator, io); //char* diffuse_texname; // map_Kd
            }

            model.materials[m].maps[RF_MAP_DIFFUSE].color = (rf_color)
            {
                (float)(materials[m].diffuse[0] * 255.0f),
                (float)(materials[m].diffuse[1] * 255.0f),
                (float)(materials[m].diffuse[2] * 255.0f),
                255
            };

            model.materials[m].maps[RF_MAP_DIFFUSE].value = 0.0f;

            if (materials[m].specular_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_SPECULAR].texture = rf_load_texture_from_file(materials[m].specular_texname, temp_allocator, io); //char* specular_texname; // map_Ks
            }

            model.materials[m].maps[RF_MAP_SPECULAR].color = (rf_color)
            {
                (float)(materials[m].specular[0] * 255.0f),
                (float)(materials[m].specular[1] * 255.0f),
                (float)(materials[m].specular[2] * 255.0f),
                255
            };

            model.materials[m].maps[RF_MAP_SPECULAR].value = 0.0f;

            if (materials[m].bump_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_NORMAL].texture = rf_load_texture_from_file(materials[m].bump_texname, temp_allocator, io); //char* bump_texname; // map_bump, bump
            }

            model.materials[m].maps[RF_MAP_NORMAL].color = rf_white;
            model.materials[m].maps[RF_MAP_NORMAL].value = materials[m].shininess;

            model.materials[m].maps[RF_MAP_EMISSION].color = (rf_color)
            {
                (float)(materials[m].emission[0] * 255.0f),
                (float)(materials[m].emission[1] * 255.0f),
                (float)(materials[m].emission[2] * 255.0f),
                255
            };

            if (materials[m].displacement_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_HEIGHT].texture = rf_load_texture_from_file(materials[m].displacement_texname, temp_allocator, io); //char* displacement_texname; // disp
            }
        }

        tinyobj_attrib_free(&attrib);
        tinyobj_shapes_free(meshes, mesh_count);
        tinyobj_materials_free(materials, material_count);
    }
    rf_global_allocator_for_thirdparty_libraries = ((rf_allocator) {0});
    rf_global_io_for_thirdparty_libraries = ((rf_io_callbacks){0});

    // NOTE: At this point we have all model data loaded
    rf_log(rf_log_type_info, "Model loaded successfully in RAM. Filename: %s", filename);

    return rf_load_meshes_and_materials_for_model(model, allocator, temp_allocator);
}

// Load IQM mesh data
rf_extern rf_model rf_load_model_from_iqm(const char* path_to_file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    #pragma region constants
    #define rf_iqm_magic "INTERQUAKEMODEL" // IQM file magic number
    #define rf_iqm_version 2 // only IQM version 2 supported

    #define rf_bone_name_len 32 // rf_bone_info name string length
    #define rf_mesh_name_len 32 // rf_mesh name string length
    #pragma endregion

    #pragma region IQM file structs
    typedef struct rf_iqm_header
    {
        char         magic[16];
        unsigned int version;
        unsigned int filesize;
        unsigned int flags;
        unsigned int num_text, ofs_text;
        unsigned int num_meshes, ofs_meshes;
        unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
        unsigned int num_triangles, ofs_triangles, ofs_adjacency;
        unsigned int num_joints, ofs_joints;
        unsigned int num_poses, ofs_poses;
        unsigned int num_anims, ofs_anims;
        unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
        unsigned int num_comment, ofs_comment;
        unsigned int num_extensions, ofs_extensions;
    } rf_iqm_header;

    typedef struct rf_iqm_mesh
    {
        unsigned int name;
        unsigned int material;
        unsigned int first_vertex, num_vertexes;
        unsigned int first_triangle, num_triangles;
    } rf_iqm_mesh;

    typedef struct rf_iqm_triangle
    {
        unsigned int vertex[3];
    } rf_iqm_triangle;

    typedef struct rf_iqm_joint
    {
        unsigned int name;
        int          parent;
        float        translate[3], rotate[4], scale[3];
    } rf_iqm_joint;

    typedef struct rf_iqm_vertex_array
    {
        unsigned int type;
        unsigned int flags;
        unsigned int format;
        unsigned int size;
        unsigned int offset;
    } rf_iqm_vertex_array;

    // IQM vertex data types
    typedef enum rf_iqm_vertex_type
    {
        RF_IQM_POSITION     = 0,
        RF_IQM_TEXCOORD     = 1,
        RF_IQM_NORMAL       = 2,
        RF_IQM_TANGENT      = 3,   // Note: Tangents unused by default
        RF_IQM_BLENDINDEXES = 4,
        RF_IQM_BLENDWEIGHTS = 5,
        RF_IQM_COLOR        = 6,   // Note: Vertex colors unused by default
        RF_IQM_CUSTOM       = 0x10 // Note: Custom vertex values unused by default
    }  rf_iqm_vertex_type;
    #pragma endregion

    rf_model model = {0};

    rf_file_contents contents = rf_read_entire_file(path_to_file, temp_allocator, io);
    
    if (contents.valid)
    {
        uint8_t* data = contents.data;
        rf_iqm_header iqm = *((rf_iqm_header*)data);

        rf_iqm_mesh*          imesh;
        rf_iqm_triangle*      tri;
        rf_iqm_vertex_array*  va;
        rf_iqm_joint*         ijoint;
    
        float* vertex         = NULL;
        float* normal         = NULL;
        float* text           = NULL;
        char*  blendi         = NULL;
        unsigned char* blendw = NULL;
    
        if (strncmp(iqm.magic, rf_iqm_magic, sizeof(rf_iqm_magic)))
        {
            rf_log(rf_log_type_warning, "[%s] IQM file does not seem to be valid", path_to_file);
            return model;
        }
    
        if (iqm.version != rf_iqm_version)
        {
            rf_log(rf_log_type_warning, "[%s] IQM file version is not supported (%i).", path_to_file, iqm.version);
            return model;
        }
    
        // Meshes data processing
        imesh = (rf_iqm_mesh*) rf_alloc(temp_allocator, sizeof(rf_iqm_mesh) * iqm.num_meshes);
        memcpy(imesh, data + iqm.ofs_meshes, sizeof(rf_iqm_mesh) * iqm.num_meshes);
    
        model.mesh_count = iqm.num_meshes;
        model.meshes = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
    
        char name[rf_mesh_name_len] = {0};
        for (rf_int i = 0; i < model.mesh_count; i++)
        {
            memcpy(name, data + (iqm.ofs_text + imesh[i].name), rf_mesh_name_len);
    
            model.meshes[i] = (rf_mesh) {
                .vertex_count = imesh[i].num_vertexes
            };
    
            model.meshes[i].vertices = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float)); // Default vertex positions
            memset(model.meshes[i].vertices, 0, model.meshes[i].vertex_count * 3 * sizeof(float));
    
            model.meshes[i].normals = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float)); // Default vertex normals
            memset(model.meshes[i].normals, 0, model.meshes[i].vertex_count * 3 * sizeof(float));
    
            model.meshes[i].texcoords = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 2 * sizeof(float)); // Default vertex texcoords
            memset(model.meshes[i].texcoords, 0, model.meshes[i].vertex_count * 2 * sizeof(float));
    
            model.meshes[i].bone_ids = (int*) rf_alloc(allocator, model.meshes[i].vertex_count * 4 * sizeof(float)); // Up-to 4 bones supported!
            memset(model.meshes[i].bone_ids, 0, model.meshes[i].vertex_count * 4 * sizeof(float));
    
            model.meshes[i].bone_weights = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 4 * sizeof(float)); // Up-to 4 bones supported!
            memset(model.meshes[i].bone_weights, 0, model.meshes[i].vertex_count * 4 * sizeof(float));
    
            model.meshes[i].triangle_count = imesh[i].num_triangles;
    
            model.meshes[i].indices = (unsigned short*) rf_alloc(allocator, model.meshes[i].triangle_count * 3 * sizeof(unsigned short));
            memset(model.meshes[i].indices, 0, model.meshes[i].triangle_count * 3 * sizeof(unsigned short));
    
            // Animated verted data, what we actually process for rendering
            // NOTE: Animated vertex should be re-uploaded to GPU (if not using GPU skinning)
            model.meshes[i].anim_vertices = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float));
            memset(model.meshes[i].anim_vertices, 0, model.meshes[i].vertex_count * 3 * sizeof(float));
    
            model.meshes[i].anim_normals = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float));
            memset(model.meshes[i].anim_normals, 0, model.meshes[i].vertex_count * 3 * sizeof(float));
    
            model.meshes[i].vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
            memset(model.meshes[i].vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));
        }
    
        // Triangles data processing
        tri = (rf_iqm_triangle*) rf_alloc(temp_allocator, iqm.num_triangles * sizeof(rf_iqm_triangle));
        memcpy(tri, data + iqm.ofs_triangles, iqm.num_triangles * sizeof(rf_iqm_triangle));
    
        for (rf_int m = 0; m < model.mesh_count; m++)
        {
            int tcounter = 0;
    
            for (rf_int i = imesh[m].first_triangle; i < (imesh[m].first_triangle + imesh[m].num_triangles); i++)
            {
                // IQM triangles are stored counter clockwise, but raylib sets opengl to clockwise drawing, so we swap them around
                model.meshes[m].indices[tcounter + 2] = tri[i].vertex[0] - imesh[m].first_vertex;
                model.meshes[m].indices[tcounter + 1] = tri[i].vertex[1] - imesh[m].first_vertex;
                model.meshes[m].indices[tcounter    ] = tri[i].vertex[2] - imesh[m].first_vertex;
                tcounter += 3;
            }
        }
    
        // Vertex arrays data processing
        va = (rf_iqm_vertex_array*) rf_alloc(temp_allocator, iqm.num_vertexarrays * sizeof(rf_iqm_vertex_array));
        memcpy(va, data + iqm.ofs_vertexarrays, iqm.num_vertexarrays * sizeof(rf_iqm_vertex_array));
    
        for (rf_int i = 0; i < iqm.num_vertexarrays; i++)
        {
            switch (va[i].type)
            {
                case RF_IQM_POSITION:
                {
                    vertex = (float*) rf_alloc(temp_allocator, iqm.num_vertexes * 3 * sizeof(float));
                    memcpy(vertex, data + va[i].offset, iqm.num_vertexes * 3 * sizeof(float));
    
                    for (rf_int m = 0; m < iqm.num_meshes; m++)
                    {
                        int vertex_pos_counter = 0;
                        for (rf_int ii = imesh[m].first_vertex * 3; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 3; ii++)
                        {
                            model.meshes[m].vertices     [vertex_pos_counter] = vertex[ii];
                            model.meshes[m].anim_vertices[vertex_pos_counter] = vertex[ii];
                            vertex_pos_counter++;
                        }
                    }
                } break;
    
                case RF_IQM_NORMAL:
                {
                    normal = (float*) rf_alloc(temp_allocator, iqm.num_vertexes * 3 * sizeof(float));
                    memcpy(normal, data + va[i].offset, iqm.num_vertexes * 3 * sizeof(float));
    
                    for (rf_int m = 0; m < iqm.num_meshes; m++)
                    {
                        int vertex_pos_counter = 0;
                        for (rf_int ii = imesh[m].first_vertex * 3; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 3; ii++)
                        {
                            model.meshes[m].normals     [vertex_pos_counter] = normal[ii];
                            model.meshes[m].anim_normals[vertex_pos_counter] = normal[ii];
                            vertex_pos_counter++;
                        }
                    }
                } break;
    
                case RF_IQM_TEXCOORD:
                {
                    text = (float*) rf_alloc(temp_allocator, iqm.num_vertexes * 2 * sizeof(float));
                    memcpy(text, data + va[i].offset, iqm.num_vertexes * 2 * sizeof(float));
    
                    for (rf_int m = 0; m < iqm.num_meshes; m++)
                    {
                        int vertex_pos_counter = 0;
                        for (rf_int ii = imesh[m].first_vertex * 2; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 2; ii++)
                        {
                            model.meshes[m].texcoords[vertex_pos_counter] = text[ii];
                            vertex_pos_counter++;
                        }
                    }
                } break;
    
                case RF_IQM_BLENDINDEXES:
                {
                    blendi = (char*) rf_alloc(temp_allocator, iqm.num_vertexes * 4 * sizeof(char));
                    memcpy(blendi, data + va[i].offset, iqm.num_vertexes * 4 * sizeof(char));
    
                    for (rf_int m = 0; m < iqm.num_meshes; m++)
                    {
                        int bone_counter = 0;
                        for (rf_int ii = imesh[m].first_vertex * 4; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; ii++)
                        {
                            model.meshes[m].bone_ids[bone_counter] = blendi[ii];
                            bone_counter++;
                        }
                    }
                } break;
    
                case RF_IQM_BLENDWEIGHTS:
                {
                    blendw = (unsigned char*) rf_alloc(temp_allocator, iqm.num_vertexes * 4 * sizeof(unsigned char));
                    memcpy(blendw, data + va[i].offset, iqm.num_vertexes * 4 * sizeof(unsigned char));
    
                    for (rf_int m = 0; m < iqm.num_meshes; m++)
                    {
                        int bone_counter = 0;
                        for (rf_int ii = imesh[m].first_vertex * 4; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; ii++)
                        {
                            model.meshes[m].bone_weights[bone_counter] = blendw[ii] / 255.0f;
                            bone_counter++;
                        }
                    }
                } break;
            }
        }
    
        // Bones (joints) data processing
        ijoint = (rf_iqm_joint*) rf_alloc(temp_allocator, iqm.num_joints * sizeof(rf_iqm_joint));
        memcpy(ijoint, data + iqm.ofs_joints, iqm.num_joints * sizeof(rf_iqm_joint));
    
        model.bone_count = iqm.num_joints;
        model.bones      = (rf_bone_info*) rf_alloc(allocator, iqm.num_joints * sizeof(rf_bone_info));
        model.bind_pose  = (rf_transform*) rf_alloc(allocator, iqm.num_joints * sizeof(rf_transform));
    
        for (rf_int i = 0; i < iqm.num_joints; i++)
        {
            // Bones
            model.bones[i].parent = ijoint[i].parent;
            memcpy(model.bones[i].name, data + iqm.ofs_text + ijoint[i].name, rf_bone_name_len * sizeof(char));
    
            // Bind pose (base pose)
            model.bind_pose[i].translation.x = ijoint[i].translate[0];
            model.bind_pose[i].translation.y = ijoint[i].translate[1];
            model.bind_pose[i].translation.z = ijoint[i].translate[2];
    
            model.bind_pose[i].rotation.x = ijoint[i].rotate[0];
            model.bind_pose[i].rotation.y = ijoint[i].rotate[1];
            model.bind_pose[i].rotation.z = ijoint[i].rotate[2];
            model.bind_pose[i].rotation.w = ijoint[i].rotate[3];
    
            model.bind_pose[i].scale.x = ijoint[i].scale[0];
            model.bind_pose[i].scale.y = ijoint[i].scale[1];
            model.bind_pose[i].scale.z = ijoint[i].scale[2];
        }
    
        // Build bind pose from parent joints
        for (rf_int i = 0; i < model.bone_count; i++)
        {
            if (model.bones[i].parent >= 0)
            {
                model.bind_pose[i].rotation    = rf_quaternion_mul(model.bind_pose[model.bones[i].parent].rotation, model.bind_pose[i].rotation);
                model.bind_pose[i].translation = rf_vec3_rotate_by_quaternion(model.bind_pose[i].translation, model.bind_pose[model.bones[i].parent].rotation);
                model.bind_pose[i].translation = rf_vec3_add(model.bind_pose[i].translation, model.bind_pose[model.bones[i].parent].translation);
                model.bind_pose[i].scale       = rf_vec3_mul_v(model.bind_pose[i].scale, model.bind_pose[model.bones[i].parent].scale);
            }
        }
    
        rf_free(temp_allocator, imesh);
        rf_free(temp_allocator, tri);
        rf_free(temp_allocator, va);
        rf_free(temp_allocator, vertex);
        rf_free(temp_allocator, normal);
        rf_free(temp_allocator, text);
        rf_free(temp_allocator, blendi);
        rf_free(temp_allocator, blendw);
        rf_free(temp_allocator, ijoint);
    }

    return rf_load_meshes_and_materials_for_model(model, allocator, temp_allocator);
}

/***********************************************************************************
    Function based on work by Wilhem Barbier (@wbrbr)

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
// Load texture from cgltf_image
rf_internal rf_texture2d rf_load_texture_from_cgltf_image(cgltf_image* image, const char* tex_path, rf_color tint, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_texture2d texture = {0};

    if (image->uri)
    {
        if (rf_str_match_prefix(rf_cstr(image->uri), rf_cstr("data:")))
        {
            // Data URI
            // Format: data:<mediatype>;base64,<data>

            // Find the comma
            int i = 0;
            while ((image->uri[i] != ',') && (image->uri[i] != 0)) i++;

            if (image->uri[i] == 0)
            {
                rf_log(rf_log_type_warning, "CGLTF rf_image: Invalid data URI");
            }
            else
            {
                rf_base64_output data = rf_decode_base64((const char*)image->uri + i + 1, temp_allocator);

                rf_image rimage = rf_load_image_from_file_data(data.buffer, data.size, 4, temp_allocator, temp_allocator);

                // TODO: Tint shouldn't be applied here!
                rf_image_color_tint(rimage, tint);

                texture = rf_load_texture_from_image(rimage);

                rf_image_free(rimage, temp_allocator);
                rf_free(temp_allocator, data.buffer);
            }
        }
        else
        {
            char buff[1024] = {0};
            snprintf(buff, 1024, "%s/%s", tex_path, image->uri);
            rf_image rimage = rf_load_image_from_file(buff, temp_allocator, temp_allocator, io);

            // TODO: Tint shouldn't be applied here!
            rf_image_color_tint(rimage, tint);

            texture = rf_load_texture_from_image(rimage);

            rf_image_free(rimage, temp_allocator);
        }
    }
    else if (image->buffer_view)
    {
        unsigned char* data = (unsigned char*) rf_alloc(temp_allocator, image->buffer_view->size);
        int n = image->buffer_view->offset;
        int stride = image->buffer_view->stride ? image->buffer_view->stride : 1;

        for (rf_int i = 0; i < image->buffer_view->size; i++)
        {
            data[i] = ((unsigned char* )image->buffer_view->buffer->data)[n];
            n += stride;
        }

        rf_image rimage = rf_load_image_from_file_data(data, image->buffer_view->size, 4, temp_allocator, temp_allocator);

        // TODO: Tint shouldn't be applied here!
        rf_image_color_tint(rimage, tint);

        texture = rf_load_texture_from_image(rimage);

        rf_image_free(rimage, temp_allocator);
        rf_free(temp_allocator, data);
    }
    else
    {
        texture = rf_load_texture_from_image((rf_image) {
            .data = &tint,
            .width = 1,
            .height = 1,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = true
        });
    }

    return texture;
}

// Load model from files (meshes and materials)
rf_extern rf_model rf_load_model_from_gltf(const char* path_to_file_cstr, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    #define rf_internal_gltf_load_accessor(type, nbcomp, acc, dst)\
    { \
        int n = 0; \
        type* buf = (type*) acc->buffer_view->buffer->data + acc->buffer_view->offset / sizeof(type) + acc->offset / sizeof(type); \
        for (rf_int k = 0; k < acc->count; k++) { \
            for (rf_int l = 0; l < nbcomp; l++) { \
                dst[nbcomp * k + l] = buf[n + l]; \
            } \
            n += acc->stride / sizeof(type); \
        } \
    }

    rf_model result = {0};

    cgltf_options options = {
        cgltf_file_type_invalid,
        .memory = {
            .alloc = &rf_cgltf_alloc,
            .free = rf_cgltf_free,
            .user_data = &temp_allocator
        },
        .file = {
            .read = &rf_cgltf_io_read,
            .release = &rf_cgltf_io_release,
            .user_data = &io
        }
    };

    rf_file_contents contents = rf_read_entire_file(path_to_file_cstr, temp_allocator, io);
    if (!contents.valid) return result;

    cgltf_data* cgltf_data = NULL;
    cgltf_result status = cgltf_parse(&options, contents.data, contents.size, &cgltf_data);

    if (status == cgltf_result_success)
    {
        rf_log(rf_log_type_info, "[%s][%s] rf_model meshes/materials: %i/%i", path_to_file_cstr, (cgltf_data->file_type == 2) ? "glb" : "gltf", cgltf_data->meshes_count, cgltf_data->materials_count);

        // Read cgltf_data buffers
        status = cgltf_load_buffers(&options, cgltf_data, path_to_file_cstr);
        if (status != cgltf_result_success) {
            rf_log(rf_log_type_info, "[%s][%s] Error loading mesh/material buffers", path_to_file_cstr, (cgltf_data->file_type == 2) ? "glb" : "gltf");
        }

        int primitivesCount = 0;

        for (rf_int i = 0; i < cgltf_data->meshes_count; i++)
        {
            primitivesCount += (int)cgltf_data->meshes[i].primitives_count;
        }

        // Process glTF cgltf_data and map to model
        allocator = allocator;
        result.mesh_count = primitivesCount;
        result.material_count = cgltf_data->materials_count + 1;
        result.meshes = (rf_mesh*) rf_alloc(allocator, result.mesh_count * sizeof(rf_mesh));
        result.materials = (rf_material*) rf_alloc(allocator, result.material_count * sizeof(rf_material));
        result.mesh_material = (int*) rf_alloc(allocator, result.mesh_count * sizeof(int));

        memset(result.meshes, 0, result.mesh_count * sizeof(rf_mesh));

        for (rf_int i = 0; i < result.mesh_count; i++)
        {
            result.meshes[i].vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
            memset(result.meshes[i].vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));
        }

        // For each material
        for (rf_int i = 0; i < result.material_count - 1; i++)
        {
            result.materials[i] = rf_load_default_material(allocator);
            rf_color tint = (rf_color){ 255, 255, 255, 255 };

            rf_str path_to_file_str = rf_cstr(path_to_file_cstr);
            rf_int last_slash = rf_str_find_last(path_to_file_str, rf_cstr("\\"));
            if (last_slash == rf_invalid_index) last_slash = rf_str_find_last(rf_cstr(path_to_file_cstr), rf_cstr("/"));

            rf_assert(last_slash != rf_invalid_index);
            rf_str_buf tex_path = rf_strbuf_from_str(rf_str_sub_b(path_to_file_str, 0, last_slash), temp_allocator);

            // Ensure material follows raylib support for PBR (metallic/roughness flow)
            if (cgltf_data->materials[i].has_pbr_metallic_roughness)
            {
                float roughness = cgltf_data->materials[i].pbr_metallic_roughness.roughness_factor;
                float metallic = cgltf_data->materials[i].pbr_metallic_roughness.metallic_factor;

                // NOTE: rf_material name not used for the moment
                //if (model.materials[i].name && cgltf_data->materials[i].name) strcpy(model.materials[i].name, cgltf_data->materials[i].name);

                // TODO: REview: shouldn't these be *255 ???
                tint.r = cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[0] * 255;
                tint.g = cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[1] * 255;
                tint.b = cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[2] * 255;
                tint.a = cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[3] * 255;

                result.materials[i].maps[RF_MAP_ROUGHNESS].color = tint;

                if (cgltf_data->materials[i].pbr_metallic_roughness.base_color_texture.texture)
                {
                    result.materials[i].maps[RF_MAP_ALBEDO].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].pbr_metallic_roughness.base_color_texture.texture->image, tex_path.data, tint, temp_allocator, io);
                }

                // NOTE: Tint isn't need for other textures.. pass null or clear?
                // Just set as white, multiplying by white has no effect
                tint = rf_white;

                if (cgltf_data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture)
                {
                    result.materials[i].maps[RF_MAP_ROUGHNESS].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture->image, tex_path.data, tint, temp_allocator, io);
                }

                result.materials[i].maps[RF_MAP_ROUGHNESS].value = roughness;
                result.materials[i].maps[RF_MAP_METALNESS].value = metallic;

                if (cgltf_data->materials[i].normal_texture.texture)
                {
                    result.materials[i].maps[RF_MAP_NORMAL].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].normal_texture.texture->image, tex_path.data, tint, temp_allocator, io);
                }

                if (cgltf_data->materials[i].occlusion_texture.texture)
                {
                    result.materials[i].maps[RF_MAP_OCCLUSION].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].occlusion_texture.texture->image, tex_path.data, tint, temp_allocator, io);
                }
            }

            rf_strbuf_free(&tex_path);
        }

        result.materials[result.material_count - 1] = rf_load_default_material(allocator);

        int primitive_index = 0;

        for (rf_int i = 0; i < cgltf_data->meshes_count; i++)
        {
            for (rf_int p = 0; p < cgltf_data->meshes[i].primitives_count; p++)
            {
                for (rf_int j = 0; j < cgltf_data->meshes[i].primitives[p].attributes_count; j++)
                {
                    if (cgltf_data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_position)
                    {
                        cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].attributes[j].data;
                        result.meshes[primitive_index].vertex_count = acc->count;
                        result.meshes[primitive_index].vertices = (float*) rf_alloc(allocator, sizeof(float) * result.meshes[primitive_index].vertex_count * 3);

                        rf_internal_gltf_load_accessor(float, 3, acc, result.meshes[primitive_index].vertices)
                    }
                    else if (cgltf_data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_normal)
                    {
                        cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].attributes[j].data;
                        result.meshes[primitive_index].normals = (float*) rf_alloc(allocator, sizeof(float) * acc->count * 3);

                        rf_internal_gltf_load_accessor(float, 3, acc, result.meshes[primitive_index].normals)
                    }
                    else if (cgltf_data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_texcoord)
                    {
                        cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].attributes[j].data;

                        if (acc->component_type == cgltf_component_type_r_32f)
                        {
                            result.meshes[primitive_index].texcoords = (float*) rf_alloc(allocator, sizeof(float) * acc->count * 2);
                            rf_internal_gltf_load_accessor(float, 2, acc, result.meshes[primitive_index].texcoords)
                        }
                        else
                        {
                            // TODO: Support normalized unsigned unsigned char/unsigned short texture coordinates
                            rf_log(rf_log_type_warning, "[%s] rf_texture coordinates must be float", path_to_file_cstr);
                        }
                    }
                }

                cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].indices;

                if (acc)
                {
                    if (acc->component_type == cgltf_component_type_r_16u)
                    {
                        result.meshes[primitive_index].triangle_count = acc->count / 3;
                        result.meshes[primitive_index].indices = (unsigned short*) rf_alloc(allocator, sizeof(unsigned short) * result.meshes[primitive_index].triangle_count * 3);
                        rf_internal_gltf_load_accessor(unsigned short, 1, acc, result.meshes[primitive_index].indices)
                    }
                    else
                    {
                        // TODO: Support unsigned unsigned char/unsigned int
                        rf_log(rf_log_type_warning, "[%s] Indices must be unsigned short", path_to_file_cstr);
                    }
                }
                else
                {
                    // Unindexed mesh
                    result.meshes[primitive_index].triangle_count = result.meshes[primitive_index].vertex_count / 3;
                }

                if (cgltf_data->meshes[i].primitives[p].material)
                {
                    // Compute the offset
                    result.mesh_material[primitive_index] = cgltf_data->meshes[i].primitives[p].material - cgltf_data->materials;
                }
                else
                {
                    result.mesh_material[primitive_index] = result.material_count - 1;
                }

                primitive_index++;
            }
        }

        cgltf_free(cgltf_data);
    }
    else
    {
        rf_log(rf_log_type_warning, "[%s] glTF cgltf_data could not be loaded", path_to_file_cstr);
    }

    rf_global_allocator_for_thirdparty_libraries = (rf_allocator) {0};

    return result;

    #undef rf_internal_gltf_load_accessor
}

// Load model from generated mesh. Note: The function takes ownership of the mesh in model.meshes[0]
rf_extern rf_model rf_load_model_from_mesh(rf_mesh mesh, rf_allocator allocator)
{
    rf_model model = {0};

    model.transform = rf_mat_identity();

    model.mesh_count = 1;
    model.meshes = rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
    memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
    model.meshes[0] = mesh;

    model.material_count = 1;
    model.materials = rf_alloc(allocator, model.material_count * sizeof(rf_material));
    memset(model.materials, 0, model.material_count * sizeof(rf_material));
    model.materials[0] = rf_load_default_material(allocator);

    model.mesh_material = rf_alloc(allocator, model.mesh_count * sizeof(int));
    memset(model.mesh_material, 0, model.mesh_count * sizeof(int));
    model.mesh_material[0] = 0; // First material index

    return model;
}

// Get collision info between ray and model
rf_extern rf_ray_hit_info rf_collision_ray_model(rf_ray ray, rf_model model)
{
    rf_ray_hit_info result = {0};

    for (rf_int m = 0; m < model.mesh_count; m++)
    {
        // Check if mesh has vertex data on CPU for testing
        if (model.meshes[m].vertices != NULL)
        {
            // model->mesh.triangle_count may not be set, vertex_count is more reliable
            int triangle_count = model.meshes[m].vertex_count / 3;

            // Test against all triangles in mesh
            for (rf_int i = 0; i < triangle_count; i++)
            {
                rf_vec3 a, b, c;
                rf_vec3 *vertdata = (rf_vec3 *) model.meshes[m].vertices;

                if (model.meshes[m].indices)
                {
                    a = vertdata[model.meshes[m].indices[i * 3 + 0]];
                    b = vertdata[model.meshes[m].indices[i * 3 + 1]];
                    c = vertdata[model.meshes[m].indices[i * 3 + 2]];
                }
                else
                {
                    a = vertdata[i * 3 + 0];
                    b = vertdata[i * 3 + 1];
                    c = vertdata[i * 3 + 2];
                }

                a = rf_vec3_transform(a, model.transform);
                b = rf_vec3_transform(b, model.transform);
                c = rf_vec3_transform(c, model.transform);

                rf_ray_hit_info tri_hit_info = rf_collision_ray_triangle(ray, a, b, c);

                if (tri_hit_info.hit)
                {
                    // Save the closest hit triangle
                    if ((!result.hit) || (result.distance > tri_hit_info.distance))
                    {
                        result = tri_hit_info;
                    }
                }
            }
        }
    }

    return result;
}

rf_extern void rf_model_mesh_set_material(rf_model* model, int mesh_id, int material_id); // Set material for a mesh

// Unload model from memory (RAM and/or VRAM)
rf_extern void rf_model_free(rf_model model, rf_allocator allocator)
{
    for (rf_int i = 0; i < model.mesh_count; i++)
    {
        rf_unload_mesh(model.meshes[i], allocator);
    }

    // As the user could be sharing shaders and textures between models,
    // we don't unload the material but just free it's maps, the user
    // is responsible for freeing models shaders and textures
    for (rf_int i = 0; i < model.material_count; i++)
    {
        rf_free(allocator, model.materials[i].maps);
    }

    rf_free(allocator, model.meshes);
    rf_free(allocator, model.materials);
    rf_free(allocator, model.mesh_material);

    // Unload animation data
    rf_free(allocator, model.bones);
    rf_free(allocator, model.bind_pose);

    rf_log(rf_log_type_info, "Unloaded model data from RAM and VRAM");
}