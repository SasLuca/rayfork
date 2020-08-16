//In this file we only initialise the window using sokol_app

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#include "sokol_app.h"

#define screen_width 800
#define screen_height 450

extern void on_init(void);
extern void on_frame(void);
extern void on_cleanup(void);
extern void on_event(const sapp_event* event);

sapp_desc sokol_main(int argc, char** argv) 
{
    return (sapp_desc) 
    {
        .width = screen_width,
        .height = screen_height,
        .init_cb = on_init,
        .frame_cb = on_frame,
        .cleanup_cb = on_cleanup,
        .event_cb = on_event,
    };
}