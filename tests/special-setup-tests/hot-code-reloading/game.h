#include "stdint.h"
#include "include/rayfork.h"

typedef struct game_data_t game_data_t;
struct game_data_t
{
    rf_context rf_ctx;
    rf_default_render_batch rf_mem;

    int screen_width;
    int screen_height;

    int reload_count;

    void* (*alloc)(size_t size);
    void (*free)(void*);
};