//Implementation of the texture npatch drawing example from raylib using rayfork
#include "game.h"
#include "include/rayfork.h"
#include "glad.h"

rf_context   rf_ctx;
rf_default_render_batch    rf_mem;
rf_default_font     default_font_buffers;

// NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
rf_texture2d npatch_texture;

rf_vec2 origin = { 0.0f, 0.0f };

// Position and size of the n-patches
rf_rec dstRec1 = { 480.0f, 160.0f, 32.0f, 32.0f };
rf_rec dstRec2 = { 160.0f, 160.0f, 32.0f, 32.0f };
rf_rec dstRecH = { 160.0f, 93.0f, 32.0f, 32.0f };
rf_rec dstRecV = { 92.0f, 160.0f, 32.0f, 32.0f };

// A 9-patch (NPT_9PATCH) changes its sizes in both axis
rf_npatch_info nine_patch_info_1 = { (rf_rec){ 0.0f, 0.0f, 64.0f, 64.0f }, 12, 40, 12, 12, RF_NPT_9PATCH };
rf_npatch_info nine_patch_info_2 = { (rf_rec){ 0.0f, 128.0f, 64.0f, 64.0f }, 16, 16, 16, 16, RF_NPT_9PATCH };

// A horizontal 3-patch (NPT_3PATCH_HORIZONTAL) changes its sizes along the x axis only
rf_npatch_info h3_patch_info = { (rf_rec){ 0.0f,  64.0f, 64.0f, 64.0f }, 8, 8, 8, 8, RF_NPT_3PATCH_HORIZONTAL };

// A vertical 3-patch (NPT_3PATCH_VERTICAL) changes its sizes along the y axis only
rf_npatch_info v3_patch_info = { (rf_rec){ 0.0f, 192.0f, 64.0f, 64.0f }, 6, 6, 6, 6, RF_NPT_3PATCH_VERTICAL };

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    npatch_texture = rf_load_texture_from_file(ASSETS_PATH"ninepatch_button.png", RF_DEFAULT_ALLOCATOR, RF_DEFAULT_IO);
}

void on_frame(const input_data input)
{
    // Update

    // Resize the n-patches based on mouse position
    dstRec1.width = input.mouse_x - dstRec1.x;
    dstRec1.height = input.mouse_y - dstRec1.y;
    dstRec2.width = input.mouse_x - dstRec2.x;
    dstRec2.height = input.mouse_y - dstRec2.y;
    dstRecH.width = input.mouse_x - dstRecH.x;
    dstRecV.height = input.mouse_y - dstRecV.y;

    // Set a minimum width and/or height
    if (dstRec1.width < 1.0f) dstRec1.width = 1.0f;
    if (dstRec1.width > 300.0f) dstRec1.width = 300.0f;
    if (dstRec1.height < 1.0f) dstRec1.height = 1.0f;
    if (dstRec2.width < 1.0f) dstRec2.width = 1.0f;
    if (dstRec2.width > 300.0f) dstRec2.width = 300.0f;
    if (dstRec2.height < 1.0f) dstRec2.height = 1.0f;
    if (dstRecH.width < 1.0f) dstRecH.width = 1.0f;
    if (dstRecV.height < 1.0f) dstRecV.height = 1.0f;

    // Draw
    rf_begin();

        rf_clear(RF_RAYWHITE);

    // Draw the n-patches
    rf_draw_texture_npatch(npatch_texture, nine_patch_info_2, dstRec2, origin, 0.0f, RF_WHITE);
    rf_draw_texture_npatch(npatch_texture, nine_patch_info_1, dstRec1, origin, 0.0f, RF_WHITE);
    rf_draw_texture_npatch(npatch_texture, h3_patch_info, dstRecH, origin, 0.0f, RF_WHITE);
    rf_draw_texture_npatch(npatch_texture, v3_patch_info, dstRecV, origin, 0.0f, RF_WHITE);

    // Draw the source texture
    rf_draw_rectangle_outline((rf_rec){ 5, 88, 74, 266 }, 1.0f, RF_BLUE);
    rf_draw_texture(npatch_texture, 10, 93, RF_WHITE);
    rf_draw_text("TEXTURE", 15, 360, 10, RF_DARKGRAY);

    rf_draw_text("Move the mouse to stretch or shrink the n-patches", 10, 20, 20, RF_DARKGRAY);

    rf_end();
}