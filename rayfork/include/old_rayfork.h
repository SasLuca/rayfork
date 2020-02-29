
// Load materials from model file
// @Incomplete
// Note: Depends on IO stuff, fix later
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

