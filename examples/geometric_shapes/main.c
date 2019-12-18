//In this file we only initialise the window using sokol_app

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#include "sokol_app.h"

void on_init(void);
void on_frame(void);
void on_cleanup(void);
void on_event(const sapp_event* event);

sapp_desc sokol_main(int argc, char** argv) 
{
    return (sapp_desc) 
    {
        .width = 640,
        .height = 480,
        .init_cb = on_init,
        .frame_cb = on_frame,
        .cleanup_cb = on_cleanup,
        .event_cb = on_event,
    };
}