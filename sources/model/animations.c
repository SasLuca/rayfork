#include "rayfork/model/animations.h"

#include "rayfork/gfx/gfx.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/math/maths.h"

#define rf_iqm_magic "INTERQUAKEMODEL" // IQM file magic number
#define rf_iqm_version 2 // only IQM version 2 supported

typedef struct rf_iqm_header
{
    char magic[16];
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

typedef struct rf_iqm_pose
{
    int parent;
    unsigned int mask;
    float channeloffset[10];
    float channelscale[10];
} rf_iqm_pose;

typedef struct rf_iqm_anim
{
    unsigned int name;
    unsigned int first_frame, num_frames;
    float framerate;
    unsigned int flags;
} rf_iqm_anim;

rf_extern rf_model_animation_array rf_load_model_animations_from_iqm_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks* io)
{
    rf_file_contents file_contents = rf_read_entire_file(filename, temp_allocator, io);
    rf_model_animation_array result = rf_load_model_animations_from_iqm(file_contents.data, file_contents.size, allocator, temp_allocator);
    rf_file_contents_free(file_contents, temp_allocator);

    return result;
}

rf_extern rf_model_animation_array rf_load_model_animations_from_iqm(const unsigned char* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (!data || !data_size) return (rf_model_animation_array) {0};

    rf_iqm_header iqm = {0};
    memcpy(&iqm, data, sizeof(rf_iqm_header));

    if (strncmp(iqm.magic, rf_iqm_magic, sizeof(rf_iqm_magic)))
    {
        char temp_str[sizeof(rf_iqm_magic) + 1] = {0};
        memcpy(temp_str, iqm.magic, sizeof(rf_iqm_magic));
        rf_log_error(rf_bad_format, "Magic Number \"%s\"does not match.", temp_str);

        return (rf_model_animation_array){0};
    }

    if (iqm.version != rf_iqm_version)
    {
        rf_log_error(rf_bad_format, "IQM version %i is incorrect.", iqm.version);

        return (rf_model_animation_array){0};
    }

    rf_model_animation_array result = {
        .size = iqm.num_anims,
    };

    // Get bones data
    rf_iqm_pose* poses = (rf_iqm_pose*) rf_alloc(temp_allocator, iqm.num_poses * sizeof(rf_iqm_pose));
    memcpy(poses, data + iqm.ofs_poses, iqm.num_poses * sizeof(rf_iqm_pose));

    // Get animations data
    rf_iqm_anim* anim = (rf_iqm_anim*) rf_alloc(temp_allocator, iqm.num_anims * sizeof(rf_iqm_anim));
    memcpy(anim, data + iqm.ofs_anims, iqm.num_anims * sizeof(rf_iqm_anim));

    rf_model_animation* animations = (rf_model_animation*) rf_alloc(allocator, iqm.num_anims * sizeof(rf_model_animation));

    result.anims = animations;
    result.size  = iqm.num_anims;

    // frameposes
    unsigned short* framedata = (unsigned short*) rf_alloc(temp_allocator, iqm.num_frames * iqm.num_framechannels * sizeof(unsigned short));
    memcpy(framedata, data + iqm.ofs_frames, iqm.num_frames*iqm.num_framechannels * sizeof(unsigned short));

    for (rf_int a = 0; a < iqm.num_anims; a++)
    {
        animations[a].frame_count = anim[a].num_frames;
        animations[a].bone_count  = iqm.num_poses;
        animations[a].bones       = (rf_bone_info*) rf_alloc(allocator, iqm.num_poses * sizeof(rf_bone_info));
        animations[a].frame_poses = (rf_transform**) rf_alloc(allocator, anim[a].num_frames * sizeof(rf_transform*));
        //animations[a].framerate = anim.framerate;     // TODO: Use framerate?

        for (rf_int j = 0; j < iqm.num_poses; j++)
        {
            strcpy(animations[a].bones[j].name, "ANIMJOINTNAME");
            animations[a].bones[j].parent = poses[j].parent;
        }

        for (rf_int j = 0; j < anim[a].num_frames; j++)
        {
            animations[a].frame_poses[j] = (rf_transform*) rf_alloc(allocator, iqm.num_poses * sizeof(rf_transform));
        }

        int dcounter = anim[a].first_frame*iqm.num_framechannels;

        for (rf_int frame = 0; frame < anim[a].num_frames; frame++)
        {
            for (rf_int i = 0; i < iqm.num_poses; i++)
            {
                animations[a].frame_poses[frame][i].translation.x = poses[i].channeloffset[0];

                if (poses[i].mask & 0x01)
                {
                    animations[a].frame_poses[frame][i].translation.x += framedata[dcounter]*poses[i].channelscale[0];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].translation.y = poses[i].channeloffset[1];

                if (poses[i].mask & 0x02)
                {
                    animations[a].frame_poses[frame][i].translation.y += framedata[dcounter]*poses[i].channelscale[1];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].translation.z = poses[i].channeloffset[2];

                if (poses[i].mask & 0x04)
                {
                    animations[a].frame_poses[frame][i].translation.z += framedata[dcounter]*poses[i].channelscale[2];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.x = poses[i].channeloffset[3];

                if (poses[i].mask & 0x08)
                {
                    animations[a].frame_poses[frame][i].rotation.x += framedata[dcounter]*poses[i].channelscale[3];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.y = poses[i].channeloffset[4];

                if (poses[i].mask & 0x10)
                {
                    animations[a].frame_poses[frame][i].rotation.y += framedata[dcounter]*poses[i].channelscale[4];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.z = poses[i].channeloffset[5];

                if (poses[i].mask & 0x20)
                {
                    animations[a].frame_poses[frame][i].rotation.z += framedata[dcounter]*poses[i].channelscale[5];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.w = poses[i].channeloffset[6];

                if (poses[i].mask & 0x40)
                {
                    animations[a].frame_poses[frame][i].rotation.w += framedata[dcounter]*poses[i].channelscale[6];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].scale.x = poses[i].channeloffset[7];

                if (poses[i].mask & 0x80)
                {
                    animations[a].frame_poses[frame][i].scale.x += framedata[dcounter]*poses[i].channelscale[7];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].scale.y = poses[i].channeloffset[8];

                if (poses[i].mask & 0x100)
                {
                    animations[a].frame_poses[frame][i].scale.y += framedata[dcounter]*poses[i].channelscale[8];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].scale.z = poses[i].channeloffset[9];

                if (poses[i].mask & 0x200)
                {
                    animations[a].frame_poses[frame][i].scale.z += framedata[dcounter]*poses[i].channelscale[9];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation = rf_quaternion_normalize(animations[a].frame_poses[frame][i].rotation);
            }
        }

        // Build frameposes
        for (rf_int frame = 0; frame < anim[a].num_frames; frame++)
        {
            for (rf_int i = 0; i < animations[a].bone_count; i++)
            {
                if (animations[a].bones[i].parent >= 0)
                {
                    animations[a].frame_poses[frame][i].rotation    = rf_quaternion_mul(animations[a].frame_poses[frame][animations[a].bones[i].parent].rotation, animations[a].frame_poses[frame][i].rotation);
                    animations[a].frame_poses[frame][i].translation = rf_vec3_rotate_by_quaternion(animations[a].frame_poses[frame][i].translation, animations[a].frame_poses[frame][animations[a].bones[i].parent].rotation);
                    animations[a].frame_poses[frame][i].translation = rf_vec3_add(animations[a].frame_poses[frame][i].translation, animations[a].frame_poses[frame][animations[a].bones[i].parent].translation);
                    animations[a].frame_poses[frame][i].scale       = rf_vec3_mul_v(animations[a].frame_poses[frame][i].scale, animations[a].frame_poses[frame][animations[a].bones[i].parent].scale);
                }
            }
        }
    }

    rf_free(temp_allocator, framedata);
    rf_free(temp_allocator, poses);
    rf_free(temp_allocator, anim);

    return result;
}

// Update model animated vertex data (positions and normals) for a given frame
rf_extern void rf_model_animation_update(rf_model model, rf_model_animation anim, int frame)
{
    if ((anim.frame_count > 0) && (anim.bones != NULL) && (anim.frame_poses != NULL))
    {
        return;
    }

    if (frame >= anim.frame_count)
    {
        frame = frame%anim.frame_count;
    }

    for (rf_int m = 0; m < model.mesh_count; m++)
    {
        rf_vec3 anim_vertex = {0};
        rf_vec3 anim_normal = {0};

        rf_vec3 in_translation = {0};
        rf_quaternion in_rotation = {0};
        rf_vec3 in_scale = {0};

        rf_vec3 out_translation = {0};
        rf_quaternion out_rotation = {0};
        rf_vec3 out_scale = {0};

        int vertex_pos_counter = 0;
        int bone_counter = 0;
        int bone_id = 0;

        for (rf_int i = 0; i < model.meshes[m].vertex_count; i++)
        {
            bone_id = model.meshes[m].bone_ids[bone_counter];
            in_translation = model.bind_pose[bone_id].translation;
            in_rotation = model.bind_pose[bone_id].rotation;
            in_scale = model.bind_pose[bone_id].scale;
            out_translation = anim.frame_poses[frame][bone_id].translation;
            out_rotation = anim.frame_poses[frame][bone_id].rotation;
            out_scale = anim.frame_poses[frame][bone_id].scale;

            // Vertices processing
            // NOTE: We use meshes.vertices (default vertex position) to calculate meshes.anim_vertices (animated vertex position)
            anim_vertex = (rf_vec3){model.meshes[m].vertices[vertex_pos_counter], model.meshes[m].vertices[vertex_pos_counter + 1], model.meshes[m].vertices[vertex_pos_counter + 2] };
            anim_vertex = rf_vec3_mul_v(anim_vertex, out_scale);
            anim_vertex = rf_vec3_sub(anim_vertex, in_translation);
            anim_vertex = rf_vec3_rotate_by_quaternion(anim_vertex, rf_quaternion_mul(out_rotation, rf_quaternion_invert(in_rotation)));
            anim_vertex = rf_vec3_add(anim_vertex, out_translation);
            model.meshes[m].anim_vertices[vertex_pos_counter] = anim_vertex.x;
            model.meshes[m].anim_vertices[vertex_pos_counter + 1] = anim_vertex.y;
            model.meshes[m].anim_vertices[vertex_pos_counter + 2] = anim_vertex.z;

            // Normals processing
            // NOTE: We use meshes.baseNormals (default normal) to calculate meshes.normals (animated normals)
            anim_normal = (rf_vec3){model.meshes[m].normals[vertex_pos_counter], model.meshes[m].normals[vertex_pos_counter + 1], model.meshes[m].normals[vertex_pos_counter + 2] };
            anim_normal = rf_vec3_rotate_by_quaternion(anim_normal, rf_quaternion_mul(out_rotation, rf_quaternion_invert(in_rotation)));
            model.meshes[m].anim_normals[vertex_pos_counter] = anim_normal.x;
            model.meshes[m].anim_normals[vertex_pos_counter + 1] = anim_normal.y;
            model.meshes[m].anim_normals[vertex_pos_counter + 2] = anim_normal.z;
            vertex_pos_counter += 3;

            bone_counter += 4;
        }

        // Upload new vertex data to GPU for model drawing
        rf_gfx_update_buffer(model.meshes[m].vbo_id[0], model.meshes[m].anim_vertices, model.meshes[m].vertex_count * 3 * sizeof(float)); // Update vertex position
        rf_gfx_update_buffer(model.meshes[m].vbo_id[2], model.meshes[m].anim_vertices, model.meshes[m].vertex_count * 3 * sizeof(float)); // Update vertex normals
    }
}

// Check model animation skeleton match. Only number of bones and parent connections are checked
rf_extern rf_valid_t rf_model_animation_valid(rf_model model, rf_model_animation anim)
{
    rf_valid_t result = rf_valid;

    if (model.bone_count != anim.bone_count)
    {
        result = rf_invalid;
    }
    else
    {
        for (rf_int i = 0; i < model.bone_count; i++)
        {
            if (model.bones[i].parent != anim.bones[i].parent)
            {
                result = rf_invalid; break;
            }
        }
    }

    return result;
}

// Unload animation data
rf_extern void rf_model_animation_free(rf_model_animation anim, rf_allocator allocator)
{
    for (rf_int i = 0; i < anim.frame_count; i++) rf_free(allocator, anim.frame_poses[i]);

    rf_free(allocator, anim.bones);
    rf_free(allocator, anim.frame_poses);
}