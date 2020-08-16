// Implementation of the input example from raylib using rayfork

#define SOKOL_IMPL
#include "sokol_time.h"
#include "include/rayfork.h"
#include "glad/glad.h"
#include "game.h"

#define G (400)
#define PLAYER_JUMP_SPD (350.0f)
#define PLAYER_HOR_SPD (200.0f)

rf_context rf_ctx;
rf_default_render_batch rf_mem;
rf_default_font default_font_buffers;

typedef struct player_t player_t;
struct player_t
{
    rf_vec2 position;
    float speed;
    bool can_jump;
};

typedef struct env_item_t env_item_t;
struct env_item_t
{
    rf_rec rect;
    int blocking;
    rf_color color;
};

void update_player(input_data input, float delta);

void update_camera_center(float delta);
void update_camera_center_inside_map(float delta);
void update_camera_center_smooth_follow(float delta);
void update_camera_even_out_on_landing(float delta);
void update_camera_player_bounds_push(float delta);

player_t player = { 0 };
env_item_t env_items[] = {
    {(rf_rec){ 0, 0, 1000, 400 }, 0, RF_LIGHTGRAY },
    {(rf_rec){ 0, 400, 1000, 200 }, 1, RF_GRAY },
    {(rf_rec){ 300, 200, 400, 10 }, 1, RF_GRAY },
    {(rf_rec){ 250, 300, 100, 10 }, 1, RF_GRAY },
    {(rf_rec){ 650, 300, 100, 10 }, 1, RF_GRAY }
};

int env_items_length = sizeof(env_items) / sizeof(env_items[0]);
rf_camera2d camera = { 0 };

// Store pointers to the multiple update camera functions
void (*camera_updaters[])(float) = {
        update_camera_center,
        update_camera_center_inside_map,
        update_camera_center_smooth_follow,
        update_camera_even_out_on_landing,
        update_camera_player_bounds_push
};

int camera_option = 0;
int camera_updaters_length = sizeof(camera_updaters) / sizeof(camera_updaters[0]);

char *cameraDescriptions[] = {
        "Follow player center",
        "Follow player center, but clamp to map edges",
        "Follow player center; smoothed",
        "Follow player center horizontally; Update player center vertically after landing",
        "Player push camera on getting too close to screen edge"
};

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    stm_setup();

    // Initialise rayfork and load the default font
    rf_init(&rf_ctx, &rf_mem, SCREEN_WIDTH, SCREEN_HEIGHT, RF_DEFAULT_OPENGL_PROCS);
    rf_load_default_font(&default_font_buffers);

    player.position = (rf_vec2) { 400, 280 };
    player.speed = 0;
    player.can_jump = false;

    camera.target = player.position;
    camera.offset = (rf_vec2) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void on_frame(const input_data input)
{
    // Update
    static uint64_t frame_time_begin = 0;
    static float delta_time = 0;
    delta_time = stm_sec(stm_since(frame_time_begin));
    frame_time_begin = stm_now();

    update_player(input, delta_time);

    if (input.mouse_scroll_y) camera.zoom += input.mouse_scroll_y/12;

    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

    if (input.r_down)
    {
        camera.zoom = 1.0f;
        player.position = (rf_vec2){ 400, 280 };
    }

    if (input.c_down) camera_option = (camera_option + 1) % camera_updaters_length;

    // Call update camera function by its pointer
    camera_updaters[camera_option](delta_time);

    //Render
    rf_begin();

        rf_clear(RF_RAYWHITE);

        rf_begin_2d(camera);

            for (int i = 0; i < env_items_length; i++) rf_draw_rectangle_rec(env_items[i].rect, env_items[i].color);

            rf_rec playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
            rf_draw_rectangle_rec(playerRect, RF_RED);

        rf_end_2d();

        rf_draw_text("Controls:", 20, 20, 10, RF_BLACK);
        rf_draw_text("- Right/Left to move", 40, 40, 10, RF_DARKGRAY);
        rf_draw_text("- Space to jump", 40, 60, 10, RF_DARKGRAY);
        rf_draw_text("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 10, RF_DARKGRAY);
        rf_draw_text("- C to change camera mode", 40, 100, 10, RF_DARKGRAY);
        rf_draw_text("Current camera mode:", 20, 120, 10, RF_BLACK);
        rf_draw_text(cameraDescriptions[camera_option], 40, 140, 10, RF_DARKGRAY);

    rf_end();

    //Sleep(10);
}

void update_player(const input_data input, float delta)
{
    if (input.left_pressed) player.position.x -= PLAYER_HOR_SPD * delta;
    if (input.right_pressed) player.position.x += PLAYER_HOR_SPD * delta;
    if (input.space_pressed && player.can_jump)
    {
        player.speed = -PLAYER_JUMP_SPD;
        player.can_jump = false;
    }

    int hit_obstacle = 0;
    for (int i = 0; i < env_items_length; i++)
    {
        env_item_t *ei = env_items + i;
        rf_vec2 *p = &(player.position);
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y < p->y + player.speed*delta)
        {
            hit_obstacle = 1;
            player.speed = 0.0f;
            p->y = ei->rect.y;
        }
    }

    if (!hit_obstacle)
    {
        player.position.y += player.speed * delta;
        player.speed += G * delta;
        player.can_jump = false;
    }
    else player.can_jump = true;
}

void update_camera_center(float delta)
{
    camera.offset = (rf_vec2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
    camera.target = player.position;
}

void update_camera_center_inside_map(float delta)
{
    camera.target = player.position;
    camera.offset = (rf_vec2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

    for (int i = 0; i < env_items_length; i++)
    {
        env_item_t *ei = env_items + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }

    rf_vec2 max = rf_get_world_to_screen2d((rf_vec2){ maxX, maxY }, camera);
    rf_vec2 min = rf_get_world_to_screen2d((rf_vec2){ minX, minY }, camera);

    if (max.x < SCREEN_WIDTH) camera.offset.x = SCREEN_WIDTH - (max.x - SCREEN_WIDTH/2);
    if (max.y < SCREEN_HEIGHT) camera.offset.y = SCREEN_HEIGHT - (max.y - SCREEN_HEIGHT/2);
    if (min.x > 0) camera.offset.x = SCREEN_WIDTH/2 - min.x;
    if (min.y > 0) camera.offset.y = SCREEN_HEIGHT/2 - min.y;
}

void update_camera_center_smooth_follow(float delta)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera.offset = (rf_vec2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
    rf_vec2 diff = rf_vec2_sub(player.position, camera.target);
    float length = rf_vec2_len(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera.target = rf_vec2_add(camera.target, rf_vec2_scale(diff, speed*delta/length));
    }
}

void update_camera_even_out_on_landing(float delta)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;

    camera.offset = (rf_vec2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    camera.target.x = player.position.x;

    if (eveningOut)
    {
        if (evenOutTarget > camera.target.y)
        {
            camera.target.y += evenOutSpeed * delta;

            if (camera.target.y > evenOutTarget)
            {
                camera.target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
        else
        {
            camera.target.y -= evenOutSpeed * delta;

            if (camera.target.y < evenOutTarget)
            {
                camera.target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
    }
    else
    {
        if (player.can_jump && (player.speed == 0) && (player.position.y != camera.target.y))
        {
            eveningOut = 1;
            evenOutTarget = player.position.y;
        }
    }
}

void update_camera_player_bounds_push(float delta)
{
    static rf_vec2 bbox = { 0.2f, 0.2f };

    rf_vec2 bbox_world_min = rf_get_screen_to_world2d((rf_vec2){ (1 - bbox.x)*0.5f*SCREEN_WIDTH, (1 - bbox.y)*0.5f*SCREEN_HEIGHT }, camera);
    rf_vec2 bbox_world_max = rf_get_screen_to_world2d((rf_vec2){ (1 + bbox.x)*0.5f*SCREEN_WIDTH, (1 + bbox.y)*0.5f*SCREEN_HEIGHT }, camera);
    camera.offset = (rf_vec2){ (1 - bbox.x)*0.5f * SCREEN_WIDTH, (1 - bbox.y)*0.5f*SCREEN_HEIGHT };

    if (player.position.x < bbox_world_min.x) camera.target.x = player.position.x;
    if (player.position.y < bbox_world_min.y) camera.target.y = player.position.y;
    if (player.position.x > bbox_world_max.x) camera.target.x = bbox_world_min.x + (player.position.x - bbox_world_max.x);
    if (player.position.y > bbox_world_max.y) camera.target.y = bbox_world_min.y + (player.position.y - bbox_world_max.y);
}