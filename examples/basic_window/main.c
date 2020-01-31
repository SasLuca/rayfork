//In this file we only initialise the window using sokol_app

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#include "sokol_app.h"

void on_init(void);
void on_frame(void);
void on_resize(int width, int height);

void on_event(const sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_RESIZED)
    {
        on_resize(event->window_width, event->window_height);
    }
}

sapp_desc sokol_main(int argc, char** argv) 
{
    return (sapp_desc) 
    {
        .width = 800,
        .height = 450,
        .init_cb = on_init,
        .frame_cb = on_frame,
        .event_cb = on_event,
        .window_title = "rayfork [core] example - basic window",
    };
}