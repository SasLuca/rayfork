#include "platform-common/platform.h"

#define G (400)
#define PLAYER_JUMP_SPD (350.0f)
#define PLAYER_HOR_SPD (200.0f)

typedef struct player_t
{
    rf_vec2 position;
    float speed;
    bool can_jump;
} player_t;

typedef struct env_item_t
{
    rf_rec rect;
    int blocking;
    rf_color color;
} env_item_t;

platform_window_details window = {
    .width  = 800,
    .height = 450,
    .title  = "rayfork - basic shapes"
};

static void update_player(const platform_input_state* input, float delta);
static void update_camera_center(float delta);
static void update_camera_center_inside_map(float delta);
static void update_camera_center_smooth_follow(float delta);
static void update_camera_even_out_on_landing(float delta);
static void update_camera_player_bounds_push(float delta);

rf_gfx_context ctx;
rf_render_batch batch;

rf_image image;
rf_texture2d texture;

player_t player;
env_item_t env_items[] = {
    {(rf_rec){ 0, 0, 1000, 400 }, 0, rf_lightgray },
    {(rf_rec){ 0, 400, 1000, 200 }, 1, rf_gray },
    {(rf_rec){ 300, 200, 400, 10 }, 1, rf_gray },
    {(rf_rec){ 250, 300, 100, 10 }, 1, rf_gray },
    {(rf_rec){ 650, 300, 100, 10 }, 1, rf_gray }
};

int env_items_length = sizeof(env_items) / sizeof(env_items[0]);
rf_camera2d camera;

// Store pointers to the multiple update camera functions
void (*camera_updaters[])(float) = {
        update_camera_center,
        update_camera_center_inside_map,
        update_camera_center_smooth_follow,
        update_camera_even_out_on_landing,
        update_camera_player_bounds_push
};

int camera_option;
int camera_updaters_length = sizeof(camera_updaters) / sizeof(camera_updaters[0]);

const char* camera_descriptions[] = {
    "Follow player center",
    "Follow player center, but clamp to map edges",
    "Follow player center; smoothed",
    "Follow player center horizontally; Update player center vertically after landing",
    "Player push camera on getting too close to screen edge"
};

static void update_player(const platform_input_state* input, float delta)
{
    if (input->keys[KEYCODE_LEFT] & KEY_HOLD_DOWN) player.position.x -= PLAYER_HOR_SPD * delta;
    if (input->keys[KEYCODE_RIGHT] & KEY_HOLD_DOWN) player.position.x += PLAYER_HOR_SPD * delta;
    if (input->keys[KEYCODE_SPACE] & KEY_RELEASE && player.can_jump)
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

static void update_camera_center(float delta)
{
    camera.offset = (rf_vec2){ window.width/2, window.height/2 };
    camera.target = player.position;
}

static void update_camera_center_inside_map(float delta)
{
    camera.target = player.position;
    camera.offset = (rf_vec2){ window.width / 2, window.height / 2 };
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

    if (max.x < window.width) camera.offset.x = window.width - (max.x - window.width/2);
    if (max.y < window.height) camera.offset.y = window.height - (max.y - window.height/2);
    if (min.x > 0) camera.offset.x = window.width/2 - min.x;
    if (min.y > 0) camera.offset.y = window.height/2 - min.y;
}

static void update_camera_center_smooth_follow(float delta)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera.offset = (rf_vec2){ window.width/2, window.height/2 };
    rf_vec2 diff = rf_vec2_sub(player.position, camera.target);
    float length = rf_vec2_len(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera.target = rf_vec2_add(camera.target, rf_vec2_scale(diff, speed*delta/length));
    }
}

static void update_camera_even_out_on_landing(float delta)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;

    camera.offset = (rf_vec2){ window.width / 2, window.height / 2 };
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

static void update_camera_player_bounds_push(float delta)
{
    static rf_vec2 bbox = { 0.2f, 0.2f };

    rf_vec2 bbox_world_min = rf_get_screen_to_world2d((rf_vec2){ (1 - bbox.x)*0.5f*window.width, (1 - bbox.y)*0.5f*window.height }, camera);
    rf_vec2 bbox_world_max = rf_get_screen_to_world2d((rf_vec2){ (1 + bbox.x)*0.5f*window.width, (1 + bbox.y)*0.5f*window.height }, camera);
    camera.offset = (rf_vec2){ (1 - bbox.x)*0.5f * window.width, (1 - bbox.y)*0.5f*window.height };

    if (player.position.x < bbox_world_min.x) camera.target.x = player.position.x;
    if (player.position.y < bbox_world_min.y) camera.target.y = player.position.y;
    if (player.position.x > bbox_world_max.x) camera.target.x = bbox_world_min.x + (player.position.x - bbox_world_max.x);
    if (player.position.y > bbox_world_max.y) camera.target.y = bbox_world_min.y + (player.position.y - bbox_world_max.y);
}

extern void game_init(rf_gfx_backend_data* gfx_data)
{
    // Initialize rayfork
    rf_gfx_init(&ctx, window.width, window.height, gfx_data);

    // Initialize the rendering batch
    batch = rf_create_default_render_batch(rf_default_allocator);
    rf_set_active_render_batch(&batch);

    player.position = (rf_vec2) { 400, 280 };
    player.speed = 0;
    player.can_jump = false;

    camera.target = player.position;
    camera.offset = (rf_vec2) { window.width / 2, window.height / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

extern void game_update(const platform_input_state* input, float delta)
{
    update_player(input, delta);

    if (input->mouse_scroll_y) camera.zoom += input->mouse_scroll_y/12;

    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

    if (input->keys[KEYCODE_R] & KEY_PRESSED_DOWN)
    {
        camera.zoom = 1.0f;
        player.position = (rf_vec2){ 400, 280 };
    }

    if (input->keys[KEYCODE_C] & KEY_PRESSED_DOWN)
    {
        camera_option = (camera_option + 1) % camera_updaters_length;
    }

    // Call update camera function by its pointer
    camera_updaters[camera_option](delta);

    // Render
    rf_begin();

        rf_clear(rf_raywhite);

        rf_begin_2d(camera);

            for (int i = 0; i < env_items_length; i++) rf_draw_rectangle_rec(env_items[i].rect, env_items[i].color);

            rf_rec playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
            rf_draw_rectangle_rec(playerRect, rf_red);

        rf_end_2d();

        rf_draw_text("Controls:", 20, 20, 10, rf_black);
        rf_draw_text("- Right/Left to move", 40, 40, 10, rf_dark_gray);
        rf_draw_text("- Space to jump", 40, 60, 10, rf_dark_gray);
        rf_draw_text("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 10, rf_dark_gray);
        rf_draw_text("- C to change camera mode", 40, 100, 10, rf_dark_gray);
        rf_draw_text("Current camera mode:", 20, 120, 10, rf_black);
        rf_draw_text(camera_descriptions[camera_option], 40, 140, 10, rf_dark_gray);

    rf_end();
}