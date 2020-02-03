#include "stdint.h"
#include "rayfork.h"

typedef struct game_data game_data;
struct game_data
{
    rf_renderer_context rf_ctx;
    int screen_width;
    int screen_height;

    int reload_count;

    void* (*alloc)(size_t size);
    void (*free)(void*);
};