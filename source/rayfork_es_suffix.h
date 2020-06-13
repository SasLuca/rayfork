#pragma region options
#ifdef RAYFORK_IMPLEMENTATION
#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#endif

#ifndef RF_WINDOW_WIDTH
#define RF_WINDOW_WIDTH (800)
#endif

#ifndef RF_WINDOW_HEIGHT
#define RF_WINDOW_HEIGHT (450)
#endif

#ifndef RF_WINDOW_TITLE
#define RF_WINDOW_TITLE ("rayfork")
#endif
#pragma endregion

#pragma region sokol-app
#ifndef SOKOL_APP_INCLUDED
/*
    sokol_app.h -- cross-platform application wrapper

    Project URL: https://github.com/floooh/sokol

    Do this:
        #define SOKOL_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.

    In the same place define one of the following to select the 3D-API
    which should be initialized by sokol_app.h:

        #define SOKOL_GLCORE33
        #define SOKOL_GLES2
        #define SOKOL_GLES3
        #define SOKOL_D3D11
        #define SOKOL_METAL
        #define SOKOL_WGPU

    If sokol_app.h is used together with sokol_gfx.h, the selected 3D-APIs
    backend match.

    Optionally provide the following defines with your own implementations:

        SOKOL_ASSERT(c)     - your own assert macro (default: assert(c))
        SOKOL_LOG(msg)      - your own logging function (default: puts(msg))
        SOKOL_UNREACHABLE() - a guard macro for unreachable code (default: assert(false))
        SOKOL_ABORT()       - called after an unrecoverable error (default: abort())
        SOKOL_WIN32_FORCE_MAIN  - define this on Win32 to use a main() entry point instead of WinMain
        SOKOL_NO_ENTRY      - define this if sokol_app.h shouldn't "hijack" the main() function
        SOKOL_API_DECL      - public function declaration prefix (default: extern)
        SOKOL_API_IMPL      - public function implementation prefix (default: -)
        SOKOL_CALLOC        - your own calloc function (default: calloc(n, s))
        SOKOL_FREE          - your own free function (default: free(p))

    Optionally define the following to force debug checks and validations
    even in release mode:

        SOKOL_DEBUG         - by default this is defined if _DEBUG is defined

    If sokol_app.h is compiled as a DLL, define the following before
    including the declaration or implementation:

    SOKOL_DLL

    On Windows, SOKOL_DLL will define SOKOL_API_DECL as __declspec(dllexport)
    or __declspec(dllimport) as needed.

    Portions of the Windows and Linux GL initialization and event code have been
    taken from GLFW (http://www.glfw.org/)

    iOS onscreen keyboard support 'inspired' by libgdx.

    If you use sokol_app.h together with sokol_gfx.h, include both headers
    in the implementation source file, and include sokol_app.h before
    sokol_gfx.h since sokol_app.h will also include the required 3D-API
    headers.

    On Windows, a minimal 'GL header' and function loader is integrated which
    contains just enough of GL for sokol_gfx.h. If you want to use your own
    GL header-generator/loader instead, define SOKOL_WIN32_NO_GL_LOADER
    before including the implementation part of sokol_app.h.

    For example code, see https://github.com/floooh/sokol-samples/tree/master/sapp

    FEATURE OVERVIEW
    ================
    sokol_app.h provides a minimalistic cross-platform API which
    implements the 'application-wrapper' parts of a 3D application:

    - a common application entry function
    - creates a window and 3D-API context/device with a 'default framebuffer'
    - makes the rendered frame visible
    - provides keyboard-, mouse- and low-level touch-events
    - platforms: MacOS, iOS, HTML5, Win32, Linux, Android (TODO: RaspberryPi)
    - 3D-APIs: Metal, D3D11, GL3.2, GLES2, GLES3, WebGL, WebGL2

    FEATURE/PLATFORM MATRIX
    =======================
                        | Windows | macOS | Linux |  iOS  | Android | Raspi | HTML5
    --------------------+---------+-------+-------+-------+---------+-------+-------
    gl 3.x              | YES     | YES   | YES   | ---   | ---     | ---   | ---
    gles2/webgl         | ---     | ---   | ---   | YES   | YES     | TODO  | YES
    gles3/webgl2        | ---     | ---   | ---   | YES   | YES     | ---   | YES
    metal               | ---     | YES   | ---   | YES   | ---     | ---   | ---
    d3d11               | YES     | ---   | ---   | ---   | ---     | ---   | ---
    KEY_DOWN            | YES     | YES   | YES   | SOME  | TODO    | TODO  | YES
    KEY_UP              | YES     | YES   | YES   | SOME  | TODO    | TODO  | YES
    CHAR                | YES     | YES   | YES   | YES   | TODO    | TODO  | YES
    MOUSE_DOWN          | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    MOUSE_UP            | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    MOUSE_SCROLL        | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    MOUSE_MOVE          | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    MOUSE_ENTER         | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    MOUSE_LEAVE         | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    TOUCHES_BEGAN       | ---     | ---   | ---   | YES   | YES     | ---   | YES
    TOUCHES_MOVED       | ---     | ---   | ---   | YES   | YES     | ---   | YES
    TOUCHES_ENDED       | ---     | ---   | ---   | YES   | YES     | ---   | YES
    TOUCHES_CANCELLED   | ---     | ---   | ---   | YES   | YES     | ---   | YES
    RESIZED             | YES     | YES   | YES   | YES   | YES     | ---   | YES
    ICONIFIED           | YES     | YES   | YES   | ---   | ---     | ---   | ---
    RESTORED            | YES     | YES   | YES   | ---   | ---     | ---   | ---
    SUSPENDED           | ---     | ---   | ---   | YES   | YES     | ---   | TODO
    RESUMED             | ---     | ---   | ---   | YES   | YES     | ---   | TODO
    QUIT_REQUESTED      | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    UPDATE_CURSOR       | YES     | YES   | TODO  | ---   | ---     | ---   | TODO
    IME                 | TODO    | TODO? | TODO  | ???   | TODO    | ???   | ???
    key repeat flag     | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    windowed            | YES     | YES   | YES   | ---   | ---     | TODO  | YES
    fullscreen          | YES     | YES   | TODO  | YES   | YES     | TODO  | ---
    pointer lock        | TODO    | TODO  | TODO  | ---   | ---     | TODO  | TODO
    screen keyboard     | ---     | ---   | ---   | YES   | TODO    | ---   | YES
    swap interval       | YES     | YES   | YES   | YES   | TODO    | TODO  | YES
    high-dpi            | YES     | YES   | TODO  | YES   | YES     | TODO  | YES
    clipboard           | YES     | YES   | TODO  | ---   | ---     | ---   | YES

    TODO
    ====
    - Linux:
        - clipboard support
        - fullscreen support
        - show/hide mouse cursor
    - sapp_consume_event() on non-web platforms?

    STEP BY STEP
    ============
    --- Add a sokol_main() function to your code which returns a sapp_desc structure
        with initialization parameters and callback function pointers. This
        function is called very early, usually at the start of the
        platform's entry function (e.g. main or WinMain). You should do as
        little as possible here, since the rest of your code might be called
        from another thread (this depends on the platform):

            sapp_desc sokol_main(int argc, char* argv[]) {
                return (sapp_desc) {
                    .width = 640,
                    .height = 480,
                    .init_cb = my_init_func,
                    .frame_cb = my_frame_func,
                    .cleanup_cb = my_cleanup_func,
                    .event_cb = my_event_func,
                    ...
                };
            }

        There are many more setup parameters, but these are the most important.
        For a complete list search for the sapp_desc structure declaration
        below.

        DO NOT call any sokol-app function from inside sokol_main(), since
        sokol-app will not be initialized at this point.

        The .width and .height parameters are the preferred size of the 3D
        rendering canvas. The actual size may differ from this depending on
        platform and other circumstances. Also the canvas size may change at
        any time (for instance when the user resizes the application window,
        or rotates the mobile device).

        All provided function callbacks will be called from the same thread,
        but this may be different from the thread where sokol_main() was called.

        .init_cb (void (*)(void))
            This function is called once after the application window,
            3D rendering context and swap chain have been created. The
            function takes no arguments and has no return value.
        .frame_cb (void (*)(void))
            This is the per-frame callback, which is usually called 60
            times per second. This is where your application would update
            most of its state and perform all rendering.
        .cleanup_cb (void (*)(void))
            The cleanup callback is called once right before the application
            quits.
        .event_cb (void (*)(const sapp_event* event))
            The event callback is mainly for input handling, but in the
            future may also be used to communicate other types of events
            to the application. Keep the event_cb struct member zero-initialized
            if your application doesn't require event handling.
        .fail_cb (void (*)(const char* msg))
            The fail callback is called when a fatal error is encountered
            during start which doesn't allow the program to continue.
            Providing a callback here gives you a chance to show an error message
            to the user. The default behaviour is SOKOL_LOG(msg)

        As you can see, those 'standard callbacks' don't have a user_data
        argument, so any data that needs to be preserved between callbacks
        must live in global variables. If you're allergic to global variables
        or cannot use them for other reasons, an alternative set of callbacks
        can be defined in sapp_desc, together with a user_data pointer:

        .user_data (void*)
            The user-data argument for the callbacks below
        .init_userdata_cb (void (*)(void* user_data))
        .frame_userdata_cb (void (*)(void* user_data))
        .cleanup_userdata_cb (void (*)(void* user_data))
        .event_cb (void(*)(const sapp_event* event, void* user_data))
        .fail_cb (void(*)(const char* msg, void* user_data))
            These are the user-data versions of the callback functions. You
            can mix those with the standard callbacks that don't have the
            user_data argument.

        The function sapp_userdata() can be used to query the user_data
        pointer provided in the sapp_desc struct.

        You can call sapp_query_desc() to get a copy of the
        original sapp_desc structure.

        NOTE that there's also an alternative compile mode where sokol_app.h
        doesn't "hijack" the main() function. Search below for SOKOL_NO_ENTRY.

    --- Implement the initialization callback function (init_cb), this is called
        once after the rendering surface, 3D API and swap chain have been
        initialized by sokol_app. All sokol-app functions can be called
        from inside the initialization callback, the most useful functions
        at this point are:

        int sapp_width(void)
            Returns the current width of the default framebuffer, this may change
            from one frame to the next.
        int sapp_height(void)
            Likewise, returns the current height of the default framebuffer.

        int sapp_color_format(void)
        int sapp_depth_format(void)
            The color and depth-stencil pixelformats of the default framebuffer,
            as int values which are compatible with sokol-gfx's
            sg_pixel_format enum, so they can be plugged directly in places
            where sg_pixel_format is expected:

                23 == SG_PIXELFORMAT_RGBA8
                27 == SG_PIXELFORMAT_BGRA8
                41 == SG_PIXELFORMAT_DEPTH
                42 == SG_PIXELFORMAT_DEPTH_STENCIL

        int sapp_sample_count(void)
            Return the MSAA sample count of the default framebuffer.

        bool sapp_gles2(void)
            Returns true if a GLES2 or WebGL context has been created. This
            is useful when a GLES3/WebGL2 context was requested but is not
            available so that sokol_app.h had to fallback to GLES2/WebGL.

        const void* sapp_metal_get_device(void)
        const void* sapp_metal_get_renderpass_descriptor(void)
        const void* sapp_metal_get_drawable(void)
            If the Metal backend has been selected, these functions return pointers
            to various Metal API objects required for rendering, otherwise
            they return a null pointer. These void pointers are actually
            Objective-C ids converted with an ARC __bridge cast so that
            they ids can be tunnel through C code. Also note that the returned
            pointers to the renderpass-descriptor and drawable may change from one
            frame to the next, only the Metal device object is guaranteed to
            stay the same.

        const void* sapp_macos_get_window(void)
            On macOS, get the NSWindow object pointer, otherwise a null pointer.
            Before being used as Objective-C object, the void* must be converted
            back with an ARC __bridge cast.

        const void* sapp_ios_get_window(void)
            On iOS, get the UIWindow object pointer, otherwise a null pointer.
            Before being used as Objective-C object, the void* must be converted
            back with an ARC __bridge cast.

        const void* sapp_win32_get_hwnd(void)
            On Windows, get the window's HWND, otherwise a null pointer. The
            HWND has been cast to a void pointer in order to be tunneled
            through code which doesn't include Windows.h.

        const void* sapp_d3d11_get_device(void)
        const void* sapp_d3d11_get_device_context(void)
        const void* sapp_d3d11_get_render_target_view(void)
        const void* sapp_d3d11_get_depth_stencil_view(void)
            Similar to the sapp_metal_* functions, the sapp_d3d11_* functions
            return pointers to D3D11 API objects required for rendering,
            only if the D3D11 backend has been selected. Otherwise they
            return a null pointer. Note that the returned pointers to the
            render-target-view and depth-stencil-view may change from one
            frame to the next!

        const void* sapp_wgpu_get_device(void)
        const void* sapp_wgpu_get_render_view(void)
        const void* sapp_wgpu_get_resolve_view(void)
        const void* sapp_wgpu_get_depth_stencil_view(void)
            These are the WebGPU-specific functions to get the WebGPU
            objects and values required for rendering. If sokol_app.h
            is not compiled with SOKOL_WGPU, these functions return null.

        const void* sapp_android_get_native_activity(void);
            On Android, get the native activity ANativeActivity pointer, otherwise
            a null pointer.

    --- Implement the frame-callback function, this function will be called
        on the same thread as the init callback, but might be on a different
        thread than the sokol_main() function. Note that the size of
        the rendering framebuffer might have changed since the frame callback
        was called last. Call the functions sapp_width() and sapp_height()
        each frame to get the current size.

    --- Optionally implement the event-callback to handle input events.
        sokol-app provides the following type of input events:
            - a 'virtual key' was pressed down or released
            - a single text character was entered (provided as UTF-32 code point)
            - a mouse button was pressed down or released (left, right, middle)
            - mouse-wheel or 2D scrolling events
            - the mouse was moved
            - the mouse has entered or left the application window boundaries
            - low-level, portable multi-touch events (began, moved, ended, cancelled)
            - the application window was resized, iconified or restored
            - the application was suspended or restored (on mobile platforms)
            - the user or application code has asked to quit the application
            - a string was pasted to the system clipboard

        To explicitly 'consume' an event and prevent that the event is
        forwarded for further handling to the operating system, call
        sapp_consume_event() from inside the event handler (NOTE that
        this behaviour is currently only implemented for some HTML5
        events, support for other platforms and event types will
        be added as needed, please open a github ticket and/or provide
        a PR if needed).

        NOTE: Do *not* call any 3D API functions in the event callback
        function, since the 3D API context may not be active when the
        event callback is called (it may work on some platforms and
        3D APIs, but not others, and the exact behaviour may change
        between sokol-app versions).

    --- Implement the cleanup-callback function, this is called once
        after the user quits the application (see the section
        "APPLICATION QUIT" for detailed information on quitting
        behaviour, and how to intercept a pending quit (for instance to show a
        "Really Quit?" dialog box). Note that the cleanup-callback isn't
        called on the web and mobile platforms.

    CLIPBOARD SUPPORT
    =================
    Applications can send and receive UTF-8 encoded text data from and to the
    system clipboard. By default, clipboard support is disabled and
    must be enabled at startup via the following sapp_desc struct
    members:

        sapp_desc.enable_clipboard  - set to true to enable clipboard support
        sapp_desc.clipboard_size    - size of the internal clipboard buffer in bytes

    Enabling the clipboard will dynamically allocate a clipboard buffer
    for UTF-8 encoded text data of the requested size in bytes, the default
    size if 8 KBytes. Strings that don't fit into the clipboard buffer
    (including the terminating zero) will be silently clipped, so it's
    important that you provide a big enough clipboard size for your
    use case.

    To send data to the clipboard, call sapp_set_clipboard_string() with
    a pointer to an UTF-8 encoded, null-terminated C-string.

    NOTE that on the HTML5 platform, sapp_set_clipboard_string() must be
    called from inside a 'short-lived event handler', and there are a few
    other HTML5-specific caveats to workaround. You'll basically have to
    tinker until it works in all browsers :/ (maybe the situation will
    improve when all browsers agree on and implement the new
    HTML5 navigator.clipboard API).

    To get data from the clipboard, check for the SAPP_EVENTTYPE_CLIPBOARD_PASTED
    event in your event handler function, and then call sapp_get_clipboard_string()
    to obtain the updated UTF-8 encoded text.

    NOTE that behaviour of sapp_get_clipboard_string() is slightly different
    depending on platform:

        - on the HTML5 platform, the internal clipboard buffer will only be updated
          right before the SAPP_EVENTTYPE_CLIPBOARD_PASTED event is sent,
          and sapp_get_clipboard_string() will simply return the current content
          of the clipboard buffer
        - on 'native' platforms, the call to sapp_get_clipboard_string() will
          update the internal clipboard buffer with the most recent data
          from the system clipboard

    Portable code should check for the SAPP_EVENTTYPE_CLIPBOARD_PASTED event,
    and then call sapp_get_clipboard_string() right in the event handler.

    The SAPP_EVENTTYPE_CLIPBOARD_PASTED event will be generated by sokol-app
    as follows:

        - on macOS: when the Cmd+V key is pressed down
        - on HTML5: when the browser sends a 'paste' event to the global 'window' object
        - on all other platforms: when the Ctrl+V key is pressed down

    HIGH-DPI RENDERING
    ==================
    You can set the sapp_desc.high_dpi flag during initialization to request
    a full-resolution framebuffer on HighDPI displays. The default behaviour
    is sapp_desc.high_dpi=false, this means that the application will
    render to a lower-resolution framebuffer on HighDPI displays and the
    rendered content will be upscaled by the window system composer.

    In a HighDPI scenario, you still request the same window size during
    sokol_main(), but the framebuffer sizes returned by sapp_width()
    and sapp_height() will be scaled up according to the DPI scaling
    ratio. You can also get a DPI scaling factor with the function
    sapp_dpi_scale().

    Here's an example on a Mac with Retina display:

    sapp_desc sokol_main() {
        return (sapp_desc) {
            .width = 640,
            .height = 480,
            .high_dpi = true,
            ...
        };
    }

    The functions sapp_width(), sapp_height() and sapp_dpi_scale() will
    return the following values:

    sapp_width      -> 1280
    sapp_height     -> 960
    sapp_dpi_scale  -> 2.0

    If the high_dpi flag is false, or you're not running on a Retina display,
    the values would be:

    sapp_width      -> 640
    sapp_height     -> 480
    sapp_dpi_scale  -> 1.0

    APPLICATION QUIT
    ================
    Without special quit handling, a sokol_app.h application will quit
    'gracefully' when the user clicks the window close-button unless a
    platform's application model prevents this (e.g. on web or mobile).
    'Graceful exit' means that the application-provided cleanup callback will
    be called before the application quits.

    On native desktop platforms sokol_app.h provides more control over the
    application-quit-process. It's possible to initiate a 'programmatic quit'
    from the application code, and a quit initiated by the application user can
    be intercepted (for instance to show a custom dialog box).

    This 'programmatic quit protocol' is implemented trough 3 functions
    and 1 event:

        - sapp_quit(): This function simply quits the application without
          giving the user a chance to intervene. Usually this might
          be called when the user clicks the 'Ok' button in a 'Really Quit?'
          dialog box
        - sapp_request_quit(): Calling sapp_request_quit() will send the
          event SAPP_EVENTTYPE_QUIT_REQUESTED to the applications event handler
          callback, giving the user code a chance to intervene and cancel the
          pending quit process (for instance to show a 'Really Quit?' dialog
          box). If the event handler callback does nothing, the application
          will be quit as usual. To prevent this, call the function
          sapp_cancel_quit() from inside the event handler.
        - sapp_cancel_quit(): Cancels a pending quit request, either initiated
          by the user clicking the window close button, or programmatically
          by calling sapp_request_quit(). The only place where calling this
          function makes sense is from inside the event handler callback when
          the SAPP_EVENTTYPE_QUIT_REQUESTED event has been received.
        - SAPP_EVENTTYPE_QUIT_REQUESTED: this event is sent when the user
          clicks the window's close button or application code calls the
          sapp_request_quit() function. The event handler callback code can handle
          this event by calling sapp_cancel_quit() to cancel the quit.
          If the event is ignored, the application will quit as usual.

    On the web platform, the quit behaviour differs from native platforms,
    because of web-specific restrictions:

    A `programmatic quit` initiated by calling sapp_quit() or
    sapp_request_quit() will work as described above: the cleanup callback is
    called, platform-specific cleanup is performed (on the web
    this means that JS event handlers are unregisters), and then
    the request-animation-loop will be exited. However that's all. The
    web page itself will continue to exist (e.g. it's not possible to
    programmatically close the browser tab).

    On the web it's also not possible to run custom code when the user
    closes a brower tab, so it's not possible to prevent this with a
    fancy custom dialog box.

    Instead the standard "Leave Site?" dialog box can be activated (or
    deactivated) with the following function:

        sapp_html5_ask_leave_site(bool ask);

    The initial state of the associated internal flag can be provided
    at startup via sapp_desc.html5_ask_leave_site.

    This feature should only be used sparingly in critical situations - for
    instance when the user would loose data - since popping up modal dialog
    boxes is considered quite rude in the web world. Note that there's no way
    to customize the content of this dialog box or run any code as a result
    of the user's decision. Also note that the user must have interacted with
    the site before the dialog box will appear. These are all security measures
    to prevent fishing.

    The Dear ImGui HighDPI sample contains example code of how to
    implement a 'Really Quit?' dialog box with Dear ImGui (native desktop
    platforms only), and for showing the hardwired "Leave Site?" dialog box
    when running on the web platform:

        https://floooh.github.io/sokol-html5/wasm/imgui-highdpi-sapp.html

    FULLSCREEN
    ==========
    If the sapp_desc.fullscreen flag is true, sokol-app will try to create
    a fullscreen window on platforms with a 'proper' window system
    (mobile devices will always use fullscreen). The implementation details
    depend on the target platform, in general sokol-app will use a
    'soft approach' which doesn't interfere too much with the platform's
    window system (for instance borderless fullscreen window instead of
    a 'real' fullscreen mode). Such details might change over time
    as sokol-app is adapted for different needs.

    The most important effect of fullscreen mode to keep in mind is that
    the requested canvas width and height will be ignored for the initial
    window size, calling sapp_width() and sapp_height() will instead return
    the resolution of the fullscreen canvas (however the provided size
    might still be used for the non-fullscreen window, in case the user can
    switch back from fullscreen- to windowed-mode).

    To toggle fullscreen mode programmatically, call sapp_toggle_fullscreen().

    To check if the application window is currently in fullscreen mode,
    call sapp_is_fullscreen().

    ONSCREEN KEYBOARD
    =================
    On some platforms which don't provide a physical keyboard, sokol-app
    can display the platform's integrated onscreen keyboard for text
    input. To request that the onscreen keyboard is shown, call

        sapp_show_keyboard(true);

    Likewise, to hide the keyboard call:

        sapp_show_keyboard(false);

    Note that on the web platform, the keyboard can only be shown from
    inside an input handler. On such platforms, sapp_show_keyboard()
    will only work as expected when it is called from inside the
    sokol-app event callback function. When called from other places,
    an internal flag will be set, and the onscreen keyboard will be
    called at the next 'legal' opportunity (when the next input event
    is handled).

    OPTIONAL: DON'T HIJACK main() (#define SOKOL_NO_ENTRY)
    ======================================================
    In its default configuration, sokol_app.h "hijacks" the platform's
    standard main() function. This was done because different platforms
    have different main functions which are not compatible with
    C's main() (for instance WinMain on Windows has completely different
    arguments). However, this "main hijacking" posed a problem for
    usage scenarios like integrating sokol_app.h with other languages than
    C or C++, so an alternative SOKOL_NO_ENTRY mode has been added
    in which the user code provides the platform's main function:

    - define SOKOL_NO_ENTRY before including the sokol_app.h implementation
    - do *not* provide a sokol_main() function
    - instead provide the standard main() function of the platform
    - from the main function, call the function ```sapp_run()``` which
      takes a pointer to an ```sapp_desc``` structure.
    - ```sapp_run()``` takes over control and calls the provided init-, frame-,
      shutdown- and event-callbacks just like in the default model, it
      will only return when the application quits (or not at all on some
      platforms, like emscripten)

    NOTE: SOKOL_NO_ENTRY is currently not supported on Android.

    TEMP NOTE DUMP
    ==============
    - onscreen keyboard support on Android requires Java :(, should we even bother?
    - sapp_desc needs a bool whether to initialize depth-stencil surface
    - GL context initialization needs more control (at least what GL version to initialize)
    - application icon
    - the UPDATE_CURSOR event currently behaves differently between Win32 and OSX
      (Win32 sends the event each frame when the mouse moves and is inside the window
      client area, OSX sends it only once when the mouse enters the client area)
    - the Android implementation calls cleanup_cb() and destroys the egl context in onDestroy
      at the latest but should do it earlier, in onStop, as an app is "killable" after onStop
      on Android Honeycomb and later (it can't be done at the moment as the app may be started
      again after onStop and the sokol lifecycle does not yet handle context teardown/bringup)

    FIXME: ERROR HANDLING (this will need an error callback function)

    zlib/libpng license

    Copyright (c) 2018 Andre Weissflog

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.

        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.

        3. This notice may not be removed or altered from any source
        distribution.
*/
#define SOKOL_APP_INCLUDED (1)
#include <stdint.h>
#include <stdbool.h>

#ifndef SOKOL_API_DECL
#if defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_IMPL)
#define SOKOL_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(SOKOL_DLL)
#define SOKOL_API_DECL __declspec(dllimport)
#else
#define SOKOL_API_DECL extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SAPP_MAX_TOUCHPOINTS = 8,
    SAPP_MAX_MOUSEBUTTONS = 3,
    SAPP_MAX_KEYCODES = 512,
};

typedef enum sapp_event_type {
    SAPP_EVENTTYPE_INVALID,
    SAPP_EVENTTYPE_KEY_DOWN,
    SAPP_EVENTTYPE_KEY_UP,
    SAPP_EVENTTYPE_CHAR,
    SAPP_EVENTTYPE_MOUSE_DOWN,
    SAPP_EVENTTYPE_MOUSE_UP,
    SAPP_EVENTTYPE_MOUSE_SCROLL,
    SAPP_EVENTTYPE_MOUSE_MOVE,
    SAPP_EVENTTYPE_MOUSE_ENTER,
    SAPP_EVENTTYPE_MOUSE_LEAVE,
    SAPP_EVENTTYPE_TOUCHES_BEGAN,
    SAPP_EVENTTYPE_TOUCHES_MOVED,
    SAPP_EVENTTYPE_TOUCHES_ENDED,
    SAPP_EVENTTYPE_TOUCHES_CANCELLED,
    SAPP_EVENTTYPE_RESIZED,
    SAPP_EVENTTYPE_ICONIFIED,
    SAPP_EVENTTYPE_RESTORED,
    SAPP_EVENTTYPE_SUSPENDED,
    SAPP_EVENTTYPE_RESUMED,
    SAPP_EVENTTYPE_UPDATE_CURSOR,
    SAPP_EVENTTYPE_QUIT_REQUESTED,
    SAPP_EVENTTYPE_CLIPBOARD_PASTED,
    _SAPP_EVENTTYPE_NUM,
    _SAPP_EVENTTYPE_FORCE_U32 = 0x7FFFFFFF
} sapp_event_type;

/* key codes are the same names and values as GLFW */
typedef enum sapp_keycode {
    SAPP_KEYCODE_INVALID          = 0,
    SAPP_KEYCODE_SPACE            = 32,
    SAPP_KEYCODE_APOSTROPHE       = 39,  /* ' */
    SAPP_KEYCODE_COMMA            = 44,  /* , */
    SAPP_KEYCODE_MINUS            = 45,  /* - */
    SAPP_KEYCODE_PERIOD           = 46,  /* . */
    SAPP_KEYCODE_SLASH            = 47,  /* / */
    SAPP_KEYCODE_0                = 48,
    SAPP_KEYCODE_1                = 49,
    SAPP_KEYCODE_2                = 50,
    SAPP_KEYCODE_3                = 51,
    SAPP_KEYCODE_4                = 52,
    SAPP_KEYCODE_5                = 53,
    SAPP_KEYCODE_6                = 54,
    SAPP_KEYCODE_7                = 55,
    SAPP_KEYCODE_8                = 56,
    SAPP_KEYCODE_9                = 57,
    SAPP_KEYCODE_SEMICOLON        = 59,  /* ; */
    SAPP_KEYCODE_EQUAL            = 61,  /* = */
    SAPP_KEYCODE_A                = 65,
    SAPP_KEYCODE_B                = 66,
    SAPP_KEYCODE_C                = 67,
    SAPP_KEYCODE_D                = 68,
    SAPP_KEYCODE_E                = 69,
    SAPP_KEYCODE_F                = 70,
    SAPP_KEYCODE_G                = 71,
    SAPP_KEYCODE_H                = 72,
    SAPP_KEYCODE_I                = 73,
    SAPP_KEYCODE_J                = 74,
    SAPP_KEYCODE_K                = 75,
    SAPP_KEYCODE_L                = 76,
    SAPP_KEYCODE_M                = 77,
    SAPP_KEYCODE_N                = 78,
    SAPP_KEYCODE_O                = 79,
    SAPP_KEYCODE_P                = 80,
    SAPP_KEYCODE_Q                = 81,
    SAPP_KEYCODE_R                = 82,
    SAPP_KEYCODE_S                = 83,
    SAPP_KEYCODE_T                = 84,
    SAPP_KEYCODE_U                = 85,
    SAPP_KEYCODE_V                = 86,
    SAPP_KEYCODE_W                = 87,
    SAPP_KEYCODE_X                = 88,
    SAPP_KEYCODE_Y                = 89,
    SAPP_KEYCODE_Z                = 90,
    SAPP_KEYCODE_LEFT_BRACKET     = 91,  /* [ */
    SAPP_KEYCODE_BACKSLASH        = 92,  /* \ */
    SAPP_KEYCODE_RIGHT_BRACKET    = 93,  /* ] */
    SAPP_KEYCODE_GRAVE_ACCENT     = 96,  /* ` */
    SAPP_KEYCODE_WORLD_1          = 161, /* non-US #1 */
    SAPP_KEYCODE_WORLD_2          = 162, /* non-US #2 */
    SAPP_KEYCODE_ESCAPE           = 256,
    SAPP_KEYCODE_ENTER            = 257,
    SAPP_KEYCODE_TAB              = 258,
    SAPP_KEYCODE_BACKSPACE        = 259,
    SAPP_KEYCODE_INSERT           = 260,
    SAPP_KEYCODE_DELETE           = 261,
    SAPP_KEYCODE_RIGHT            = 262,
    SAPP_KEYCODE_LEFT             = 263,
    SAPP_KEYCODE_DOWN             = 264,
    SAPP_KEYCODE_UP               = 265,
    SAPP_KEYCODE_PAGE_UP          = 266,
    SAPP_KEYCODE_PAGE_DOWN        = 267,
    SAPP_KEYCODE_HOME             = 268,
    SAPP_KEYCODE_END              = 269,
    SAPP_KEYCODE_CAPS_LOCK        = 280,
    SAPP_KEYCODE_SCROLL_LOCK      = 281,
    SAPP_KEYCODE_NUM_LOCK         = 282,
    SAPP_KEYCODE_PRINT_SCREEN     = 283,
    SAPP_KEYCODE_PAUSE            = 284,
    SAPP_KEYCODE_F1               = 290,
    SAPP_KEYCODE_F2               = 291,
    SAPP_KEYCODE_F3               = 292,
    SAPP_KEYCODE_F4               = 293,
    SAPP_KEYCODE_F5               = 294,
    SAPP_KEYCODE_F6               = 295,
    SAPP_KEYCODE_F7               = 296,
    SAPP_KEYCODE_F8               = 297,
    SAPP_KEYCODE_F9               = 298,
    SAPP_KEYCODE_F10              = 299,
    SAPP_KEYCODE_F11              = 300,
    SAPP_KEYCODE_F12              = 301,
    SAPP_KEYCODE_F13              = 302,
    SAPP_KEYCODE_F14              = 303,
    SAPP_KEYCODE_F15              = 304,
    SAPP_KEYCODE_F16              = 305,
    SAPP_KEYCODE_F17              = 306,
    SAPP_KEYCODE_F18              = 307,
    SAPP_KEYCODE_F19              = 308,
    SAPP_KEYCODE_F20              = 309,
    SAPP_KEYCODE_F21              = 310,
    SAPP_KEYCODE_F22              = 311,
    SAPP_KEYCODE_F23              = 312,
    SAPP_KEYCODE_F24              = 313,
    SAPP_KEYCODE_F25              = 314,
    SAPP_KEYCODE_KP_0             = 320,
    SAPP_KEYCODE_KP_1             = 321,
    SAPP_KEYCODE_KP_2             = 322,
    SAPP_KEYCODE_KP_3             = 323,
    SAPP_KEYCODE_KP_4             = 324,
    SAPP_KEYCODE_KP_5             = 325,
    SAPP_KEYCODE_KP_6             = 326,
    SAPP_KEYCODE_KP_7             = 327,
    SAPP_KEYCODE_KP_8             = 328,
    SAPP_KEYCODE_KP_9             = 329,
    SAPP_KEYCODE_KP_DECIMAL       = 330,
    SAPP_KEYCODE_KP_DIVIDE        = 331,
    SAPP_KEYCODE_KP_MULTIPLY      = 332,
    SAPP_KEYCODE_KP_SUBTRACT      = 333,
    SAPP_KEYCODE_KP_ADD           = 334,
    SAPP_KEYCODE_KP_ENTER         = 335,
    SAPP_KEYCODE_KP_EQUAL         = 336,
    SAPP_KEYCODE_LEFT_SHIFT       = 340,
    SAPP_KEYCODE_LEFT_CONTROL     = 341,
    SAPP_KEYCODE_LEFT_ALT         = 342,
    SAPP_KEYCODE_LEFT_SUPER       = 343,
    SAPP_KEYCODE_RIGHT_SHIFT      = 344,
    SAPP_KEYCODE_RIGHT_CONTROL    = 345,
    SAPP_KEYCODE_RIGHT_ALT        = 346,
    SAPP_KEYCODE_RIGHT_SUPER      = 347,
    SAPP_KEYCODE_MENU             = 348,
} sapp_keycode;

typedef struct sapp_touchpoint {
    uintptr_t identifier;
    float pos_x;
    float pos_y;
    bool changed;
} sapp_touchpoint;

typedef enum sapp_mousebutton {
    SAPP_MOUSEBUTTON_INVALID = -1,
    SAPP_MOUSEBUTTON_LEFT = 0,
    SAPP_MOUSEBUTTON_RIGHT = 1,
    SAPP_MOUSEBUTTON_MIDDLE = 2,
} sapp_mousebutton;

enum {
    SAPP_MODIFIER_SHIFT = (1<<0),
    SAPP_MODIFIER_CTRL = (1<<1),
    SAPP_MODIFIER_ALT = (1<<2),
    SAPP_MODIFIER_SUPER = (1<<3)
};

typedef struct sapp_event {
    uint64_t frame_count;
    sapp_event_type type;
    sapp_keycode key_code;
    uint32_t char_code;
    bool key_repeat;
    uint32_t modifiers;
    sapp_mousebutton mouse_button;
    float mouse_x;
    float mouse_y;
    float scroll_x;
    float scroll_y;
    int num_touches;
    sapp_touchpoint touches[SAPP_MAX_TOUCHPOINTS];
    int window_width;
    int window_height;
    int framebuffer_width;
    int framebuffer_height;
} sapp_event;

typedef struct sapp_desc {
    void (*init_cb)(void);                  /* these are the user-provided callbacks without user data */
    void (*frame_cb)(void);
    void (*cleanup_cb)(void);
    void (*event_cb)(const sapp_event*);
    void (*fail_cb)(const char*);

    void* user_data;                        /* these are the user-provided callbacks with user data */
    void (*init_userdata_cb)(void*);
    void (*frame_userdata_cb)(void*);
    void (*cleanup_userdata_cb)(void*);
    void (*event_userdata_cb)(const sapp_event*, void*);
    void (*fail_userdata_cb)(const char*, void*);

    int width;                          /* the preferred width of the window / canvas */
    int height;                         /* the preferred height of the window / canvas */
    int sample_count;                   /* MSAA sample count */
    int swap_interval;                  /* the preferred swap interval (ignored on some platforms) */
    bool high_dpi;                      /* whether the rendering canvas is full-resolution on HighDPI displays */
    bool fullscreen;                    /* whether the window should be created in fullscreen mode */
    bool alpha;                         /* whether the framebuffer should have an alpha channel (ignored on some platforms) */
    const char* window_title;           /* the window title as UTF-8 encoded string */
    bool user_cursor;                   /* if true, user is expected to manage cursor image in SAPP_EVENTTYPE_UPDATE_CURSOR */
    bool enable_clipboard;              /* enable clipboard access, default is false */
    int clipboard_size;                 /* max size of clipboard content in bytes */

    const char* html5_canvas_name;      /* the name (id) of the HTML5 canvas element, default is "canvas" */
    bool html5_canvas_resize;           /* if true, the HTML5 canvas size is set to sapp_desc.width/height, otherwise canvas size is tracked */
    bool html5_preserve_drawing_buffer; /* HTML5 only: whether to preserve default framebuffer content between frames */
    bool html5_premultiplied_alpha;     /* HTML5 only: whether the rendered pixels use premultiplied alpha convention */
    bool html5_ask_leave_site;          /* initial state of the internal html5_ask_leave_site flag (see sapp_html5_ask_leave_site()) */
    bool ios_keyboard_resizes_canvas;   /* if true, showing the iOS keyboard shrinks the canvas */
    bool gl_force_gles2;                /* if true, setup GLES2/WebGL even if GLES3/WebGL2 is available */
} sapp_desc;

/* user-provided functions */
extern sapp_desc sokol_main(int argc, char* argv[]);

/* returns true after sokol-app has been initialized */
SOKOL_API_DECL bool sapp_isvalid(void);
/* returns the current framebuffer width in pixels */
SOKOL_API_DECL int sapp_width(void);
/* returns the current framebuffer height in pixels */
SOKOL_API_DECL int sapp_height(void);
/* get default framebuffer color pixel format */
SOKOL_API_DECL int sapp_color_format(void);
/* get default framebuffer depth pixel format */
SOKOL_API_DECL int sapp_depth_format(void);
/* get default framebuffer sample count */
SOKOL_API_DECL int sapp_sample_count(void);
/* returns true when high_dpi was requested and actually running in a high-dpi scenario */
SOKOL_API_DECL bool sapp_high_dpi(void);
/* returns the dpi scaling factor (window pixels to framebuffer pixels) */
SOKOL_API_DECL float sapp_dpi_scale(void);
/* show or hide the mobile device onscreen keyboard */
SOKOL_API_DECL void sapp_show_keyboard(bool visible);
/* return true if the mobile device onscreen keyboard is currently shown */
SOKOL_API_DECL bool sapp_keyboard_shown(void);
/* query fullscreen mode */
SOKOL_API_DECL bool sapp_is_fullscreen(void);
/* toggle fullscreen mode */
SOKOL_API_DECL void sapp_toggle_fullscreen(void);
/* show or hide the mouse cursor */
SOKOL_API_DECL void sapp_show_mouse(bool visible);
/* show or hide the mouse cursor */
SOKOL_API_DECL bool sapp_mouse_shown();
/* return the userdata pointer optionally provided in sapp_desc */
SOKOL_API_DECL void* sapp_userdata(void);
/* return a copy of the sapp_desc structure */
SOKOL_API_DECL sapp_desc sapp_query_desc(void);
/* initiate a "soft quit" (sends SAPP_EVENTTYPE_QUIT_REQUESTED) */
SOKOL_API_DECL void sapp_request_quit(void);
/* cancel a pending quit (when SAPP_EVENTTYPE_QUIT_REQUESTED has been received) */
SOKOL_API_DECL void sapp_cancel_quit(void);
/* initiate a "hard quit" (quit application without sending SAPP_EVENTTYPE_QUIT_REQUSTED) */
SOKOL_API_DECL void sapp_quit(void);
/* call from inside event callback to consume the current event (don't forward to platform) */
SOKOL_API_DECL void sapp_consume_event(void);
/* get the current frame counter (for comparison with sapp_event.frame_count) */
SOKOL_API_DECL uint64_t sapp_frame_count(void);
/* write string into clipboard */
SOKOL_API_DECL void sapp_set_clipboard_string(const char* str);
/* read string from clipboard (usually during SAPP_EVENTTYPE_CLIPBOARD_PASTED) */
SOKOL_API_DECL const char* sapp_get_clipboard_string(void);

/* special run-function for SOKOL_NO_ENTRY (in standard mode this is an empty stub) */
SOKOL_API_DECL int sapp_run(const sapp_desc* desc);

/* GL: return true when GLES2 fallback is active (to detect fallback from GLES3) */
SOKOL_API_DECL bool sapp_gles2(void);

/* HTML5: enable or disable the hardwired "Leave Site?" dialog box */
SOKOL_API_DECL void sapp_html5_ask_leave_site(bool ask);

/* Metal: get ARC-bridged pointer to Metal device object */
SOKOL_API_DECL const void* sapp_metal_get_device(void);
/* Metal: get ARC-bridged pointer to this frame's renderpass descriptor */
SOKOL_API_DECL const void* sapp_metal_get_renderpass_descriptor(void);
/* Metal: get ARC-bridged pointer to current drawable */
SOKOL_API_DECL const void* sapp_metal_get_drawable(void);
/* macOS: get ARC-bridged pointer to macOS NSWindow */
SOKOL_API_DECL const void* sapp_macos_get_window(void);
/* iOS: get ARC-bridged pointer to iOS UIWindow */
SOKOL_API_DECL const void* sapp_ios_get_window(void);

/* D3D11: get pointer to ID3D11Device object */
SOKOL_API_DECL const void* sapp_d3d11_get_device(void);
/* D3D11: get pointer to ID3D11DeviceContext object */
SOKOL_API_DECL const void* sapp_d3d11_get_device_context(void);
/* D3D11: get pointer to ID3D11RenderTargetView object */
SOKOL_API_DECL const void* sapp_d3d11_get_render_target_view(void);
/* D3D11: get pointer to ID3D11DepthStencilView */
SOKOL_API_DECL const void* sapp_d3d11_get_depth_stencil_view(void);
/* Win32: get the HWND window handle */
SOKOL_API_DECL const void* sapp_win32_get_hwnd(void);

/* WebGPU: get WGPUDevice handle */
SOKOL_API_DECL const void* sapp_wgpu_get_device(void);
/* WebGPU: get swapchain's WGPUTextureView handle for rendering */
SOKOL_API_DECL const void* sapp_wgpu_get_render_view(void);
/* WebGPU: get swapchain's MSAA-resolve WGPUTextureView (may return null) */
SOKOL_API_DECL const void* sapp_wgpu_get_resolve_view(void);
/* WebGPU: get swapchain's WGPUTextureView for the depth-stencil surface */
SOKOL_API_DECL const void* sapp_wgpu_get_depth_stencil_view(void);

/* Android: get native activity handle */
SOKOL_API_DECL const void* sapp_android_get_native_activity(void);

#ifdef __cplusplus
} /* extern "C" */

/* reference-based equivalents for C++ */
inline int sapp_run(const sapp_desc& desc) { return sapp_run(&desc); }

#endif
#endif // SOKOL_APP_INCLUDED

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef SOKOL_IMPL
#define SOKOL_APP_IMPL_INCLUDED (1)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4201)   /* nonstandard extension used: nameless struct/union */
#pragma warning(disable:4115)   /* named type definition in parentheses */
#pragma warning(disable:4054)   /* 'type cast': from function pointer */
#pragma warning(disable:4055)   /* 'type cast': from data pointer */
#pragma warning(disable:4505)   /* unreferenced local function has been removed */
#pragma warning(disable:4115)   /* /W4: 'ID3D11ModuleInstance': named type definition in parentheses (in d3d11.h) */
#endif

#include <string.h> /* memset */

/* check if the config defines are alright */
#if defined(__APPLE__)
    #if !__has_feature(objc_arc)
        #error "sokol_app.h requires ARC (Automatic Reference Counting) on MacOS and iOS"
    #endif
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        /* iOS */
        #if !defined(SOKOL_METAL) && !defined(SOKOL_GLES3)
        #error("sokol_app.h: unknown 3D API selected for iOS, must be SOKOL_METAL or SOKOL_GLES3")
        #endif
    #else
        /* MacOS */
        #if !defined(SOKOL_METAL) && !defined(SOKOL_GLCORE33)
        #error("sokol_app.h: unknown 3D API selected for MacOS, must be SOKOL_METAL or SOKOL_GLCORE33")
        #endif
    #endif
#elif defined(__EMSCRIPTEN__)
    /* emscripten (asm.js or wasm) */
    #if !defined(SOKOL_GLES3) && !defined(SOKOL_GLES2) && !defined(SOKOL_WGPU)
    #error("sokol_app.h: unknown 3D API selected for emscripten, must be SOKOL_GLES3, SOKOL_GLES2 or SOKOL_WGPU")
    #endif
#elif defined(_WIN32)
    /* Windows (D3D11 or GL) */
    #if !defined(SOKOL_D3D11) && !defined(SOKOL_GLCORE33)
    #error("sokol_app.h: unknown 3D API selected for Win32, must be SOKOL_D3D11 or SOKOL_GLCORE33")
    #endif
#elif defined(__ANDROID__)
    /* Android */
    #if !defined(SOKOL_GLES3) && !defined(SOKOL_GLES2)
    #error("sokol_app.h: unknown 3D API selected for Android, must be SOKOL_GLES3 or SOKOL_GLES2")
    #endif
    #if defined(SOKOL_NO_ENTRY)
    #error("sokol_app.h: SOKOL_NO_ENTRY is not supported on Android")
    #endif
#elif defined(__linux__) || defined(__unix__)
    /* Linux */
    #if !defined(SOKOL_GLCORE33)
    #error("sokol_app.h: unknown 3D API selected for Linux, must be SOKOL_GLCORE33")
    #endif
#else
#error "sokol_app.h: Unknown platform"
#endif

#ifndef SOKOL_API_IMPL
    #define SOKOL_API_IMPL
#endif
#ifndef SOKOL_DEBUG
    #ifndef NDEBUG
        #define SOKOL_DEBUG (1)
    #endif
#endif
#ifndef SOKOL_ASSERT
    #include <assert.h>
    #define SOKOL_ASSERT(c) assert(c)
#endif
#ifndef SOKOL_UNREACHABLE
    #define SOKOL_UNREACHABLE SOKOL_ASSERT(false)
#endif
#if !defined(SOKOL_CALLOC) || !defined(SOKOL_FREE)
    #include <stdlib.h>
#endif
#if !defined(SOKOL_CALLOC)
    #define SOKOL_CALLOC(n,s) calloc(n,s)
#endif
#if !defined(SOKOL_FREE)
    #define SOKOL_FREE(p) free(p)
#endif
#ifndef SOKOL_LOG
    #ifdef SOKOL_DEBUG
        #if defined(__ANDROID__)
            #include <android/log.h>
            #define SOKOL_LOG(s) { SOKOL_ASSERT(s); __android_log_write(ANDROID_LOG_INFO, "SOKOL_APP", s); }
        #else
            #include <stdio.h>
            #define SOKOL_LOG(s) { SOKOL_ASSERT(s); puts(s); }
        #endif
    #else
        #define SOKOL_LOG(s)
    #endif
#endif
#ifndef SOKOL_ABORT
    #include <stdlib.h>
    #define SOKOL_ABORT() abort()
#endif
#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif
#ifndef _SOKOL_UNUSED
    #define _SOKOL_UNUSED(x) (void)(x)
#endif

/* helper macros */
#define _sapp_def(val, def) (((val) == 0) ? (def) : (val))
#define _sapp_absf(a) (((a)<0.0f)?-(a):(a))

#define _SAPP_MAX_TITLE_LENGTH (128)
/* NOTE: the pixel format values *must* be compatible with sg_pixel_format */
#define _SAPP_PIXELFORMAT_RGBA8 (23)
#define _SAPP_PIXELFORMAT_BGRA8 (27)
#define _SAPP_PIXELFORMAT_DEPTH (41)
#define _SAPP_PIXELFORMAT_DEPTH_STENCIL (42)

typedef struct {
    bool valid;
    int window_width;
    int window_height;
    int framebuffer_width;
    int framebuffer_height;
    int sample_count;
    int swap_interval;
    float dpi_scale;
    bool fullscreen;
    bool gles2_fallback;
    bool first_frame;
    bool init_called;
    bool cleanup_called;
    bool quit_requested;
    bool quit_ordered;
    bool event_consumed;
    bool html5_ask_leave_site;
    char html5_canvas_name[_SAPP_MAX_TITLE_LENGTH];
    char window_title[_SAPP_MAX_TITLE_LENGTH];      /* UTF-8 */
    wchar_t window_title_wide[_SAPP_MAX_TITLE_LENGTH];   /* UTF-32 or UCS-2 */
    uint64_t frame_count;
    float mouse_x;
    float mouse_y;
    bool win32_mouse_tracked;
    bool onscreen_keyboard_shown;
    sapp_event event;
    sapp_desc desc;
    sapp_keycode keycodes[SAPP_MAX_KEYCODES];
    bool clipboard_enabled;
    int clipboard_size;
    char* clipboard;
} _sapp_state;
static _sapp_state _sapp;

_SOKOL_PRIVATE void _sapp_fail(const char* msg) {
    if (_sapp.desc.fail_cb) {
        _sapp.desc.fail_cb(msg);
    }
    else if (_sapp.desc.fail_userdata_cb) {
        _sapp.desc.fail_userdata_cb(msg, _sapp.desc.user_data);
    }
    else {
        SOKOL_LOG(msg);
    }
    SOKOL_ABORT();
}

_SOKOL_PRIVATE void _sapp_call_init(void) {
    if (_sapp.desc.init_cb) {
        _sapp.desc.init_cb();
    }
    else if (_sapp.desc.init_userdata_cb) {
        _sapp.desc.init_userdata_cb(_sapp.desc.user_data);
    }
    _sapp.init_called = true;
}

_SOKOL_PRIVATE void _sapp_call_frame(void) {
    if (_sapp.init_called && !_sapp.cleanup_called) {
        if (_sapp.desc.frame_cb) {
            _sapp.desc.frame_cb();
        }
        else if (_sapp.desc.frame_userdata_cb) {
            _sapp.desc.frame_userdata_cb(_sapp.desc.user_data);
        }
    }
}

_SOKOL_PRIVATE void _sapp_call_cleanup(void) {
    if (!_sapp.cleanup_called) {
        if (_sapp.desc.cleanup_cb) {
            _sapp.desc.cleanup_cb();
        }
        else if (_sapp.desc.cleanup_userdata_cb) {
            _sapp.desc.cleanup_userdata_cb(_sapp.desc.user_data);
        }
        _sapp.cleanup_called = true;
    }
}

_SOKOL_PRIVATE bool _sapp_call_event(const sapp_event* e) {
    if (!_sapp.cleanup_called) {
        if (_sapp.desc.event_cb) {
            _sapp.desc.event_cb(e);
        }
        else if (_sapp.desc.event_userdata_cb) {
            _sapp.desc.event_userdata_cb(e, _sapp.desc.user_data);
        }
    }
    if (_sapp.event_consumed) {
        _sapp.event_consumed = false;
        return true;
    }
    else {
        return false;
    }
}

_SOKOL_PRIVATE void _sapp_strcpy(const char* src, char* dst, int max_len) {
    SOKOL_ASSERT(src && dst && (max_len > 0));
    char* const end = &(dst[max_len-1]);
    char c = 0;
    for (int i = 0; i < max_len; i++) {
        c = *src;
        if (c != 0) {
            src++;
        }
        *dst++ = c;
    }
    /* truncated? */
    if (c != 0) {
        *end = 0;
    }
}

_SOKOL_PRIVATE sapp_desc _sapp_desc_defaults(const sapp_desc* in_desc) {
    sapp_desc desc = *in_desc;
    desc.width = _sapp_def(desc.width, 640);
    desc.height = _sapp_def(desc.height, 480);
    desc.sample_count = _sapp_def(desc.sample_count, 1);
    desc.swap_interval = _sapp_def(desc.swap_interval, 1);
    desc.html5_canvas_name = _sapp_def(desc.html5_canvas_name, "canvas");
    desc.clipboard_size = _sapp_def(desc.clipboard_size, 8192);
    desc.window_title = _sapp_def(desc.window_title, "sokol_app");
    return desc;
}

_SOKOL_PRIVATE void _sapp_init_state(const sapp_desc* desc) {
    memset(&_sapp, 0, sizeof(_sapp));
    _sapp.desc = _sapp_desc_defaults(desc);
    _sapp.first_frame = true;
    _sapp.window_width = _sapp.desc.width;
    _sapp.window_height = _sapp.desc.height;
    _sapp.framebuffer_width = _sapp.window_width;
    _sapp.framebuffer_height = _sapp.window_height;
    _sapp.sample_count = _sapp.desc.sample_count;
    _sapp.swap_interval = _sapp.desc.swap_interval;
    _sapp_strcpy(_sapp.desc.html5_canvas_name, _sapp.html5_canvas_name, sizeof(_sapp.html5_canvas_name));
    _sapp.desc.html5_canvas_name = _sapp.html5_canvas_name;
    _sapp.html5_ask_leave_site = _sapp.desc.html5_ask_leave_site;
    _sapp.clipboard_enabled = _sapp.desc.enable_clipboard;
    if (_sapp.clipboard_enabled) {
        _sapp.clipboard_size = _sapp.desc.clipboard_size;
        _sapp.clipboard = (char*) SOKOL_CALLOC(1, _sapp.clipboard_size);
    }
    _sapp_strcpy(_sapp.desc.window_title, _sapp.window_title, sizeof(_sapp.window_title));
    _sapp.desc.window_title = _sapp.window_title;
    _sapp.dpi_scale = 1.0f;
    _sapp.fullscreen = _sapp.desc.fullscreen;
}

_SOKOL_PRIVATE void _sapp_discard_state(void) {
    if (_sapp.clipboard_enabled) {
        SOKOL_ASSERT(_sapp.clipboard);
        SOKOL_FREE((void*)_sapp.clipboard);
    }
    memset(&_sapp, 0, sizeof(_sapp));
}

_SOKOL_PRIVATE void _sapp_init_event(sapp_event_type type) {
    memset(&_sapp.event, 0, sizeof(_sapp.event));
    _sapp.event.type = type;
    _sapp.event.frame_count = _sapp.frame_count;
    _sapp.event.mouse_button = SAPP_MOUSEBUTTON_INVALID;
    _sapp.event.window_width = _sapp.window_width;
    _sapp.event.window_height = _sapp.window_height;
    _sapp.event.framebuffer_width = _sapp.framebuffer_width;
    _sapp.event.framebuffer_height = _sapp.framebuffer_height;
}

_SOKOL_PRIVATE bool _sapp_events_enabled(void) {
    /* only send events when an event callback is set, and the init function was called */
    return (_sapp.desc.event_cb || _sapp.desc.event_userdata_cb) && _sapp.init_called;
}

_SOKOL_PRIVATE sapp_keycode _sapp_translate_key(int scan_code) {
    if ((scan_code >= 0) && (scan_code < SAPP_MAX_KEYCODES)) {
        return _sapp.keycodes[scan_code];
    }
    else {
        return SAPP_KEYCODE_INVALID;
    }
}

_SOKOL_PRIVATE void _sapp_frame(void) {
    if (_sapp.first_frame) {
        _sapp.first_frame = false;
        _sapp_call_init();
    }
    _sapp_call_frame();
    _sapp.frame_count++;
}

/*== MacOS/iOS ===============================================================*/

#if defined(__APPLE__)

/*== MacOS ===================================================================*/
#if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE

#if defined(SOKOL_METAL)
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#elif defined(SOKOL_GLCORE33)
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <Cocoa/Cocoa.h>
#include <OpenGL/gl3.h>
#endif

@interface _sapp_macos_app_delegate : NSObject<NSApplicationDelegate>
@end
@interface _sapp_macos_window_delegate : NSObject<NSWindowDelegate>
@end
#if defined(SOKOL_METAL)
@interface _sapp_macos_mtk_view_dlg : NSObject<MTKViewDelegate>
@end
@interface _sapp_macos_view : MTKView
{
    NSTrackingArea* trackingArea;
}
@end
#elif defined(SOKOL_GLCORE33)
@interface _sapp_macos_view : NSOpenGLView
{
    NSTrackingArea* trackingArea;
}
- (void)timerFired:(id)sender;
- (void)prepareOpenGL;
- (void)drawRect:(NSRect)bounds;
@end
#endif

static NSWindow* _sapp_macos_window_obj;
static _sapp_macos_window_delegate* _sapp_macos_win_dlg_obj;
static _sapp_macos_app_delegate* _sapp_macos_app_dlg_obj;
static _sapp_macos_view* _sapp_view_obj;
#if defined(SOKOL_METAL)
static _sapp_macos_mtk_view_dlg* _sapp_macos_mtk_view_dlg_obj;
static id<MTLDevice> _sapp_mtl_device_obj;
#elif defined(SOKOL_GLCORE33)
static NSOpenGLPixelFormat* _sapp_macos_glpixelformat_obj;
static NSTimer* _sapp_macos_timer_obj;
#endif
static uint32_t _sapp_macos_flags_changed_store;

_SOKOL_PRIVATE void _sapp_macos_init_keytable(void) {
    _sapp.keycodes[0x1D] = SAPP_KEYCODE_0;
    _sapp.keycodes[0x12] = SAPP_KEYCODE_1;
    _sapp.keycodes[0x13] = SAPP_KEYCODE_2;
    _sapp.keycodes[0x14] = SAPP_KEYCODE_3;
    _sapp.keycodes[0x15] = SAPP_KEYCODE_4;
    _sapp.keycodes[0x17] = SAPP_KEYCODE_5;
    _sapp.keycodes[0x16] = SAPP_KEYCODE_6;
    _sapp.keycodes[0x1A] = SAPP_KEYCODE_7;
    _sapp.keycodes[0x1C] = SAPP_KEYCODE_8;
    _sapp.keycodes[0x19] = SAPP_KEYCODE_9;
    _sapp.keycodes[0x00] = SAPP_KEYCODE_A;
    _sapp.keycodes[0x0B] = SAPP_KEYCODE_B;
    _sapp.keycodes[0x08] = SAPP_KEYCODE_C;
    _sapp.keycodes[0x02] = SAPP_KEYCODE_D;
    _sapp.keycodes[0x0E] = SAPP_KEYCODE_E;
    _sapp.keycodes[0x03] = SAPP_KEYCODE_F;
    _sapp.keycodes[0x05] = SAPP_KEYCODE_G;
    _sapp.keycodes[0x04] = SAPP_KEYCODE_H;
    _sapp.keycodes[0x22] = SAPP_KEYCODE_I;
    _sapp.keycodes[0x26] = SAPP_KEYCODE_J;
    _sapp.keycodes[0x28] = SAPP_KEYCODE_K;
    _sapp.keycodes[0x25] = SAPP_KEYCODE_L;
    _sapp.keycodes[0x2E] = SAPP_KEYCODE_M;
    _sapp.keycodes[0x2D] = SAPP_KEYCODE_N;
    _sapp.keycodes[0x1F] = SAPP_KEYCODE_O;
    _sapp.keycodes[0x23] = SAPP_KEYCODE_P;
    _sapp.keycodes[0x0C] = SAPP_KEYCODE_Q;
    _sapp.keycodes[0x0F] = SAPP_KEYCODE_R;
    _sapp.keycodes[0x01] = SAPP_KEYCODE_S;
    _sapp.keycodes[0x11] = SAPP_KEYCODE_T;
    _sapp.keycodes[0x20] = SAPP_KEYCODE_U;
    _sapp.keycodes[0x09] = SAPP_KEYCODE_V;
    _sapp.keycodes[0x0D] = SAPP_KEYCODE_W;
    _sapp.keycodes[0x07] = SAPP_KEYCODE_X;
    _sapp.keycodes[0x10] = SAPP_KEYCODE_Y;
    _sapp.keycodes[0x06] = SAPP_KEYCODE_Z;
    _sapp.keycodes[0x27] = SAPP_KEYCODE_APOSTROPHE;
    _sapp.keycodes[0x2A] = SAPP_KEYCODE_BACKSLASH;
    _sapp.keycodes[0x2B] = SAPP_KEYCODE_COMMA;
    _sapp.keycodes[0x18] = SAPP_KEYCODE_EQUAL;
    _sapp.keycodes[0x32] = SAPP_KEYCODE_GRAVE_ACCENT;
    _sapp.keycodes[0x21] = SAPP_KEYCODE_LEFT_BRACKET;
    _sapp.keycodes[0x1B] = SAPP_KEYCODE_MINUS;
    _sapp.keycodes[0x2F] = SAPP_KEYCODE_PERIOD;
    _sapp.keycodes[0x1E] = SAPP_KEYCODE_RIGHT_BRACKET;
    _sapp.keycodes[0x29] = SAPP_KEYCODE_SEMICOLON;
    _sapp.keycodes[0x2C] = SAPP_KEYCODE_SLASH;
    _sapp.keycodes[0x0A] = SAPP_KEYCODE_WORLD_1;
    _sapp.keycodes[0x33] = SAPP_KEYCODE_BACKSPACE;
    _sapp.keycodes[0x39] = SAPP_KEYCODE_CAPS_LOCK;
    _sapp.keycodes[0x75] = SAPP_KEYCODE_DELETE;
    _sapp.keycodes[0x7D] = SAPP_KEYCODE_DOWN;
    _sapp.keycodes[0x77] = SAPP_KEYCODE_END;
    _sapp.keycodes[0x24] = SAPP_KEYCODE_ENTER;
    _sapp.keycodes[0x35] = SAPP_KEYCODE_ESCAPE;
    _sapp.keycodes[0x7A] = SAPP_KEYCODE_F1;
    _sapp.keycodes[0x78] = SAPP_KEYCODE_F2;
    _sapp.keycodes[0x63] = SAPP_KEYCODE_F3;
    _sapp.keycodes[0x76] = SAPP_KEYCODE_F4;
    _sapp.keycodes[0x60] = SAPP_KEYCODE_F5;
    _sapp.keycodes[0x61] = SAPP_KEYCODE_F6;
    _sapp.keycodes[0x62] = SAPP_KEYCODE_F7;
    _sapp.keycodes[0x64] = SAPP_KEYCODE_F8;
    _sapp.keycodes[0x65] = SAPP_KEYCODE_F9;
    _sapp.keycodes[0x6D] = SAPP_KEYCODE_F10;
    _sapp.keycodes[0x67] = SAPP_KEYCODE_F11;
    _sapp.keycodes[0x6F] = SAPP_KEYCODE_F12;
    _sapp.keycodes[0x69] = SAPP_KEYCODE_F13;
    _sapp.keycodes[0x6B] = SAPP_KEYCODE_F14;
    _sapp.keycodes[0x71] = SAPP_KEYCODE_F15;
    _sapp.keycodes[0x6A] = SAPP_KEYCODE_F16;
    _sapp.keycodes[0x40] = SAPP_KEYCODE_F17;
    _sapp.keycodes[0x4F] = SAPP_KEYCODE_F18;
    _sapp.keycodes[0x50] = SAPP_KEYCODE_F19;
    _sapp.keycodes[0x5A] = SAPP_KEYCODE_F20;
    _sapp.keycodes[0x73] = SAPP_KEYCODE_HOME;
    _sapp.keycodes[0x72] = SAPP_KEYCODE_INSERT;
    _sapp.keycodes[0x7B] = SAPP_KEYCODE_LEFT;
    _sapp.keycodes[0x3A] = SAPP_KEYCODE_LEFT_ALT;
    _sapp.keycodes[0x3B] = SAPP_KEYCODE_LEFT_CONTROL;
    _sapp.keycodes[0x38] = SAPP_KEYCODE_LEFT_SHIFT;
    _sapp.keycodes[0x37] = SAPP_KEYCODE_LEFT_SUPER;
    _sapp.keycodes[0x6E] = SAPP_KEYCODE_MENU;
    _sapp.keycodes[0x47] = SAPP_KEYCODE_NUM_LOCK;
    _sapp.keycodes[0x79] = SAPP_KEYCODE_PAGE_DOWN;
    _sapp.keycodes[0x74] = SAPP_KEYCODE_PAGE_UP;
    _sapp.keycodes[0x7C] = SAPP_KEYCODE_RIGHT;
    _sapp.keycodes[0x3D] = SAPP_KEYCODE_RIGHT_ALT;
    _sapp.keycodes[0x3E] = SAPP_KEYCODE_RIGHT_CONTROL;
    _sapp.keycodes[0x3C] = SAPP_KEYCODE_RIGHT_SHIFT;
    _sapp.keycodes[0x36] = SAPP_KEYCODE_RIGHT_SUPER;
    _sapp.keycodes[0x31] = SAPP_KEYCODE_SPACE;
    _sapp.keycodes[0x30] = SAPP_KEYCODE_TAB;
    _sapp.keycodes[0x7E] = SAPP_KEYCODE_UP;
    _sapp.keycodes[0x52] = SAPP_KEYCODE_KP_0;
    _sapp.keycodes[0x53] = SAPP_KEYCODE_KP_1;
    _sapp.keycodes[0x54] = SAPP_KEYCODE_KP_2;
    _sapp.keycodes[0x55] = SAPP_KEYCODE_KP_3;
    _sapp.keycodes[0x56] = SAPP_KEYCODE_KP_4;
    _sapp.keycodes[0x57] = SAPP_KEYCODE_KP_5;
    _sapp.keycodes[0x58] = SAPP_KEYCODE_KP_6;
    _sapp.keycodes[0x59] = SAPP_KEYCODE_KP_7;
    _sapp.keycodes[0x5B] = SAPP_KEYCODE_KP_8;
    _sapp.keycodes[0x5C] = SAPP_KEYCODE_KP_9;
    _sapp.keycodes[0x45] = SAPP_KEYCODE_KP_ADD;
    _sapp.keycodes[0x41] = SAPP_KEYCODE_KP_DECIMAL;
    _sapp.keycodes[0x4B] = SAPP_KEYCODE_KP_DIVIDE;
    _sapp.keycodes[0x4C] = SAPP_KEYCODE_KP_ENTER;
    _sapp.keycodes[0x51] = SAPP_KEYCODE_KP_EQUAL;
    _sapp.keycodes[0x43] = SAPP_KEYCODE_KP_MULTIPLY;
    _sapp.keycodes[0x4E] = SAPP_KEYCODE_KP_SUBTRACT;
}

_SOKOL_PRIVATE void _sapp_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    _sapp_macos_init_keytable();
    [NSApplication sharedApplication];
    NSApp.activationPolicy = NSApplicationActivationPolicyRegular;
    _sapp_macos_app_dlg_obj = [[_sapp_macos_app_delegate alloc] init];
    NSApp.delegate = _sapp_macos_app_dlg_obj;
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
    _sapp_discard_state();
}

/* MacOS entry function */
#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */

_SOKOL_PRIVATE void _sapp_macos_update_dimensions(void) {
    #if defined(SOKOL_METAL)
        const CGSize fb_size = [_sapp_view_obj drawableSize];
        _sapp.framebuffer_width = fb_size.width;
        _sapp.framebuffer_height = fb_size.height;
    #elif defined(SOKOL_GLCORE33)
        const NSRect fb_rect = [_sapp_view_obj convertRectToBacking:[_sapp_view_obj frame]];
        _sapp.framebuffer_width = fb_rect.size.width;
        _sapp.framebuffer_height = fb_rect.size.height;
    #endif
    const NSRect bounds = [_sapp_view_obj bounds];
    _sapp.window_width = bounds.size.width;
    _sapp.window_height = bounds.size.height;
    SOKOL_ASSERT((_sapp.framebuffer_width > 0) && (_sapp.framebuffer_height > 0));
    _sapp.dpi_scale = (float)_sapp.framebuffer_width / (float)_sapp.window_width;
}

_SOKOL_PRIVATE void _sapp_macos_frame(void) {
    const NSPoint mouse_pos = [_sapp_macos_window_obj mouseLocationOutsideOfEventStream];
    _sapp.mouse_x = mouse_pos.x * _sapp.dpi_scale;
    _sapp.mouse_y = _sapp.framebuffer_height - (mouse_pos.y * _sapp.dpi_scale) - 1;
    _sapp_frame();
    if (_sapp.quit_requested || _sapp.quit_ordered) {
        [_sapp_macos_window_obj performClose:nil];
    }
}

_SOKOL_PRIVATE void _sapp_macos_toggle_fullscreen(void) {
    /* NOTE: the _sapp.fullscreen flag is also notified by the
       windowDidEnterFullscreen / windowDidExitFullscreen
       event handlers
    */
    _sapp.fullscreen = !_sapp.fullscreen;
    [_sapp_macos_window_obj toggleFullScreen:nil];
}

@implementation _sapp_macos_app_delegate
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
    _SOKOL_UNUSED(aNotification);
    if (_sapp.fullscreen) {
        NSRect screen_rect = NSScreen.mainScreen.frame;
        _sapp.window_width = screen_rect.size.width;
        _sapp.window_height = screen_rect.size.height;
        if (_sapp.desc.high_dpi) {
            _sapp.framebuffer_width = 2 * _sapp.window_width;
            _sapp.framebuffer_height = 2 * _sapp.window_height;
        }
        else {
            _sapp.framebuffer_width = _sapp.window_width;
            _sapp.framebuffer_height = _sapp.window_height;
        }
        _sapp.dpi_scale = (float)_sapp.framebuffer_width / (float) _sapp.window_width;
    }
    const NSUInteger style =
        NSWindowStyleMaskTitled |
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable;
    NSRect window_rect = NSMakeRect(0, 0, _sapp.window_width, _sapp.window_height);
    _sapp_macos_window_obj = [[NSWindow alloc]
        initWithContentRect:window_rect
        styleMask:style
        backing:NSBackingStoreBuffered
        defer:NO];
    _sapp_macos_window_obj.title = [NSString stringWithUTF8String:_sapp.window_title];
    _sapp_macos_window_obj.acceptsMouseMovedEvents = YES;
    _sapp_macos_window_obj.restorable = YES;
    _sapp_macos_win_dlg_obj = [[_sapp_macos_window_delegate alloc] init];
    _sapp_macos_window_obj.delegate = _sapp_macos_win_dlg_obj;
    #if defined(SOKOL_METAL)
        _sapp_mtl_device_obj = MTLCreateSystemDefaultDevice();
        _sapp_macos_mtk_view_dlg_obj = [[_sapp_macos_mtk_view_dlg alloc] init];
        _sapp_view_obj = [[_sapp_macos_view alloc] init];
        [_sapp_view_obj updateTrackingAreas];
        _sapp_view_obj.preferredFramesPerSecond = 60 / _sapp.swap_interval;
        _sapp_view_obj.delegate = _sapp_macos_mtk_view_dlg_obj;
        _sapp_view_obj.device = _sapp_mtl_device_obj;
        _sapp_view_obj.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
        _sapp_view_obj.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        _sapp_view_obj.sampleCount = _sapp.sample_count;
        _sapp_macos_window_obj.contentView = _sapp_view_obj;
        [_sapp_macos_window_obj makeFirstResponder:_sapp_view_obj];
        if (!_sapp.desc.high_dpi) {
            CGSize drawable_size = { (CGFloat) _sapp.framebuffer_width, (CGFloat) _sapp.framebuffer_height };
            _sapp_view_obj.drawableSize = drawable_size;
        }
        _sapp_macos_update_dimensions();
        _sapp_view_obj.layer.magnificationFilter = kCAFilterNearest;
    #elif defined(SOKOL_GLCORE33)
        NSOpenGLPixelFormatAttribute attrs[32];
        int i = 0;
        attrs[i++] = NSOpenGLPFAAccelerated;
        attrs[i++] = NSOpenGLPFADoubleBuffer;
        attrs[i++] = NSOpenGLPFAOpenGLProfile; attrs[i++] = NSOpenGLProfileVersion3_2Core;
        attrs[i++] = NSOpenGLPFAColorSize; attrs[i++] = 24;
        attrs[i++] = NSOpenGLPFAAlphaSize; attrs[i++] = 8;
        attrs[i++] = NSOpenGLPFADepthSize; attrs[i++] = 24;
        attrs[i++] = NSOpenGLPFAStencilSize; attrs[i++] = 8;
        if (_sapp.sample_count > 1) {
            attrs[i++] = NSOpenGLPFAMultisample;
            attrs[i++] = NSOpenGLPFASampleBuffers; attrs[i++] = 1;
            attrs[i++] = NSOpenGLPFASamples; attrs[i++] = _sapp.sample_count;
        }
        else {
            attrs[i++] = NSOpenGLPFASampleBuffers; attrs[i++] = 0;
        }
        attrs[i++] = 0;
        _sapp_macos_glpixelformat_obj = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        SOKOL_ASSERT(_sapp_macos_glpixelformat_obj != nil);

        _sapp_view_obj = [[_sapp_macos_view alloc]
            initWithFrame:window_rect
            pixelFormat:_sapp_macos_glpixelformat_obj];
        [_sapp_view_obj updateTrackingAreas];
        if (_sapp.desc.high_dpi) {
            [_sapp_view_obj setWantsBestResolutionOpenGLSurface:YES];
        }
        else {
            [_sapp_view_obj setWantsBestResolutionOpenGLSurface:NO];
        }

        _sapp_macos_window_obj.contentView = _sapp_view_obj;
        [_sapp_macos_window_obj makeFirstResponder:_sapp_view_obj];

        _sapp_macos_timer_obj = [NSTimer timerWithTimeInterval:0.001
            target:_sapp_view_obj
            selector:@selector(timerFired:)
            userInfo:nil
            repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:_sapp_macos_timer_obj forMode:NSDefaultRunLoopMode];
    #endif
    _sapp.valid = true;
    if (_sapp.fullscreen) {
        /* on GL, this already toggles a rendered frame, so set the valid flag before */
        [_sapp_macos_window_obj toggleFullScreen:self];
    }
    else {
        [_sapp_macos_window_obj center];
    }
    [_sapp_macos_window_obj makeKeyAndOrderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    _SOKOL_UNUSED(sender);
    return YES;
}
@end

_SOKOL_PRIVATE uint32_t _sapp_macos_mod(NSEventModifierFlags f) {
    uint32_t m = 0;
    if (f & NSEventModifierFlagShift) {
        m |= SAPP_MODIFIER_SHIFT;
    }
    if (f & NSEventModifierFlagControl) {
        m |= SAPP_MODIFIER_CTRL;
    }
    if (f & NSEventModifierFlagOption) {
        m |= SAPP_MODIFIER_ALT;
    }
    if (f & NSEventModifierFlagCommand) {
        m |= SAPP_MODIFIER_SUPER;
    }
    return m;
}

_SOKOL_PRIVATE void _sapp_macos_mouse_event(sapp_event_type type, sapp_mousebutton btn, uint32_t mod) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.mouse_button = btn;
        _sapp.event.modifiers = mod;
        _sapp.event.mouse_x = _sapp.mouse_x;
        _sapp.event.mouse_y = _sapp.mouse_y;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_macos_key_event(sapp_event_type type, sapp_keycode key, bool repeat, uint32_t mod) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.key_code = key;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = mod;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_macos_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

@implementation _sapp_macos_window_delegate
- (BOOL)windowShouldClose:(id)sender {
    _SOKOL_UNUSED(sender);
    /* only give user-code a chance to intervene when sapp_quit() wasn't already called */
    if (!_sapp.quit_ordered) {
        /* if window should be closed and event handling is enabled, give user code
           a chance to intervene via sapp_cancel_quit()
        */
        _sapp.quit_requested = true;
        _sapp_macos_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
        /* user code hasn't intervened, quit the app */
        if (_sapp.quit_requested) {
            _sapp.quit_ordered = true;
        }
    }
    if (_sapp.quit_ordered) {
        _sapp_call_cleanup();
        return YES;
    }
    else {
        return NO;
    }
}

- (void)windowDidResize:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_update_dimensions();
    _sapp_macos_app_event(SAPP_EVENTTYPE_RESIZED);
}

- (void)windowDidMiniaturize:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_app_event(SAPP_EVENTTYPE_ICONIFIED);
}

- (void)windowDidDeminiaturize:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_app_event(SAPP_EVENTTYPE_RESTORED);
}

- (void)windowDidEnterFullScreen:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp.fullscreen = true;
}

- (void)windowDidExitFullScreen:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp.fullscreen = false;
}
@end

#if defined(SOKOL_METAL)
@implementation _sapp_macos_mtk_view_dlg
- (void)drawInMTKView:(MTKView*)view {
    _SOKOL_UNUSED(view);
    @autoreleasepool {
        _sapp_macos_frame();
    }
}
- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size {
    _SOKOL_UNUSED(view);
    _SOKOL_UNUSED(size);
    /* this is required by the protocol, but we can't do anything useful here */
}
@end
#endif

@implementation _sapp_macos_view
#if defined(SOKOL_GLCORE33)
- (void)timerFired:(id)sender {
    _SOKOL_UNUSED(sender);
    [self setNeedsDisplay:YES];
}
- (void)prepareOpenGL {
    [super prepareOpenGL];
    GLint swapInt = 1;
    NSOpenGLContext* ctx = [_sapp_view_obj openGLContext];
    [ctx setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval];
    [ctx makeCurrentContext];
}
- (void)drawRect:(NSRect)bound {
    _SOKOL_UNUSED(bound);
    _sapp_macos_frame();
    [[_sapp_view_obj openGLContext] flushBuffer];
}
#endif

- (BOOL)isOpaque {
    return YES;
}
- (BOOL)canBecomeKey {
    return YES;
}
- (BOOL)acceptsFirstResponder {
    return YES;
}
- (void)updateTrackingAreas {
    if (trackingArea != nil) {
        [self removeTrackingArea:trackingArea];
        trackingArea = nil;
    }
    const NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
                                          NSTrackingActiveInKeyWindow |
                                          NSTrackingEnabledDuringMouseDrag |
                                          NSTrackingCursorUpdate |
                                          NSTrackingInVisibleRect |
                                          NSTrackingAssumeInside;
    trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:options owner:self userInfo:nil];
    [self addTrackingArea:trackingArea];
    [super updateTrackingAreas];
}
- (void)mouseEntered:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mod(event.modifierFlags));
}
- (void)mouseExited:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mod(event.modifierFlags));
}
- (void)mouseDown:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_LEFT, _sapp_macos_mod(event.modifierFlags));
}
- (void)mouseUp:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_LEFT, _sapp_macos_mod(event.modifierFlags));
}
- (void)rightMouseDown:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_RIGHT, _sapp_macos_mod(event.modifierFlags));
}
- (void)rightMouseUp:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_RIGHT, _sapp_macos_mod(event.modifierFlags));
}
- (void)otherMouseDown:(NSEvent*)event {
    if (2 == event.buttonNumber) {
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_MIDDLE, _sapp_macos_mod(event.modifierFlags));
    }
}
- (void)otherMouseUp:(NSEvent*)event {
    if (2 == event.buttonNumber) {
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_MIDDLE, _sapp_macos_mod(event.modifierFlags));
    }
}
- (void)mouseMoved:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID , _sapp_macos_mod(event.modifierFlags));
}
- (void)mouseDragged:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID , _sapp_macos_mod(event.modifierFlags));
}
- (void)rightMouseDragged:(NSEvent*)event {
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mod(event.modifierFlags));
}
- (void)scrollWheel:(NSEvent*)event {
    if (_sapp_events_enabled()) {
        float dx = (float) event.scrollingDeltaX;
        float dy = (float) event.scrollingDeltaY;
        if (event.hasPreciseScrollingDeltas) {
            dx *= 0.1;
            dy *= 0.1;
        }
        if ((_sapp_absf(dx) > 0.0f) || (_sapp_absf(dy) > 0.0f)) {
            _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
            _sapp.event.modifiers = _sapp_macos_mod(event.modifierFlags);
            _sapp.event.mouse_x = _sapp.mouse_x;
            _sapp.event.mouse_y = _sapp.mouse_y;
            _sapp.event.scroll_x = dx;
            _sapp.event.scroll_y = dy;
            _sapp_call_event(&_sapp.event);
        }
    }
}
- (void)keyDown:(NSEvent*)event {
    if (_sapp_events_enabled()) {
        const uint32_t mods = _sapp_macos_mod(event.modifierFlags);
        /* NOTE: macOS doesn't send keyUp events while the Cmd key is pressed,
            as a workaround, to prevent key presses from sticking we'll send
            a keyup event following right after the keydown if SUPER is also pressed
        */
        const sapp_keycode key_code = _sapp_translate_key(event.keyCode);
        _sapp_macos_key_event(SAPP_EVENTTYPE_KEY_DOWN, key_code, event.isARepeat, mods);
        if (0 != (mods & SAPP_MODIFIER_SUPER)) {
            _sapp_macos_key_event(SAPP_EVENTTYPE_KEY_UP, key_code, event.isARepeat, mods);
        }
        const NSString* chars = event.characters;
        const NSUInteger len = chars.length;
        if (len > 0) {
            _sapp_init_event(SAPP_EVENTTYPE_CHAR);
            _sapp.event.modifiers = mods;
            for (NSUInteger i = 0; i < len; i++) {
                const unichar codepoint = [chars characterAtIndex:i];
                if ((codepoint & 0xFF00) == 0xF700) {
                    continue;
                }
                _sapp.event.char_code = codepoint;
                _sapp.event.key_repeat = event.isARepeat;
                _sapp_call_event(&_sapp.event);
            }
        }
        /* if this is a Cmd+V (paste), also send a CLIPBOARD_PASTE event */
        if (_sapp.clipboard_enabled && (mods == SAPP_MODIFIER_SUPER) && (key_code == SAPP_KEYCODE_V)) {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}
- (void)keyUp:(NSEvent*)event {
    _sapp_macos_key_event(SAPP_EVENTTYPE_KEY_UP,
        _sapp_translate_key(event.keyCode),
        event.isARepeat,
        _sapp_macos_mod(event.modifierFlags));
}
- (void)flagsChanged:(NSEvent*)event {
    const uint32_t old_f = _sapp_macos_flags_changed_store;
    const uint32_t new_f = event.modifierFlags;
    _sapp_macos_flags_changed_store = new_f;
    sapp_keycode key_code = SAPP_KEYCODE_INVALID;
    bool down = false;
    if ((new_f ^ old_f) & NSEventModifierFlagShift) {
        key_code = SAPP_KEYCODE_LEFT_SHIFT;
        down = 0 != (new_f & NSEventModifierFlagShift);
    }
    if ((new_f ^ old_f) & NSEventModifierFlagControl) {
        key_code = SAPP_KEYCODE_LEFT_CONTROL;
        down = 0 != (new_f & NSEventModifierFlagControl);
    }
    if ((new_f ^ old_f) & NSEventModifierFlagOption) {
        key_code = SAPP_KEYCODE_LEFT_ALT;
        down = 0 != (new_f & NSEventModifierFlagOption);
    }
    if ((new_f ^ old_f) & NSEventModifierFlagCommand) {
        key_code = SAPP_KEYCODE_LEFT_SUPER;
        down = 0 != (new_f & NSEventModifierFlagCommand);
    }
    if (key_code != SAPP_KEYCODE_INVALID) {
        _sapp_macos_key_event(down ? SAPP_EVENTTYPE_KEY_DOWN : SAPP_EVENTTYPE_KEY_UP,
            key_code,
            false,
            _sapp_macos_mod(event.modifierFlags));
    }
}
- (void)cursorUpdate:(NSEvent*)event {
    _SOKOL_UNUSED(event);
    if (_sapp.desc.user_cursor) {
        _sapp_macos_app_event(SAPP_EVENTTYPE_UPDATE_CURSOR);
    }
}
@end

void _sapp_macos_set_clipboard_string(const char* str) {
    @autoreleasepool {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard declareTypes:@[NSPasteboardTypeString] owner:nil];
        [pasteboard setString:@(str) forType:NSPasteboardTypeString];
    }
}

void _sapp_macos_show_mouse(bool shown) {
    if (shown) {
        [NSCursor unhide];
    }
    else {
        [NSCursor hide];
    }
}

const char* _sapp_macos_get_clipboard_string(void) {
    SOKOL_ASSERT(_sapp.clipboard);
    @autoreleasepool {
        _sapp.clipboard[0] = 0;
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        if (![[pasteboard types] containsObject:NSPasteboardTypeString]) {
            return _sapp.clipboard;
        }
        NSString* str = [pasteboard stringForType:NSPasteboardTypeString];
        if (!str) {
            return _sapp.clipboard;
        }
        _sapp_strcpy([str UTF8String], _sapp.clipboard, _sapp.clipboard_size);
    }
    return _sapp.clipboard;
}

#endif /* MacOS */

/*== iOS =====================================================================*/
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#if defined(SOKOL_METAL)
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#else
#import <GLKit/GLKit.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

@interface _sapp_app_delegate : NSObject<UIApplicationDelegate>
@end
@interface _sapp_textfield_dlg : NSObject<UITextFieldDelegate>
- (void)keyboardWasShown:(NSNotification*)notif;
- (void)keyboardWillBeHidden:(NSNotification*)notif;
- (void)keyboardDidChangeFrame:(NSNotification*)notif;
@end
#if defined(SOKOL_METAL)
@interface _sapp_ios_mtk_view_dlg : NSObject<MTKViewDelegate>
@end
@interface _sapp_ios_view : MTKView;
@end
#else
@interface _sapp_ios_glk_view_dlg : NSObject<GLKViewDelegate>
@end
@interface _sapp_ios_view : GLKView
@end
#endif

static bool _sapp_ios_suspended;
static UIWindow* _sapp_ios_window_obj;
static _sapp_ios_view* _sapp_view_obj;
static UITextField* _sapp_ios_textfield_obj;
static _sapp_textfield_dlg* _sapp_ios_textfield_dlg_obj;
#if defined(SOKOL_METAL)
static _sapp_ios_mtk_view_dlg* _sapp_ios_mtk_view_dlg_obj;
static UIViewController<MTKViewDelegate>* _sapp_ios_view_ctrl_obj;
static id<MTLDevice> _sapp_mtl_device_obj;
#else
static EAGLContext* _sapp_ios_eagl_ctx_obj;
static _sapp_ios_glk_view_dlg* _sapp_ios_glk_view_dlg_obj;
static GLKViewController* _sapp_ios_view_ctrl_obj;
#endif

_SOKOL_PRIVATE void _sapp_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    static int argc = 1;
    static char* argv[] = { (char*)"sokol_app" };
    UIApplicationMain(argc, argv, nil, NSStringFromClass([_sapp_app_delegate class]));
    _sapp_discard_state();
}

/* iOS entry function */
#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */

_SOKOL_PRIVATE void _sapp_ios_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_ios_update_dimensions(void) {
    CGRect screen_rect = UIScreen.mainScreen.bounds;
    _sapp.window_width = (int) screen_rect.size.width;
    _sapp.window_height = (int) screen_rect.size.height;
    int cur_fb_width, cur_fb_height;
    #if defined(SOKOL_METAL)
        const CGSize fb_size = _sapp_view_obj.drawableSize;
        cur_fb_width = (int) fb_size.width;
        cur_fb_height = (int) fb_size.height;
    #else
        cur_fb_width = (int) _sapp_view_obj.drawableWidth;
        cur_fb_height = (int) _sapp_view_obj.drawableHeight;
    #endif
    const bool dim_changed =
        (_sapp.framebuffer_width != cur_fb_width) ||
        (_sapp.framebuffer_height != cur_fb_height);
    _sapp.framebuffer_width = cur_fb_width;
    _sapp.framebuffer_height = cur_fb_height;
    SOKOL_ASSERT((_sapp.framebuffer_width > 0) && (_sapp.framebuffer_height > 0));
    _sapp.dpi_scale = (float)_sapp.framebuffer_width / (float) _sapp.window_width;
    if (dim_changed) {
        _sapp_ios_app_event(SAPP_EVENTTYPE_RESIZED);
    }
}

_SOKOL_PRIVATE void _sapp_ios_frame(void) {
    _sapp_ios_update_dimensions();
    _sapp_frame();
}

_SOKOL_PRIVATE void _sapp_ios_show_keyboard(bool shown) {
    /* if not happened yet, create an invisible text field */
    if (nil == _sapp_ios_textfield_obj) {
        _sapp_ios_textfield_dlg_obj = [[_sapp_textfield_dlg alloc] init];
        _sapp_ios_textfield_obj = [[UITextField alloc] initWithFrame:CGRectMake(10, 10, 100, 50)];
        _sapp_ios_textfield_obj.keyboardType = UIKeyboardTypeDefault;
        _sapp_ios_textfield_obj.returnKeyType = UIReturnKeyDefault;
        _sapp_ios_textfield_obj.autocapitalizationType = UITextAutocapitalizationTypeNone;
        _sapp_ios_textfield_obj.autocorrectionType = UITextAutocorrectionTypeNo;
        _sapp_ios_textfield_obj.spellCheckingType = UITextSpellCheckingTypeNo;
        _sapp_ios_textfield_obj.hidden = YES;
        _sapp_ios_textfield_obj.text = @"x";
        _sapp_ios_textfield_obj.delegate = _sapp_ios_textfield_dlg_obj;
        [_sapp_ios_view_ctrl_obj.view addSubview:_sapp_ios_textfield_obj];

        [[NSNotificationCenter defaultCenter] addObserver:_sapp_ios_textfield_dlg_obj
            selector:@selector(keyboardWasShown:)
            name:UIKeyboardDidShowNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:_sapp_ios_textfield_dlg_obj
            selector:@selector(keyboardWillBeHidden:)
            name:UIKeyboardWillHideNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:_sapp_ios_textfield_dlg_obj
            selector:@selector(keyboardDidChangeFrame:)
            name:UIKeyboardDidChangeFrameNotification object:nil];
    }
    if (shown) {
        /* setting the text field as first responder brings up the onscreen keyboard */
        [_sapp_ios_textfield_obj becomeFirstResponder];
    }
    else {
        [_sapp_ios_textfield_obj resignFirstResponder];
    }
}

@implementation _sapp_app_delegate
- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    CGRect screen_rect = UIScreen.mainScreen.bounds;
    _sapp_ios_window_obj = [[UIWindow alloc] initWithFrame:screen_rect];
    _sapp.window_width = screen_rect.size.width;
    _sapp.window_height = screen_rect.size.height;
    if (_sapp.desc.high_dpi) {
        _sapp.framebuffer_width = 2 * _sapp.window_width;
        _sapp.framebuffer_height = 2 * _sapp.window_height;
    }
    else {
        _sapp.framebuffer_width = _sapp.window_width;
        _sapp.framebuffer_height = _sapp.window_height;
    }
    _sapp.dpi_scale = (float)_sapp.framebuffer_width / (float) _sapp.window_width;
    #if defined(SOKOL_METAL)
        _sapp_mtl_device_obj = MTLCreateSystemDefaultDevice();
        _sapp_ios_mtk_view_dlg_obj = [[_sapp_ios_mtk_view_dlg alloc] init];
        _sapp_view_obj = [[_sapp_ios_view alloc] init];
        _sapp_view_obj.preferredFramesPerSecond = 60 / _sapp.swap_interval;
        _sapp_view_obj.delegate = _sapp_ios_mtk_view_dlg_obj;
        _sapp_view_obj.device = _sapp_mtl_device_obj;
        _sapp_view_obj.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
        _sapp_view_obj.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        _sapp_view_obj.sampleCount = _sapp.sample_count;
        if (_sapp.desc.high_dpi) {
            _sapp_view_obj.contentScaleFactor = 2.0;
        }
        else {
            _sapp_view_obj.contentScaleFactor = 1.0;
        }
        _sapp_view_obj.userInteractionEnabled = YES;
        _sapp_view_obj.multipleTouchEnabled = YES;
        [_sapp_ios_window_obj addSubview:_sapp_view_obj];
        _sapp_ios_view_ctrl_obj = [[UIViewController<MTKViewDelegate> alloc] init];
        _sapp_ios_view_ctrl_obj.view = _sapp_view_obj;
        _sapp_ios_window_obj.rootViewController = _sapp_ios_view_ctrl_obj;
    #else
        if (_sapp.desc.gl_force_gles2) {
            _sapp_ios_eagl_ctx_obj = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
            _sapp.gles2_fallback = true;
        }
        else {
            _sapp_ios_eagl_ctx_obj = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
            if (_sapp_ios_eagl_ctx_obj == nil) {
                _sapp_ios_eagl_ctx_obj = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
                _sapp.gles2_fallback = true;
            }
        }
        _sapp_ios_glk_view_dlg_obj = [[_sapp_ios_glk_view_dlg alloc] init];
        _sapp_view_obj = [[_sapp_ios_view alloc] initWithFrame:screen_rect];
        _sapp_view_obj.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
        _sapp_view_obj.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        _sapp_view_obj.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
        _sapp_view_obj.drawableMultisample = GLKViewDrawableMultisampleNone; /* FIXME */
        _sapp_view_obj.context = _sapp_ios_eagl_ctx_obj;
        _sapp_view_obj.delegate = _sapp_ios_glk_view_dlg_obj;
        _sapp_view_obj.enableSetNeedsDisplay = NO;
        _sapp_view_obj.userInteractionEnabled = YES;
        _sapp_view_obj.multipleTouchEnabled = YES;
        if (_sapp.desc.high_dpi) {
            _sapp_view_obj.contentScaleFactor = 2.0;
        }
        else {
            _sapp_view_obj.contentScaleFactor = 1.0;
        }
        [_sapp_ios_window_obj addSubview:_sapp_view_obj];
        _sapp_ios_view_ctrl_obj = [[GLKViewController alloc] init];
        _sapp_ios_view_ctrl_obj.view = _sapp_view_obj;
        _sapp_ios_view_ctrl_obj.preferredFramesPerSecond = 60 / _sapp.swap_interval;
        _sapp_ios_window_obj.rootViewController = _sapp_ios_view_ctrl_obj;
    #endif
    [_sapp_ios_window_obj makeKeyAndVisible];

    _sapp.valid = true;
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    if (!_sapp_ios_suspended) {
        _sapp_ios_suspended = true;
        _sapp_ios_app_event(SAPP_EVENTTYPE_SUSPENDED);
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    if (_sapp_ios_suspended) {
        _sapp_ios_suspended = false;
        _sapp_ios_app_event(SAPP_EVENTTYPE_RESUMED);
    }
}
@end

@implementation _sapp_textfield_dlg
- (void)keyboardWasShown:(NSNotification*)notif {
    _sapp.onscreen_keyboard_shown = true;
    /* query the keyboard's size, and modify the content view's size */
    if (_sapp.desc.ios_keyboard_resizes_canvas) {
        NSDictionary* info = notif.userInfo;
        CGFloat kbd_h = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size.height;
        CGRect view_frame = UIScreen.mainScreen.bounds;
        view_frame.size.height -= kbd_h;
        _sapp_view_obj.frame = view_frame;
    }
}
- (void)keyboardWillBeHidden:(NSNotification*)notif {
    _sapp.onscreen_keyboard_shown = false;
    if (_sapp.desc.ios_keyboard_resizes_canvas) {
        _sapp_view_obj.frame = UIScreen.mainScreen.bounds;
    }
}
- (void)keyboardDidChangeFrame:(NSNotification*)notif {
    /* this is for the case when the screen rotation changes while the keyboard is open */
    if (_sapp.onscreen_keyboard_shown && _sapp.desc.ios_keyboard_resizes_canvas) {
        NSDictionary* info = notif.userInfo;
        CGFloat kbd_h = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size.height;
        CGRect view_frame = UIScreen.mainScreen.bounds;
        view_frame.size.height -= kbd_h;
        _sapp_view_obj.frame = view_frame;
    }
}
- (BOOL)textField:(UITextField*)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString*)string {
    if (_sapp_events_enabled()) {
        const NSUInteger len = string.length;
        if (len > 0) {
            for (NSUInteger i = 0; i < len; i++) {
                unichar c = [string characterAtIndex:i];
                if (c >= 32) {
                    /* ignore surrogates for now */
                    if ((c < 0xD800) || (c > 0xDFFF)) {
                        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
                        _sapp.event.char_code = c;
                        _sapp_call_event(&_sapp.event);
                    }
                }
                if (c <= 32) {
                    sapp_keycode k = SAPP_KEYCODE_INVALID;
                    switch (c) {
                        case 10: k = SAPP_KEYCODE_ENTER; break;
                        case 32: k = SAPP_KEYCODE_SPACE; break;
                        default: break;
                    }
                    if (k != SAPP_KEYCODE_INVALID) {
                        _sapp_init_event(SAPP_EVENTTYPE_KEY_DOWN);
                        _sapp.event.key_code = k;
                        _sapp_call_event(&_sapp.event);
                        _sapp_init_event(SAPP_EVENTTYPE_KEY_UP);
                        _sapp.event.key_code = k;
                        _sapp_call_event(&_sapp.event);
                    }
                }
            }
        }
        else {
            /* this was a backspace */
            _sapp_init_event(SAPP_EVENTTYPE_KEY_DOWN);
            _sapp.event.key_code = SAPP_KEYCODE_BACKSPACE;
            _sapp_call_event(&_sapp.event);
            _sapp_init_event(SAPP_EVENTTYPE_KEY_UP);
            _sapp.event.key_code = SAPP_KEYCODE_BACKSPACE;
            _sapp_call_event(&_sapp.event);
        }
    }
    return NO;
}
@end

#if defined(SOKOL_METAL)
@implementation _sapp_ios_mtk_view_dlg
- (void)drawInMTKView:(MTKView*)view {
    @autoreleasepool {
        _sapp_ios_frame();
    }
}

- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size {
    /* this is required by the protocol, but we can't do anything useful here */
}
@end
#else
@implementation _sapp_ios_glk_view_dlg
- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect {
    @autoreleasepool {
        _sapp_ios_frame();
    }
}
@end
#endif

_SOKOL_PRIVATE void _sapp_ios_touch_event(sapp_event_type type, NSSet<UITouch *>* touches, UIEvent* event) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        NSEnumerator* enumerator = event.allTouches.objectEnumerator;
        UITouch* ios_touch;
        while ((ios_touch = [enumerator nextObject])) {
            if ((_sapp.event.num_touches + 1) < SAPP_MAX_TOUCHPOINTS) {
                CGPoint ios_pos = [ios_touch locationInView:_sapp_view_obj];
                sapp_touchpoint* cur_point = &_sapp.event.touches[_sapp.event.num_touches++];
                cur_point->identifier = (uintptr_t) ios_touch;
                cur_point->pos_x = ios_pos.x * _sapp.dpi_scale;
                cur_point->pos_y = ios_pos.y * _sapp.dpi_scale;
                cur_point->changed = [touches containsObject:ios_touch];
            }
        }
        if (_sapp.event.num_touches > 0) {
            _sapp_call_event(&_sapp.event);
        }
    }
}

@implementation _sapp_ios_view
- (BOOL) isOpaque {
    return YES;
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_BEGAN, touches, event);
}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_MOVED, touches, event);
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_ENDED, touches, event);
}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_CANCELLED, touches, event);
}
@end
#endif /* TARGET_OS_IPHONE */

#endif /* __APPLE__ */

/*== EMSCRIPTEN ==============================================================*/
#if defined(__EMSCRIPTEN__)
#if defined(SOKOL_GLES3)
#include <GLES3/gl3.h>
#elif defined(SOKOL_GLES2)
#ifndef GL_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(SOKOL_WGPU)
#include <webgpu/webgpu.h>
#endif
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

static struct {
    bool textfield_created;
    bool wants_show_keyboard;
    bool wants_hide_keyboard;
    #if defined(SOKOL_WGPU)
    struct {
        int state;
        WGPUDevice device;
        WGPUSwapChain swapchain;
        WGPUTextureFormat render_format;
        WGPUTexture msaa_tex;
        WGPUTexture depth_stencil_tex;
        WGPUTextureView swapchain_view;
        WGPUTextureView msaa_view;
        WGPUTextureView depth_stencil_view;
    } wgpu;
    #endif
} _sapp_emsc;

/* this function is called from a JS event handler when the user hides
    the onscreen keyboard pressing the 'dismiss keyboard key'
*/
#ifdef __cplusplus
extern "C" {
#endif
EMSCRIPTEN_KEEPALIVE void _sapp_emsc_notify_keyboard_hidden(void) {
    _sapp.onscreen_keyboard_shown = false;
}
#ifdef __cplusplus
} /* extern "C" */
#endif

/* Javascript helper functions for mobile virtual keyboard input */
EM_JS(void, sapp_js_create_textfield, (void), {
    var _sapp_inp = document.createElement("input");
    _sapp_inp.type = "text";
    _sapp_inp.id = "_sokol_app_input_element";
    _sapp_inp.autocapitalize = "none";
    _sapp_inp.addEventListener("focusout", function(_sapp_event) {
        __sapp_emsc_notify_keyboard_hidden()

    });
    document.body.append(_sapp_inp);
});

EM_JS(void, sapp_js_focus_textfield, (void), {
    document.getElementById("_sokol_app_input_element").focus();
});

EM_JS(void, sapp_js_unfocus_textfield, (void), {
    document.getElementById("_sokol_app_input_element").blur();
});

EMSCRIPTEN_KEEPALIVE void _sapp_emsc_onpaste(const char* str) {
    if (_sapp.clipboard_enabled) {
        _sapp_strcpy(str, _sapp.clipboard, _sapp.clipboard_size);
        if (_sapp_events_enabled()) {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}

/*  https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onbeforeunload */
EMSCRIPTEN_KEEPALIVE int _sapp_html5_get_ask_leave_site(void) {
    return _sapp.html5_ask_leave_site ? 1 : 0;
}

EM_JS(void, sapp_add_js_hook_beforeunload, (void), {
    Module.sokol_beforeunload = function(_sapp_event) {
        if (__sapp_html5_get_ask_leave_site() != 0) {
            _sapp_event.preventDefault();
            _sapp_event.returnValue = ' ';
        }
    };
    window.addEventListener('beforeunload', Module.sokol_beforeunload);
});

EM_JS(void, sapp_remove_js_hook_beforeunload, (void), {
    window.removeEventListener('beforeunload', Module.sokol_beforeunload);
});

EM_JS(void, sapp_add_js_hook_clipboard, (void), {
    Module.sokol_paste = function(event) {
        var pasted_str = event.clipboardData.getData('text');
        ccall('_sapp_emsc_onpaste', 'void', ['string'], [pasted_str]);
    };
    window.addEventListener('paste', Module.sokol_paste);
});

EM_JS(void, sapp_remove_js_hook_clipboard, (void), {
    window.removeEventListener('paste', Module.sokol_paste);
});

EM_JS(void, sapp_js_write_clipboard, (const char* c_str), {
    var str = UTF8ToString(c_str);
    var ta = document.createElement('textarea');
    ta.setAttribute('autocomplete', 'off');
    ta.setAttribute('autocorrect', 'off');
    ta.setAttribute('autocapitalize', 'off');
    ta.setAttribute('spellcheck', 'false');
    ta.style.left = -100 + 'px';
    ta.style.top = -100 + 'px';
    ta.style.height = 1;
    ta.style.width = 1;
    ta.value = str;
    document.body.appendChild(ta);
    ta.select();
    document.execCommand('copy');
    document.body.removeChild(ta);
});

_SOKOL_PRIVATE void _sapp_emsc_set_clipboard_string(const char* str) {
    sapp_js_write_clipboard(str);
}

/* called from the emscripten event handler to update the keyboard visibility
    state, this must happen from an JS input event handler, otherwise
    the request will be ignored by the browser
*/
_SOKOL_PRIVATE void _sapp_emsc_update_keyboard_state(void) {
    if (_sapp_emsc.wants_show_keyboard) {
        /* create input text field on demand */
        if (!_sapp_emsc.textfield_created) {
            _sapp_emsc.textfield_created = true;
            sapp_js_create_textfield();
        }
        /* focus the text input field, this will bring up the keyboard */
        _sapp.onscreen_keyboard_shown = true;
        _sapp_emsc.wants_show_keyboard = false;
        sapp_js_focus_textfield();
    }
    if (_sapp_emsc.wants_hide_keyboard) {
        /* unfocus the text input field */
        if (_sapp_emsc.textfield_created) {
            _sapp.onscreen_keyboard_shown = false;
            _sapp_emsc.wants_hide_keyboard = false;
            sapp_js_unfocus_textfield();
        }
    }
}

/* actually showing the onscreen keyboard must be initiated from a JS
    input event handler, so we'll just keep track of the desired
    state, and the actual state change will happen with the next input event
*/
_SOKOL_PRIVATE void _sapp_emsc_show_keyboard(bool show) {
    if (show) {
        _sapp_emsc.wants_show_keyboard = true;
    }
    else {
        _sapp_emsc.wants_hide_keyboard = true;
    }
}

#if defined(SOKOL_WGPU)
_SOKOL_PRIVATE void _sapp_emsc_wgpu_surfaces_create(void);
_SOKOL_PRIVATE void _sapp_emsc_wgpu_surfaces_discard(void);
#endif

_SOKOL_PRIVATE EM_BOOL _sapp_emsc_size_changed(int event_type, const EmscriptenUiEvent* ui_event, void* user_data) {
    _SOKOL_UNUSED(event_type);
    _SOKOL_UNUSED(user_data);
    double w, h;
    emscripten_get_element_css_size(_sapp.html5_canvas_name, &w, &h);
    /* The above method might report zero when toggling HTML5 fullscreen,
       in that case use the window's inner width reported by the
       emscripten event. This works ok when toggling *into* fullscreen
       but doesn't properly restore the previous canvas size when switching
       back from fullscreen.

       In general, due to the HTML5's fullscreen API's flaky nature it is
       recommended to use 'soft fullscreen' (stretching the WebGL canvas
       over the browser windows client rect) with a CSS definition like this:

            position: absolute;
            top: 0px;
            left: 0px;
            margin: 0px;
            border: 0;
            width: 100%;
            height: 100%;
            overflow: hidden;
            display: block;
    */
    if (w < 1.0) {
        w = ui_event->windowInnerWidth;
    }
    else {
        _sapp.window_width = (int) w;
    }
    if (h < 1.0) {
        h = ui_event->windowInnerHeight;
    }
    else {
        _sapp.window_height = (int) h;
    }
    if (_sapp.desc.high_dpi) {
        _sapp.dpi_scale = emscripten_get_device_pixel_ratio();
    }
    _sapp.framebuffer_width = (int) (w * _sapp.dpi_scale);
    _sapp.framebuffer_height = (int) (h * _sapp.dpi_scale);
    SOKOL_ASSERT((_sapp.framebuffer_width > 0) && (_sapp.framebuffer_height > 0));
    emscripten_set_canvas_element_size(_sapp.html5_canvas_name, _sapp.framebuffer_width, _sapp.framebuffer_height);
    #if defined(SOKOL_WGPU)
        /* on WebGPU: recreate size-dependent rendering surfaces */
        _sapp_emsc_wgpu_surfaces_discard();
        _sapp_emsc_wgpu_surfaces_create();
    #endif
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_RESIZED);
        _sapp_call_event(&_sapp.event);
    }
    return true;
}

_SOKOL_PRIVATE EM_BOOL _sapp_emsc_mouse_cb(int emsc_type, const EmscriptenMouseEvent* emsc_event, void* user_data) {
    _SOKOL_UNUSED(user_data);
    _sapp.mouse_x = (emsc_event->targetX * _sapp.dpi_scale);
    _sapp.mouse_y = (emsc_event->targetY * _sapp.dpi_scale);
    if (_sapp_events_enabled() && (emsc_event->button >= 0) && (emsc_event->button < SAPP_MAX_MOUSEBUTTONS)) {
        sapp_event_type type;
        bool is_button_event = false;
        switch (emsc_type) {
            case EMSCRIPTEN_EVENT_MOUSEDOWN:
                type = SAPP_EVENTTYPE_MOUSE_DOWN;
                is_button_event = true;
                break;
            case EMSCRIPTEN_EVENT_MOUSEUP:
                type = SAPP_EVENTTYPE_MOUSE_UP;
                is_button_event = true;
                break;
            case EMSCRIPTEN_EVENT_MOUSEMOVE:
                type = SAPP_EVENTTYPE_MOUSE_MOVE;
                break;
            case EMSCRIPTEN_EVENT_MOUSEENTER:
                type = SAPP_EVENTTYPE_MOUSE_ENTER;
                break;
            case EMSCRIPTEN_EVENT_MOUSELEAVE:
                type = SAPP_EVENTTYPE_MOUSE_LEAVE;
                break;
            default:
                type = SAPP_EVENTTYPE_INVALID;
                break;
        }
        if (type != SAPP_EVENTTYPE_INVALID) {
            _sapp_init_event(type);
            if (emsc_event->ctrlKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_CTRL;
            }
            if (emsc_event->shiftKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_SHIFT;
            }
            if (emsc_event->altKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_ALT;
            }
            if (emsc_event->metaKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_SUPER;
            }
            if (is_button_event) {
                switch (emsc_event->button) {
                    case 0: _sapp.event.mouse_button = SAPP_MOUSEBUTTON_LEFT; break;
                    case 1: _sapp.event.mouse_button = SAPP_MOUSEBUTTON_MIDDLE; break;
                    case 2: _sapp.event.mouse_button = SAPP_MOUSEBUTTON_RIGHT; break;
                    default: _sapp.event.mouse_button = (sapp_mousebutton)emsc_event->button; break;
                }
            }
            else {
                _sapp.event.mouse_button = SAPP_MOUSEBUTTON_INVALID;
            }
            _sapp.event.mouse_x = _sapp.mouse_x;
            _sapp.event.mouse_y = _sapp.mouse_y;
            _sapp_call_event(&_sapp.event);
        }
    }
    _sapp_emsc_update_keyboard_state();
    return true;
}

_SOKOL_PRIVATE EM_BOOL _sapp_emsc_wheel_cb(int emsc_type, const EmscriptenWheelEvent* emsc_event, void* user_data) {
    _SOKOL_UNUSED(emsc_type);
    _SOKOL_UNUSED(user_data);
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
        if (emsc_event->mouse.ctrlKey) {
            _sapp.event.modifiers |= SAPP_MODIFIER_CTRL;
        }
        if (emsc_event->mouse.shiftKey) {
            _sapp.event.modifiers |= SAPP_MODIFIER_SHIFT;
        }
        if (emsc_event->mouse.altKey) {
            _sapp.event.modifiers |= SAPP_MODIFIER_ALT;
        }
        if (emsc_event->mouse.metaKey) {
            _sapp.event.modifiers |= SAPP_MODIFIER_SUPER;
        }
        _sapp.event.scroll_x = -0.1 * (float)emsc_event->deltaX;
        _sapp.event.scroll_y = -0.1 * (float)emsc_event->deltaY;
        _sapp_call_event(&_sapp.event);
    }
    _sapp_emsc_update_keyboard_state();
    return true;
}

_SOKOL_PRIVATE EM_BOOL _sapp_emsc_key_cb(int emsc_type, const EmscriptenKeyboardEvent* emsc_event, void* user_data) {
    _SOKOL_UNUSED(user_data);
    bool retval = true;
    if (_sapp_events_enabled()) {
        sapp_event_type type;
        switch (emsc_type) {
            case EMSCRIPTEN_EVENT_KEYDOWN:
                type = SAPP_EVENTTYPE_KEY_DOWN;
                break;
            case EMSCRIPTEN_EVENT_KEYUP:
                type = SAPP_EVENTTYPE_KEY_UP;
                break;
            case EMSCRIPTEN_EVENT_KEYPRESS:
                type = SAPP_EVENTTYPE_CHAR;
                break;
            default:
                type = SAPP_EVENTTYPE_INVALID;
                break;
        }
        if (type != SAPP_EVENTTYPE_INVALID) {
            bool send_keyup_followup = false;
            _sapp_init_event(type);
            _sapp.event.key_repeat = emsc_event->repeat;
            if (emsc_event->ctrlKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_CTRL;
            }
            if (emsc_event->shiftKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_SHIFT;
            }
            if (emsc_event->altKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_ALT;
            }
            if (emsc_event->metaKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_SUPER;
            }
            if (type == SAPP_EVENTTYPE_CHAR) {
                _sapp.event.char_code = emsc_event->charCode;
                /* workaround to make Cmd+V work on Safari */
                if ((emsc_event->metaKey) && (emsc_event->charCode == 118)) {
                    retval = false;
                }
            }
            else {
                _sapp.event.key_code = _sapp_translate_key(emsc_event->keyCode);
                /* Special hack for macOS: if the Super key is pressed, macOS doesn't
                    send keyUp events. As a workaround, to prevent keys from
                    "sticking", we'll send a keyup event following a keydown
                    when the SUPER key is pressed
                */
                if ((type == SAPP_EVENTTYPE_KEY_DOWN) &&
                    (_sapp.event.key_code != SAPP_KEYCODE_LEFT_SUPER) &&
                    (_sapp.event.key_code != SAPP_KEYCODE_RIGHT_SUPER) &&
                    (_sapp.event.modifiers & SAPP_MODIFIER_SUPER))
                {
                    send_keyup_followup = true;
                }
                /* only forward a certain key ranges to the browser */
                switch (_sapp.event.key_code) {
                    case SAPP_KEYCODE_WORLD_1:
                    case SAPP_KEYCODE_WORLD_2:
                    case SAPP_KEYCODE_ESCAPE:
                    case SAPP_KEYCODE_ENTER:
                    case SAPP_KEYCODE_TAB:
                    case SAPP_KEYCODE_BACKSPACE:
                    case SAPP_KEYCODE_INSERT:
                    case SAPP_KEYCODE_DELETE:
                    case SAPP_KEYCODE_RIGHT:
                    case SAPP_KEYCODE_LEFT:
                    case SAPP_KEYCODE_DOWN:
                    case SAPP_KEYCODE_UP:
                    case SAPP_KEYCODE_PAGE_UP:
                    case SAPP_KEYCODE_PAGE_DOWN:
                    case SAPP_KEYCODE_HOME:
                    case SAPP_KEYCODE_END:
                    case SAPP_KEYCODE_CAPS_LOCK:
                    case SAPP_KEYCODE_SCROLL_LOCK:
                    case SAPP_KEYCODE_NUM_LOCK:
                    case SAPP_KEYCODE_PRINT_SCREEN:
                    case SAPP_KEYCODE_PAUSE:
                    case SAPP_KEYCODE_F1:
                    case SAPP_KEYCODE_F2:
                    case SAPP_KEYCODE_F3:
                    case SAPP_KEYCODE_F4:
                    case SAPP_KEYCODE_F5:
                    case SAPP_KEYCODE_F6:
                    case SAPP_KEYCODE_F7:
                    case SAPP_KEYCODE_F8:
                    case SAPP_KEYCODE_F9:
                    case SAPP_KEYCODE_F10:
                    case SAPP_KEYCODE_F11:
                    case SAPP_KEYCODE_F12:
                    case SAPP_KEYCODE_F13:
                    case SAPP_KEYCODE_F14:
                    case SAPP_KEYCODE_F15:
                    case SAPP_KEYCODE_F16:
                    case SAPP_KEYCODE_F17:
                    case SAPP_KEYCODE_F18:
                    case SAPP_KEYCODE_F19:
                    case SAPP_KEYCODE_F20:
                    case SAPP_KEYCODE_F21:
                    case SAPP_KEYCODE_F22:
                    case SAPP_KEYCODE_F23:
                    case SAPP_KEYCODE_F24:
                    case SAPP_KEYCODE_F25:
                    case SAPP_KEYCODE_LEFT_SHIFT:
                    case SAPP_KEYCODE_LEFT_CONTROL:
                    case SAPP_KEYCODE_LEFT_ALT:
                    case SAPP_KEYCODE_LEFT_SUPER:
                    case SAPP_KEYCODE_RIGHT_SHIFT:
                    case SAPP_KEYCODE_RIGHT_CONTROL:
                    case SAPP_KEYCODE_RIGHT_ALT:
                    case SAPP_KEYCODE_RIGHT_SUPER:
                    case SAPP_KEYCODE_MENU:
                        /* consume the event */
                        break;
                    default:
                        /* forward key to browser */
                        retval = false;
                        break;
                }
            }
            if (_sapp_call_event(&_sapp.event)) {
                /* consume event via sapp_consume_event() */
                retval = true;
            }
            if (send_keyup_followup) {
                _sapp.event.type = SAPP_EVENTTYPE_KEY_UP;
                if (_sapp_call_event(&_sapp.event)) {
                    retval = true;
                }
            }
        }
    }
    _sapp_emsc_update_keyboard_state();
    return retval;
}

_SOKOL_PRIVATE EM_BOOL _sapp_emsc_touch_cb(int emsc_type, const EmscriptenTouchEvent* emsc_event, void* user_data) {
    _SOKOL_UNUSED(user_data);
    bool retval = true;
    if (_sapp_events_enabled()) {
        sapp_event_type type;
        switch (emsc_type) {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
                type = SAPP_EVENTTYPE_TOUCHES_BEGAN;
                break;
            case EMSCRIPTEN_EVENT_TOUCHMOVE:
                type = SAPP_EVENTTYPE_TOUCHES_MOVED;
                break;
            case EMSCRIPTEN_EVENT_TOUCHEND:
                type = SAPP_EVENTTYPE_TOUCHES_ENDED;
                break;
            case EMSCRIPTEN_EVENT_TOUCHCANCEL:
                type = SAPP_EVENTTYPE_TOUCHES_CANCELLED;
                break;
            default:
                type = SAPP_EVENTTYPE_INVALID;
                retval = false;
                break;
        }
        if (type != SAPP_EVENTTYPE_INVALID) {
            _sapp_init_event(type);
            if (emsc_event->ctrlKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_CTRL;
            }
            if (emsc_event->shiftKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_SHIFT;
            }
            if (emsc_event->altKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_ALT;
            }
            if (emsc_event->metaKey) {
                _sapp.event.modifiers |= SAPP_MODIFIER_SUPER;
            }
            _sapp.event.num_touches = emsc_event->numTouches;
            if (_sapp.event.num_touches > SAPP_MAX_TOUCHPOINTS) {
                _sapp.event.num_touches = SAPP_MAX_TOUCHPOINTS;
            }
            for (int i = 0; i < _sapp.event.num_touches; i++) {
                const EmscriptenTouchPoint* src = &emsc_event->touches[i];
                sapp_touchpoint* dst = &_sapp.event.touches[i];
                dst->identifier = src->identifier;
                dst->pos_x = src->targetX * _sapp.dpi_scale;
                dst->pos_y = src->targetY * _sapp.dpi_scale;
                dst->changed = src->isChanged;
            }
            _sapp_call_event(&_sapp.event);
        }
    }
    _sapp_emsc_update_keyboard_state();
    return retval;
}

_SOKOL_PRIVATE void _sapp_emsc_keytable_init(void) {
    _sapp.keycodes[8]   = SAPP_KEYCODE_BACKSPACE;
    _sapp.keycodes[9]   = SAPP_KEYCODE_TAB;
    _sapp.keycodes[13]  = SAPP_KEYCODE_ENTER;
    _sapp.keycodes[16]  = SAPP_KEYCODE_LEFT_SHIFT;
    _sapp.keycodes[17]  = SAPP_KEYCODE_LEFT_CONTROL;
    _sapp.keycodes[18]  = SAPP_KEYCODE_LEFT_ALT;
    _sapp.keycodes[19]  = SAPP_KEYCODE_PAUSE;
    _sapp.keycodes[27]  = SAPP_KEYCODE_ESCAPE;
    _sapp.keycodes[32]  = SAPP_KEYCODE_SPACE;
    _sapp.keycodes[33]  = SAPP_KEYCODE_PAGE_UP;
    _sapp.keycodes[34]  = SAPP_KEYCODE_PAGE_DOWN;
    _sapp.keycodes[35]  = SAPP_KEYCODE_END;
    _sapp.keycodes[36]  = SAPP_KEYCODE_HOME;
    _sapp.keycodes[37]  = SAPP_KEYCODE_LEFT;
    _sapp.keycodes[38]  = SAPP_KEYCODE_UP;
    _sapp.keycodes[39]  = SAPP_KEYCODE_RIGHT;
    _sapp.keycodes[40]  = SAPP_KEYCODE_DOWN;
    _sapp.keycodes[45]  = SAPP_KEYCODE_INSERT;
    _sapp.keycodes[46]  = SAPP_KEYCODE_DELETE;
    _sapp.keycodes[48]  = SAPP_KEYCODE_0;
    _sapp.keycodes[49]  = SAPP_KEYCODE_1;
    _sapp.keycodes[50]  = SAPP_KEYCODE_2;
    _sapp.keycodes[51]  = SAPP_KEYCODE_3;
    _sapp.keycodes[52]  = SAPP_KEYCODE_4;
    _sapp.keycodes[53]  = SAPP_KEYCODE_5;
    _sapp.keycodes[54]  = SAPP_KEYCODE_6;
    _sapp.keycodes[55]  = SAPP_KEYCODE_7;
    _sapp.keycodes[56]  = SAPP_KEYCODE_8;
    _sapp.keycodes[57]  = SAPP_KEYCODE_9;
    _sapp.keycodes[59]  = SAPP_KEYCODE_SEMICOLON;
    _sapp.keycodes[64]  = SAPP_KEYCODE_EQUAL;
    _sapp.keycodes[65]  = SAPP_KEYCODE_A;
    _sapp.keycodes[66]  = SAPP_KEYCODE_B;
    _sapp.keycodes[67]  = SAPP_KEYCODE_C;
    _sapp.keycodes[68]  = SAPP_KEYCODE_D;
    _sapp.keycodes[69]  = SAPP_KEYCODE_E;
    _sapp.keycodes[70]  = SAPP_KEYCODE_F;
    _sapp.keycodes[71]  = SAPP_KEYCODE_G;
    _sapp.keycodes[72]  = SAPP_KEYCODE_H;
    _sapp.keycodes[73]  = SAPP_KEYCODE_I;
    _sapp.keycodes[74]  = SAPP_KEYCODE_J;
    _sapp.keycodes[75]  = SAPP_KEYCODE_K;
    _sapp.keycodes[76]  = SAPP_KEYCODE_L;
    _sapp.keycodes[77]  = SAPP_KEYCODE_M;
    _sapp.keycodes[78]  = SAPP_KEYCODE_N;
    _sapp.keycodes[79]  = SAPP_KEYCODE_O;
    _sapp.keycodes[80]  = SAPP_KEYCODE_P;
    _sapp.keycodes[81]  = SAPP_KEYCODE_Q;
    _sapp.keycodes[82]  = SAPP_KEYCODE_R;
    _sapp.keycodes[83]  = SAPP_KEYCODE_S;
    _sapp.keycodes[84]  = SAPP_KEYCODE_T;
    _sapp.keycodes[85]  = SAPP_KEYCODE_U;
    _sapp.keycodes[86]  = SAPP_KEYCODE_V;
    _sapp.keycodes[87]  = SAPP_KEYCODE_W;
    _sapp.keycodes[88]  = SAPP_KEYCODE_X;
    _sapp.keycodes[89]  = SAPP_KEYCODE_Y;
    _sapp.keycodes[90]  = SAPP_KEYCODE_Z;
    _sapp.keycodes[91]  = SAPP_KEYCODE_LEFT_SUPER;
    _sapp.keycodes[93]  = SAPP_KEYCODE_MENU;
    _sapp.keycodes[96]  = SAPP_KEYCODE_KP_0;
    _sapp.keycodes[97]  = SAPP_KEYCODE_KP_1;
    _sapp.keycodes[98]  = SAPP_KEYCODE_KP_2;
    _sapp.keycodes[99]  = SAPP_KEYCODE_KP_3;
    _sapp.keycodes[100] = SAPP_KEYCODE_KP_4;
    _sapp.keycodes[101] = SAPP_KEYCODE_KP_5;
    _sapp.keycodes[102] = SAPP_KEYCODE_KP_6;
    _sapp.keycodes[103] = SAPP_KEYCODE_KP_7;
    _sapp.keycodes[104] = SAPP_KEYCODE_KP_8;
    _sapp.keycodes[105] = SAPP_KEYCODE_KP_9;
    _sapp.keycodes[106] = SAPP_KEYCODE_KP_MULTIPLY;
    _sapp.keycodes[107] = SAPP_KEYCODE_KP_ADD;
    _sapp.keycodes[109] = SAPP_KEYCODE_KP_SUBTRACT;
    _sapp.keycodes[110] = SAPP_KEYCODE_KP_DECIMAL;
    _sapp.keycodes[111] = SAPP_KEYCODE_KP_DIVIDE;
    _sapp.keycodes[112] = SAPP_KEYCODE_F1;
    _sapp.keycodes[113] = SAPP_KEYCODE_F2;
    _sapp.keycodes[114] = SAPP_KEYCODE_F3;
    _sapp.keycodes[115] = SAPP_KEYCODE_F4;
    _sapp.keycodes[116] = SAPP_KEYCODE_F5;
    _sapp.keycodes[117] = SAPP_KEYCODE_F6;
    _sapp.keycodes[118] = SAPP_KEYCODE_F7;
    _sapp.keycodes[119] = SAPP_KEYCODE_F8;
    _sapp.keycodes[120] = SAPP_KEYCODE_F9;
    _sapp.keycodes[121] = SAPP_KEYCODE_F10;
    _sapp.keycodes[122] = SAPP_KEYCODE_F11;
    _sapp.keycodes[123] = SAPP_KEYCODE_F12;
    _sapp.keycodes[144] = SAPP_KEYCODE_NUM_LOCK;
    _sapp.keycodes[145] = SAPP_KEYCODE_SCROLL_LOCK;
    _sapp.keycodes[173] = SAPP_KEYCODE_MINUS;
    _sapp.keycodes[186] = SAPP_KEYCODE_SEMICOLON;
    _sapp.keycodes[187] = SAPP_KEYCODE_EQUAL;
    _sapp.keycodes[188] = SAPP_KEYCODE_COMMA;
    _sapp.keycodes[189] = SAPP_KEYCODE_MINUS;
    _sapp.keycodes[190] = SAPP_KEYCODE_PERIOD;
    _sapp.keycodes[191] = SAPP_KEYCODE_SLASH;
    _sapp.keycodes[192] = SAPP_KEYCODE_GRAVE_ACCENT;
    _sapp.keycodes[219] = SAPP_KEYCODE_LEFT_BRACKET;
    _sapp.keycodes[220] = SAPP_KEYCODE_BACKSLASH;
    _sapp.keycodes[221] = SAPP_KEYCODE_RIGHT_BRACKET;
    _sapp.keycodes[222] = SAPP_KEYCODE_APOSTROPHE;
    _sapp.keycodes[224] = SAPP_KEYCODE_LEFT_SUPER;
}

#if defined(SOKOL_GLES2) || defined(SOKOL_GLES3)
_SOKOL_PRIVATE EM_BOOL _sapp_emsc_webgl_context_cb(int emsc_type, const void* reserved, void* user_data) {
    _SOKOL_UNUSED(reserved);
    _SOKOL_UNUSED(user_data);
    sapp_event_type type;
    switch (emsc_type) {
        case EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST:     type = SAPP_EVENTTYPE_SUSPENDED; break;
        case EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED: type = SAPP_EVENTTYPE_RESUMED; break;
        default:                                    type = SAPP_EVENTTYPE_INVALID; break;
    }
    if (_sapp_events_enabled() && (SAPP_EVENTTYPE_INVALID != type)) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
    return true;
}

_SOKOL_PRIVATE void _sapp_emsc_webgl_init(void) {
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.alpha = _sapp.desc.alpha;
    attrs.depth = true;
    attrs.stencil = true;
    attrs.antialias = _sapp.sample_count > 1;
    attrs.premultipliedAlpha = _sapp.desc.html5_premultiplied_alpha;
    attrs.preserveDrawingBuffer = _sapp.desc.html5_preserve_drawing_buffer;
    attrs.enableExtensionsByDefault = true;
    #if defined(SOKOL_GLES3)
        if (_sapp.desc.gl_force_gles2) {
            attrs.majorVersion = 1;
            _sapp.gles2_fallback = true;
        }
        else {
            attrs.majorVersion = 2;
        }
    #endif
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(_sapp.html5_canvas_name, &attrs);
    if (!ctx) {
        attrs.majorVersion = 1;
        ctx = emscripten_webgl_create_context(_sapp.html5_canvas_name, &attrs);
        _sapp.gles2_fallback = true;
    }
    emscripten_webgl_make_context_current(ctx);

    /* some WebGL extension are not enabled automatically by emscripten */
    emscripten_webgl_enable_extension(ctx, "WEBKIT_WEBGL_compressed_texture_pvrtc");
}
#endif

#if defined(SOKOL_WGPU)
#define _SAPP_EMSC_WGPU_STATE_INITIAL (0)
#define _SAPP_EMSC_WGPU_STATE_READY (1)
#define _SAPP_EMSC_WGPU_STATE_RUNNING (2)

/* called when the asynchronous WebGPU device + swapchain init code in JS has finished */
EMSCRIPTEN_KEEPALIVE void _sapp_emsc_wgpu_ready(int device_id, int swapchain_id, int swapchain_fmt) {
    SOKOL_ASSERT(0 == _sapp_emsc.wgpu.device);
    _sapp_emsc.wgpu.device = (WGPUDevice) device_id;
    _sapp_emsc.wgpu.swapchain = (WGPUSwapChain) swapchain_id;
    _sapp_emsc.wgpu.render_format = (WGPUTextureFormat) swapchain_fmt;
    _sapp_emsc.wgpu.state = _SAPP_EMSC_WGPU_STATE_READY;
}

/* embedded JS function to handle all the asynchronous WebGPU setup */
EM_JS(void, _sapp_emsc_wgpu_init, (), {
    WebGPU.initManagers();
    // FIXME: the extension activation must be more clever here
    navigator.gpu.requestAdapter().then(function(adapter) {
        console.log("wgpu adapter extensions: " + adapter.extensions);
        adapter.requestDevice({ extensions: ["textureCompressionBC"]}).then(function(device) {
            var gpuContext = document.getElementById("canvas").getContext("gpupresent");
            console.log("wgpu device extensions: " + adapter.extensions);
            gpuContext.getSwapChainPreferredFormat(device).then(function(fmt) {
                var swapChainDescriptor = { device: device, format: fmt };
                var swapChain = gpuContext.configureSwapChain(swapChainDescriptor);
                var deviceId = WebGPU.mgrDevice.create(device);
                var swapChainId = WebGPU.mgrSwapChain.create(swapChain);
                var fmtId = WebGPU.TextureFormat.findIndex(function(elm) { return elm==fmt; });
                console.log("wgpu device: " + device);
                console.log("wgpu swap chain: " + swapChain);
                console.log("wgpu preferred format: " + fmt + " (" + fmtId + ")");
                __sapp_emsc_wgpu_ready(deviceId, swapChainId, fmtId);
            });
        });
    });
});

_SOKOL_PRIVATE void _sapp_emsc_wgpu_surfaces_create(void) {
    SOKOL_ASSERT(_sapp_emsc.wgpu.device);
    SOKOL_ASSERT(_sapp_emsc.wgpu.swapchain);
    SOKOL_ASSERT(0 == _sapp_emsc.wgpu.depth_stencil_tex);
    SOKOL_ASSERT(0 == _sapp_emsc.wgpu.depth_stencil_view);
    SOKOL_ASSERT(0 == _sapp_emsc.wgpu.msaa_tex);
    SOKOL_ASSERT(0 == _sapp_emsc.wgpu.msaa_view);

    WGPUTextureDescriptor ds_desc;
    memset(&ds_desc, 0, sizeof(ds_desc));
    ds_desc.usage = WGPUTextureUsage_OutputAttachment;
    ds_desc.dimension = WGPUTextureDimension_2D;
    ds_desc.size.width = (uint32_t) _sapp.framebuffer_width;
    ds_desc.size.height = (uint32_t) _sapp.framebuffer_height;
    ds_desc.size.depth = 1;
    ds_desc.arrayLayerCount = 1;
    ds_desc.format = WGPUTextureFormat_Depth24PlusStencil8;
    ds_desc.mipLevelCount = 1;
    ds_desc.sampleCount = _sapp.sample_count;
    _sapp_emsc.wgpu.depth_stencil_tex = wgpuDeviceCreateTexture(_sapp_emsc.wgpu.device, &ds_desc);
    _sapp_emsc.wgpu.depth_stencil_view = wgpuTextureCreateView(_sapp_emsc.wgpu.depth_stencil_tex, 0);

    if (_sapp.sample_count > 1) {
        WGPUTextureDescriptor msaa_desc;
        memset(&msaa_desc, 0, sizeof(msaa_desc));
        msaa_desc.usage = WGPUTextureUsage_OutputAttachment;
        msaa_desc.dimension = WGPUTextureDimension_2D;
        msaa_desc.size.width = (uint32_t) _sapp.framebuffer_width;
        msaa_desc.size.height = (uint32_t) _sapp.framebuffer_height;
        msaa_desc.size.depth = 1;
        msaa_desc.arrayLayerCount = 1;
        msaa_desc.format = _sapp_emsc.wgpu.render_format;
        msaa_desc.mipLevelCount = 1;
        msaa_desc.sampleCount = _sapp.sample_count;
        _sapp_emsc.wgpu.msaa_tex = wgpuDeviceCreateTexture(_sapp_emsc.wgpu.device, &msaa_desc);
        _sapp_emsc.wgpu.msaa_view = wgpuTextureCreateView(_sapp_emsc.wgpu.msaa_tex, 0);
    }
}

_SOKOL_PRIVATE void _sapp_emsc_wgpu_surfaces_discard(void) {
    if (_sapp_emsc.wgpu.msaa_tex) {
        wgpuTextureRelease(_sapp_emsc.wgpu.msaa_tex);
        _sapp_emsc.wgpu.msaa_tex = 0;
    }
    if (_sapp_emsc.wgpu.msaa_view) {
        wgpuTextureViewRelease(_sapp_emsc.wgpu.msaa_view);
        _sapp_emsc.wgpu.msaa_view = 0;
    }
    if (_sapp_emsc.wgpu.depth_stencil_tex) {
        wgpuTextureRelease(_sapp_emsc.wgpu.depth_stencil_tex);
        _sapp_emsc.wgpu.depth_stencil_tex = 0;
    }
    if (_sapp_emsc.wgpu.depth_stencil_view) {
        wgpuTextureViewRelease(_sapp_emsc.wgpu.depth_stencil_view);
        _sapp_emsc.wgpu.depth_stencil_view = 0;
    }
}

_SOKOL_PRIVATE void _sapp_emsc_wgpu_next_frame(void) {
    if (_sapp_emsc.wgpu.swapchain_view) {
        wgpuTextureViewRelease(_sapp_emsc.wgpu.swapchain_view);
    }
    _sapp_emsc.wgpu.swapchain_view = wgpuSwapChainGetCurrentTextureView(_sapp_emsc.wgpu.swapchain);
}
#endif

_SOKOL_PRIVATE void _sapp_emsc_register_eventhandlers(void) {
    emscripten_set_mousedown_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_mouse_cb);
    emscripten_set_mouseup_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_mouse_cb);
    emscripten_set_mousemove_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_mouse_cb);
    emscripten_set_mouseenter_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_mouse_cb);
    emscripten_set_mouseleave_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_mouse_cb);
    emscripten_set_wheel_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_wheel_cb);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, _sapp_emsc_key_cb);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, _sapp_emsc_key_cb);
    emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, _sapp_emsc_key_cb);
    emscripten_set_touchstart_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_touch_cb);
    emscripten_set_touchmove_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_touch_cb);
    emscripten_set_touchend_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_touch_cb);
    emscripten_set_touchcancel_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_touch_cb);
    sapp_add_js_hook_beforeunload();
    if (_sapp.clipboard_enabled) {
        sapp_add_js_hook_clipboard();
    }
    #if defined(SOKOL_GLES2) || defined(SOKOL_GLES3)
        emscripten_set_webglcontextlost_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_webgl_context_cb);
        emscripten_set_webglcontextrestored_callback(_sapp.html5_canvas_name, 0, true, _sapp_emsc_webgl_context_cb);
    #endif
}

_SOKOL_PRIVATE void _sapp_emsc_unregister_eventhandlers() {
    emscripten_set_mousedown_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_mouseup_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_mousemove_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_mouseenter_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_mouseleave_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_wheel_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, 0);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, 0);
    emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, 0);
    emscripten_set_touchstart_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_touchmove_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_touchend_callback(_sapp.html5_canvas_name, 0, true, 0);
    emscripten_set_touchcancel_callback(_sapp.html5_canvas_name, 0, true, 0);
    sapp_remove_js_hook_beforeunload();
    if (_sapp.clipboard_enabled) {
        sapp_remove_js_hook_clipboard();
    }
    #if defined(SOKOL_GLES2) || defined(SOKOL_GLES3)
        emscripten_set_webglcontextlost_callback(_sapp.html5_canvas_name, 0, true, 0);
        emscripten_set_webglcontextrestored_callback(_sapp.html5_canvas_name, 0, true, 0);
    #endif
}

_SOKOL_PRIVATE EM_BOOL _sapp_emsc_frame(double time, void* userData) {
    _SOKOL_UNUSED(time);
    _SOKOL_UNUSED(userData);

    #if defined(SOKOL_WGPU)
        /*
            on WebGPU, the emscripten frame callback will already be called while
            the asynchronous WebGPU device and swapchain initialization is still
            in progress
        */
        switch (_sapp_emsc.wgpu.state) {
            case _SAPP_EMSC_WGPU_STATE_INITIAL:
                /* async JS init hasn't finished yet */
                break;
            case _SAPP_EMSC_WGPU_STATE_READY:
                /* perform post-async init stuff */
                _sapp_emsc_wgpu_surfaces_create();
                _sapp_emsc.wgpu.state = _SAPP_EMSC_WGPU_STATE_RUNNING;
                break;
            case _SAPP_EMSC_WGPU_STATE_RUNNING:
                /* a regular frame */
                _sapp_emsc_wgpu_next_frame();
                _sapp_frame();
                break;
        }
    #else
        /* WebGL code path */
        _sapp_frame();
    #endif

    /* quit-handling */
    if (_sapp.quit_requested) {
        _sapp_init_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
        _sapp_call_event(&_sapp.event);
        if (_sapp.quit_requested) {
            _sapp.quit_ordered = true;
        }
    }
    if (_sapp.quit_ordered) {
        _sapp_emsc_unregister_eventhandlers();
        _sapp_call_cleanup();
        return EM_FALSE;
    }
    return EM_TRUE;
}

_SOKOL_PRIVATE void _sapp_run(const sapp_desc* desc) {
    memset(&_sapp_emsc, 0, sizeof(_sapp_emsc));
    _sapp_init_state(desc);
    _sapp_emsc_keytable_init();
    double w, h;
    if (_sapp.desc.html5_canvas_resize) {
        w = (double) _sapp.desc.width;
        h = (double) _sapp.desc.height;
    }
    else {
        emscripten_get_element_css_size(_sapp.html5_canvas_name, &w, &h);
        emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false, _sapp_emsc_size_changed);
    }
    if (_sapp.desc.high_dpi) {
        _sapp.dpi_scale = emscripten_get_device_pixel_ratio();
    }
    _sapp.window_width = (int) w;
    _sapp.window_height = (int) h;
    _sapp.framebuffer_width = (int) (w * _sapp.dpi_scale);
    _sapp.framebuffer_height = (int) (h * _sapp.dpi_scale);
    emscripten_set_canvas_element_size(_sapp.html5_canvas_name, _sapp.framebuffer_width, _sapp.framebuffer_height);
    #if defined(SOKOL_GLES2) || defined(SOKOL_GLES3)
        _sapp_emsc_webgl_init();
    #elif defined(SOKOL_WGPU)
        _sapp_emsc_wgpu_init();
    #endif
    _sapp.valid = true;
    _sapp_emsc_register_eventhandlers();

    /* start the frame loop */
    emscripten_request_animation_frame_loop(_sapp_emsc_frame, 0);

    /* NOT A BUG: do not call _sapp_discard_state() */
}

#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */
#endif /* __EMSCRIPTEN__ */

/*== MISC GL SUPPORT FUNCTIONS ================================================*/
#if defined(SOKOL_GLCORE33)
typedef struct {
    int         red_bits;
    int         green_bits;
    int         blue_bits;
    int         alpha_bits;
    int         depth_bits;
    int         stencil_bits;
    int         samples;
    bool        doublebuffer;
    uintptr_t   handle;
} _sapp_gl_fbconfig;

_SOKOL_PRIVATE void _sapp_gl_init_fbconfig(_sapp_gl_fbconfig* fbconfig) {
    memset(fbconfig, 0, sizeof(_sapp_gl_fbconfig));
    /* -1 means "don't care" */
    fbconfig->red_bits = -1;
    fbconfig->green_bits = -1;
    fbconfig->blue_bits = -1;
    fbconfig->alpha_bits = -1;
    fbconfig->depth_bits = -1;
    fbconfig->stencil_bits = -1;
    fbconfig->samples = -1;
}

_SOKOL_PRIVATE const _sapp_gl_fbconfig* _sapp_gl_choose_fbconfig(const _sapp_gl_fbconfig* desired, const _sapp_gl_fbconfig* alternatives, unsigned int count) {
    unsigned int i;
    unsigned int missing, least_missing = 1000000;
    unsigned int color_diff, least_color_diff = 10000000;
    unsigned int extra_diff, least_extra_diff = 10000000;
    const _sapp_gl_fbconfig* current;
    const _sapp_gl_fbconfig* closest = NULL;
    for (i = 0;  i < count;  i++) {
        current = alternatives + i;
        if (desired->doublebuffer != current->doublebuffer) {
            continue;
        }
        missing = 0;
        if (desired->alpha_bits > 0 && current->alpha_bits == 0) {
            missing++;
        }
        if (desired->depth_bits > 0 && current->depth_bits == 0) {
            missing++;
        }
        if (desired->stencil_bits > 0 && current->stencil_bits == 0) {
            missing++;
        }
        if (desired->samples > 0 && current->samples == 0) {
            /* Technically, several multisampling buffers could be
                involved, but that's a lower level implementation detail and
                not important to us here, so we count them as one
            */
            missing++;
        }

        /* These polynomials make many small channel size differences matter
            less than one large channel size difference
            Calculate color channel size difference value
        */
        color_diff = 0;
        if (desired->red_bits != -1) {
            color_diff += (desired->red_bits - current->red_bits) * (desired->red_bits - current->red_bits);
        }
        if (desired->green_bits != -1) {
            color_diff += (desired->green_bits - current->green_bits) * (desired->green_bits - current->green_bits);
        }
        if (desired->blue_bits != -1) {
            color_diff += (desired->blue_bits - current->blue_bits) * (desired->blue_bits - current->blue_bits);
        }

        /* Calculate non-color channel size difference value */
        extra_diff = 0;
        if (desired->alpha_bits != -1) {
            extra_diff += (desired->alpha_bits - current->alpha_bits) * (desired->alpha_bits - current->alpha_bits);
        }
        if (desired->depth_bits != -1) {
            extra_diff += (desired->depth_bits - current->depth_bits) * (desired->depth_bits - current->depth_bits);
        }
        if (desired->stencil_bits != -1) {
            extra_diff += (desired->stencil_bits - current->stencil_bits) * (desired->stencil_bits - current->stencil_bits);
        }
        if (desired->samples != -1) {
            extra_diff += (desired->samples - current->samples) * (desired->samples - current->samples);
        }

        /* Figure out if the current one is better than the best one found so far
            Least number of missing buffers is the most important heuristic,
            then color buffer size match and lastly size match for other buffers
        */
        if (missing < least_missing) {
            closest = current;
        }
        else if (missing == least_missing) {
            if ((color_diff < least_color_diff) ||
                (color_diff == least_color_diff && extra_diff < least_extra_diff))
            {
                closest = current;
            }
        }
        if (current == closest) {
            least_missing = missing;
            least_color_diff = color_diff;
            least_extra_diff = extra_diff;
        }
    }
    return closest;
}
#endif

/*== WINDOWS ==================================================================*/
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#pragma comment (lib, "Shell32.lib")

#if !defined(SOKOL_WIN32_FORCE_MAIN)
#pragma comment (linker, "/subsystem:windows")
#endif

#if (defined(WINAPI_FAMILY_PARTITION) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP))
#pragma comment (lib, "WindowsApp.lib")
#else
#pragma comment (lib, "user32.lib")
#if defined(SOKOL_D3D11)
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")
#endif
#if defined(SOKOL_GLCORE33)
#pragma comment (lib, "gdi32.lib")
#endif
#endif

#if defined(SOKOL_D3D11)
#ifndef D3D11_NO_HELPERS
#define D3D11_NO_HELPERS
#endif
#ifndef CINTERFACE
#define CINTERFACE
#endif
#ifndef COBJMACROS
#define COBJMACROS
#endif
#include <d3d11.h>
#include <dxgi.h>
#endif

/* see https://github.com/floooh/sokol/issues/138 */
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL (0x020E)
#endif

#ifndef DPI_ENUMS_DECLARED
typedef enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#endif /*DPI_ENUMS_DECLARED*/

static HWND _sapp_win32_hwnd;
static HDC _sapp_win32_dc;
static bool _sapp_win32_in_create_window;
static bool _sapp_win32_dpi_aware;
static float _sapp_win32_content_scale;
static float _sapp_win32_window_scale;
static float _sapp_win32_mouse_scale;
static bool _sapp_win32_iconified;
typedef BOOL(WINAPI * SETPROCESSDPIAWARE_T)(void);
typedef HRESULT(WINAPI * SETPROCESSDPIAWARENESS_T)(PROCESS_DPI_AWARENESS);
typedef HRESULT(WINAPI * GETDPIFORMONITOR_T)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
static SETPROCESSDPIAWARE_T _sapp_win32_setprocessdpiaware;
static SETPROCESSDPIAWARENESS_T _sapp_win32_setprocessdpiawareness;
static GETDPIFORMONITOR_T _sapp_win32_getdpiformonitor;
#if defined(SOKOL_D3D11)
static ID3D11Device* _sapp_d3d11_device;
static ID3D11DeviceContext* _sapp_d3d11_device_context;
static DXGI_SWAP_CHAIN_DESC _sapp_dxgi_swap_chain_desc;
static IDXGISwapChain* _sapp_dxgi_swap_chain;
static ID3D11Texture2D* _sapp_d3d11_rt;
static ID3D11RenderTargetView* _sapp_d3d11_rtv;
static ID3D11Texture2D* _sapp_d3d11_ds;
static ID3D11DepthStencilView* _sapp_d3d11_dsv;
#endif
#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201a
#define WGL_ALPHA_BITS_ARB 0x201b
#define WGL_ALPHA_SHIFT_ARB 0x201c
#define WGL_ACCUM_BITS_ARB 0x201d
#define WGL_ACCUM_RED_BITS_ARB 0x201e
#define WGL_ACCUM_GREEN_BITS_ARB 0x201f
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_STEREO_ARB 0x2012
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_SAMPLES_ARB 0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_COLORSPACE_EXT 0x309d
#define WGL_COLORSPACE_SRGB_EXT 0x3089
#define ERROR_INVALID_VERSION_ARB 0x2095
#define ERROR_INVALID_PROFILE_ARB 0x2096
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054
typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC,int,int,UINT,const int*,int*);
typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC,HGLRC,const int*);
typedef HGLRC (WINAPI * PFN_wglCreateContext)(HDC);
typedef BOOL (WINAPI * PFN_wglDeleteContext)(HGLRC);
typedef PROC (WINAPI * PFN_wglGetProcAddress)(LPCSTR);
typedef HDC (WINAPI * PFN_wglGetCurrentDC)(void);
typedef BOOL (WINAPI * PFN_wglMakeCurrent)(HDC,HGLRC);
static HINSTANCE _sapp_opengl32;
static HGLRC _sapp_gl_ctx;
static PFN_wglCreateContext _sapp_wglCreateContext;
static PFN_wglDeleteContext _sapp_wglDeleteContext;
static PFN_wglGetProcAddress _sapp_wglGetProcAddress;
static PFN_wglGetCurrentDC _sapp_wglGetCurrentDC;
static PFN_wglMakeCurrent _sapp_wglMakeCurrent;
static PFNWGLSWAPINTERVALEXTPROC _sapp_SwapIntervalEXT;
static PFNWGLGETPIXELFORMATATTRIBIVARBPROC _sapp_GetPixelFormatAttribivARB;
static PFNWGLGETEXTENSIONSSTRINGEXTPROC _sapp_GetExtensionsStringEXT;
static PFNWGLGETEXTENSIONSSTRINGARBPROC _sapp_GetExtensionsStringARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC _sapp_CreateContextAttribsARB;
static bool _sapp_ext_swap_control;
static bool _sapp_arb_multisample;
static bool _sapp_arb_pixel_format;
static bool _sapp_arb_create_context;
static bool _sapp_arb_create_context_profile;
static HWND _sapp_win32_msg_hwnd;
static HDC _sapp_win32_msg_dc;

/* NOTE: the optional GL loader only contains the GL constants and functions required for sokol_gfx.h, if you need
more, you'll need to use you own gl header-generator/loader
*/
#if !defined(SOKOL_WIN32_NO_GL_LOADER)
#if defined(SOKOL_GLCORE33)
#define __gl_h_ 1
#define __gl32_h_ 1
#define __gl31_h_ 1
#define __GL_H__ 1
#define __glext_h_ 1
#define __GLEXT_H_ 1
#define __gltypes_h_ 1
#define __glcorearb_h_ 1
#define __gl_glcorearb_h_ 1
#define GL_APIENTRY APIENTRY

typedef unsigned int  GLenum;
typedef unsigned int  GLuint;
typedef int  GLsizei;
typedef char  GLchar;
typedef ptrdiff_t  GLintptr;
typedef ptrdiff_t  GLsizeiptr;
typedef double  GLclampd;
typedef unsigned short  GLushort;
typedef unsigned char  GLubyte;
typedef unsigned char  GLboolean;
typedef uint64_t  GLuint64;
typedef double  GLdouble;
typedef unsigned short  GLhalf;
typedef float  GLclampf;
typedef unsigned int  GLbitfield;
typedef signed char  GLbyte;
typedef short  GLshort;
typedef void  GLvoid;
typedef int64_t  GLint64;
typedef float  GLfloat;
typedef struct __GLsync * GLsync;
typedef int  GLint;
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_R32F 0x822E
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_R16F 0x822D
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_NUM_EXTENSIONS 0x821D
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_VERTEX_SHADER 0x8B31
#define GL_INCR 0x1E02
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STATIC_DRAW 0x88E4
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_CONSTANT_COLOR 0x8001
#define GL_DECR_WRAP 0x8508
#define GL_R8 0x8229
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_SHORT 0x1402
#define GL_DEPTH_TEST 0x0B71
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_LINK_STATUS 0x8B82
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_RGBA16F 0x881A
#define GL_CONSTANT_ALPHA 0x8003
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_STREAM_DRAW 0x88E0
#define GL_ONE 1
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_RGB10_A2 0x8059
#define GL_RGBA8 0x8058
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_RGBA4 0x8056
#define GL_RGB8 0x8051
#define GL_ARRAY_BUFFER 0x8892
#define GL_STENCIL 0x1802
#define GL_TEXTURE_2D 0x0DE1
#define GL_DEPTH 0x1801
#define GL_FRONT 0x0404
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_REPEAT 0x2901
#define GL_RGBA 0x1908
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_DECR 0x1E03
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_FLOAT 0x1406
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_DEPTH_COMPONENT 0x1902
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_COLOR 0x1800
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_BYTE 0x1401
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_NONE 0
#define GL_SRC_COLOR 0x0300
#define GL_BYTE 0x1400
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_LINE_STRIP 0x0003
#define GL_TEXTURE_3D 0x806F
#define GL_CW 0x0900
#define GL_LINEAR 0x2601
#define GL_RENDERBUFFER 0x8D41
#define GL_GEQUAL 0x0206
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_RGBA32F 0x8814
#define GL_BLEND 0x0BE2
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_EXTENSIONS 0x1F03
#define GL_NO_ERROR 0
#define GL_REPLACE 0x1E01
#define GL_KEEP 0x1E00
#define GL_CCW 0x0901
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_RGB 0x1907
#define GL_TRIANGLE_STRIP 0x0005
#define GL_FALSE 0
#define GL_ZERO 0
#define GL_CULL_FACE 0x0B44
#define GL_INVERT 0x150A
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_SHORT 0x1403
#define GL_NEAREST 0x2600
#define GL_SCISSOR_TEST 0x0C11
#define GL_LEQUAL 0x0203
#define GL_STENCIL_TEST 0x0B90
#define GL_DITHER 0x0BD0
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_EQUAL 0x0202
#define GL_FRAMEBUFFER 0x8D40
#define GL_RGB5 0x8050
#define GL_LINES 0x0001
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_SRC_ALPHA 0x0302
#define GL_INCR_WRAP 0x8507
#define GL_LESS 0x0201
#define GL_MULTISAMPLE 0x809D
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_BACK 0x0405
#define GL_ALWAYS 0x0207
#define GL_FUNC_ADD 0x8006
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_NOTEQUAL 0x0205
#define GL_DST_COLOR 0x0306
#define GL_COMPILE_STATUS 0x8B81
#define GL_RED 0x1903
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_DST_ALPHA 0x0304
#define GL_RGB5_A1 0x8057
#define GL_GREATER 0x0204
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_TRUE 1
#define GL_NEVER 0x0200
#define GL_POINTS 0x0000
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_MIRRORED_REPEAT 0x8370
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RED_INTEGER 0x8D94
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_RGBA_INTEGER 0x8D99
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGB8_SNORM 0x8F96
#define GL_RGBA8_SNORM 0x8F97
#define GL_R16_SNORM 0x8F98
#define GL_RG16_SNORM 0x8F99
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGBA16_SNORM 0x8F9B
#define GL_RGBA16 0x805B
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_CURRENT_PROGRAM 0x8B8D

typedef void  (GL_APIENTRY *PFN_glBindVertexArray)(GLuint array);
static PFN_glBindVertexArray _sapp_glBindVertexArray;
typedef void  (GL_APIENTRY *PFN_glFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
static PFN_glFramebufferTextureLayer _sapp_glFramebufferTextureLayer;
typedef void  (GL_APIENTRY *PFN_glGenFramebuffers)(GLsizei n, GLuint * framebuffers);
static PFN_glGenFramebuffers _sapp_glGenFramebuffers;
typedef void  (GL_APIENTRY *PFN_glBindFramebuffer)(GLenum target, GLuint framebuffer);
static PFN_glBindFramebuffer _sapp_glBindFramebuffer;
typedef void  (GL_APIENTRY *PFN_glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
static PFN_glBindRenderbuffer _sapp_glBindRenderbuffer;
typedef const GLubyte * (GL_APIENTRY *PFN_glGetStringi)(GLenum name, GLuint index);
static PFN_glGetStringi _sapp_glGetStringi;
typedef void  (GL_APIENTRY *PFN_glClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
static PFN_glClearBufferfi _sapp_glClearBufferfi;
typedef void  (GL_APIENTRY *PFN_glClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat * value);
static PFN_glClearBufferfv _sapp_glClearBufferfv;
typedef void  (GL_APIENTRY *PFN_glClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint * value);
static PFN_glClearBufferuiv _sapp_glClearBufferuiv;
typedef void  (GL_APIENTRY *PFN_glDeleteRenderbuffers)(GLsizei n, const GLuint * renderbuffers);
static PFN_glDeleteRenderbuffers _sapp_glDeleteRenderbuffers;
typedef void  (GL_APIENTRY *PFN_glUniform4fv)(GLint location, GLsizei count, const GLfloat * value);
static PFN_glUniform4fv _sapp_glUniform4fv;
typedef void  (GL_APIENTRY *PFN_glUniform2fv)(GLint location, GLsizei count, const GLfloat * value);
static PFN_glUniform2fv _sapp_glUniform2fv;
typedef void  (GL_APIENTRY *PFN_glUseProgram)(GLuint program);
static PFN_glUseProgram _sapp_glUseProgram;
typedef void  (GL_APIENTRY *PFN_glShaderSource)(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length);
static PFN_glShaderSource _sapp_glShaderSource;
typedef void  (GL_APIENTRY *PFN_glLinkProgram)(GLuint program);
static PFN_glLinkProgram _sapp_glLinkProgram;
typedef GLint (GL_APIENTRY *PFN_glGetUniformLocation)(GLuint program, const GLchar * name);
static PFN_glGetUniformLocation _sapp_glGetUniformLocation;
typedef void  (GL_APIENTRY *PFN_glGetShaderiv)(GLuint shader, GLenum pname, GLint * params);
static PFN_glGetShaderiv _sapp_glGetShaderiv;
typedef void  (GL_APIENTRY *PFN_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
static PFN_glGetProgramInfoLog _sapp_glGetProgramInfoLog;
typedef GLint (GL_APIENTRY *PFN_glGetAttribLocation)(GLuint program, const GLchar * name);
static PFN_glGetAttribLocation _sapp_glGetAttribLocation;
typedef void  (GL_APIENTRY *PFN_glDisableVertexAttribArray)(GLuint index);
static PFN_glDisableVertexAttribArray _sapp_glDisableVertexAttribArray;
typedef void  (GL_APIENTRY *PFN_glDeleteShader)(GLuint shader);
static PFN_glDeleteShader _sapp_glDeleteShader;
typedef void  (GL_APIENTRY *PFN_glDeleteProgram)(GLuint program);
static PFN_glDeleteProgram _sapp_glDeleteProgram;
typedef void  (GL_APIENTRY *PFN_glCompileShader)(GLuint shader);
static PFN_glCompileShader _sapp_glCompileShader;
typedef void  (GL_APIENTRY *PFN_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
static PFN_glStencilFuncSeparate _sapp_glStencilFuncSeparate;
typedef void  (GL_APIENTRY *PFN_glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
static PFN_glStencilOpSeparate _sapp_glStencilOpSeparate;
typedef void  (GL_APIENTRY *PFN_glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
static PFN_glRenderbufferStorageMultisample _sapp_glRenderbufferStorageMultisample;
typedef void  (GL_APIENTRY *PFN_glDrawBuffers)(GLsizei n, const GLenum * bufs);
static PFN_glDrawBuffers _sapp_glDrawBuffers;
typedef void  (GL_APIENTRY *PFN_glVertexAttribDivisor)(GLuint index, GLuint divisor);
static PFN_glVertexAttribDivisor _sapp_glVertexAttribDivisor;
typedef void  (GL_APIENTRY *PFN_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void * data);
static PFN_glBufferSubData _sapp_glBufferSubData;
typedef void  (GL_APIENTRY *PFN_glGenBuffers)(GLsizei n, GLuint * buffers);
static PFN_glGenBuffers _sapp_glGenBuffers;
typedef GLenum (GL_APIENTRY *PFN_glCheckFramebufferStatus)(GLenum target);
static PFN_glCheckFramebufferStatus _sapp_glCheckFramebufferStatus;
typedef void  (GL_APIENTRY *PFN_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
static PFN_glFramebufferRenderbuffer _sapp_glFramebufferRenderbuffer;
typedef void  (GL_APIENTRY *PFN_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void * data);
static PFN_glCompressedTexImage2D _sapp_glCompressedTexImage2D;
typedef void  (GL_APIENTRY *PFN_glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void * data);
static PFN_glCompressedTexImage3D _sapp_glCompressedTexImage3D;
typedef void  (GL_APIENTRY *PFN_glActiveTexture)(GLenum texture);
static PFN_glActiveTexture _sapp_glActiveTexture;
typedef void  (GL_APIENTRY *PFN_glTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels);
static PFN_glTexSubImage3D _sapp_glTexSubImage3D;
typedef void  (GL_APIENTRY *PFN_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
static PFN_glUniformMatrix4fv _sapp_glUniformMatrix4fv;
typedef void  (GL_APIENTRY *PFN_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
static PFN_glRenderbufferStorage _sapp_glRenderbufferStorage;
typedef void  (GL_APIENTRY *PFN_glGenTextures)(GLsizei n, GLuint * textures);
static PFN_glGenTextures _sapp_glGenTextures;
typedef void  (GL_APIENTRY *PFN_glPolygonOffset)(GLfloat factor, GLfloat units);
static PFN_glPolygonOffset _sapp_glPolygonOffset;
typedef void  (GL_APIENTRY *PFN_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void * indices);
static PFN_glDrawElements _sapp_glDrawElements;
typedef void  (GL_APIENTRY *PFN_glDeleteFramebuffers)(GLsizei n, const GLuint * framebuffers);
static PFN_glDeleteFramebuffers _sapp_glDeleteFramebuffers;
typedef void  (GL_APIENTRY *PFN_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
static PFN_glBlendEquationSeparate _sapp_glBlendEquationSeparate;
typedef void  (GL_APIENTRY *PFN_glDeleteTextures)(GLsizei n, const GLuint * textures);
static PFN_glDeleteTextures _sapp_glDeleteTextures;
typedef void  (GL_APIENTRY *PFN_glGetProgramiv)(GLuint program, GLenum pname, GLint * params);
static PFN_glGetProgramiv _sapp_glGetProgramiv;
typedef void  (GL_APIENTRY *PFN_glBindTexture)(GLenum target, GLuint texture);
static PFN_glBindTexture _sapp_glBindTexture;
typedef void  (GL_APIENTRY *PFN_glTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels);
static PFN_glTexImage3D _sapp_glTexImage3D;
typedef GLuint (GL_APIENTRY *PFN_glCreateShader)(GLenum type);
static PFN_glCreateShader _sapp_glCreateShader;
typedef void  (GL_APIENTRY *PFN_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels);
static PFN_glTexSubImage2D _sapp_glTexSubImage2D;
typedef void  (GL_APIENTRY *PFN_glClearDepth)(GLdouble depth);
static PFN_glClearDepth _sapp_glClearDepth;
typedef void  (GL_APIENTRY *PFN_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
static PFN_glFramebufferTexture2D _sapp_glFramebufferTexture2D;
typedef GLuint (GL_APIENTRY *PFN_glCreateProgram)();
static PFN_glCreateProgram _sapp_glCreateProgram;
typedef void  (GL_APIENTRY *PFN_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
static PFN_glViewport _sapp_glViewport;
typedef void  (GL_APIENTRY *PFN_glDeleteBuffers)(GLsizei n, const GLuint * buffers);
static PFN_glDeleteBuffers _sapp_glDeleteBuffers;
typedef void  (GL_APIENTRY *PFN_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
static PFN_glDrawArrays _sapp_glDrawArrays;
typedef void  (GL_APIENTRY *PFN_glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount);
static PFN_glDrawElementsInstanced _sapp_glDrawElementsInstanced;
typedef void  (GL_APIENTRY *PFN_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
static PFN_glVertexAttribPointer _sapp_glVertexAttribPointer;
typedef void  (GL_APIENTRY *PFN_glUniform1i)(GLint location, GLint v0);
static PFN_glUniform1i _sapp_glUniform1i;
typedef void  (GL_APIENTRY *PFN_glDisable)(GLenum cap);
static PFN_glDisable _sapp_glDisable;
typedef void  (GL_APIENTRY *PFN_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
static PFN_glColorMask _sapp_glColorMask;
typedef void  (GL_APIENTRY *PFN_glBindBuffer)(GLenum target, GLuint buffer);
static PFN_glBindBuffer _sapp_glBindBuffer;
typedef void  (GL_APIENTRY *PFN_glDeleteVertexArrays)(GLsizei n, const GLuint * arrays);
static PFN_glDeleteVertexArrays _sapp_glDeleteVertexArrays;
typedef void  (GL_APIENTRY *PFN_glDepthMask)(GLboolean flag);
static PFN_glDepthMask _sapp_glDepthMask;
typedef void  (GL_APIENTRY *PFN_glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
static PFN_glDrawArraysInstanced _sapp_glDrawArraysInstanced;
typedef void  (GL_APIENTRY *PFN_glClearStencil)(GLint s);
static PFN_glClearStencil _sapp_glClearStencil;
typedef void  (GL_APIENTRY *PFN_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
static PFN_glScissor _sapp_glScissor;
typedef void  (GL_APIENTRY *PFN_glUniform3fv)(GLint location, GLsizei count, const GLfloat * value);
static PFN_glUniform3fv _sapp_glUniform3fv;
typedef void  (GL_APIENTRY *PFN_glGenRenderbuffers)(GLsizei n, GLuint * renderbuffers);
static PFN_glGenRenderbuffers _sapp_glGenRenderbuffers;
typedef void  (GL_APIENTRY *PFN_glBufferData)(GLenum target, GLsizeiptr size, const void * data, GLenum usage);
static PFN_glBufferData _sapp_glBufferData;
typedef void  (GL_APIENTRY *PFN_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
static PFN_glBlendFuncSeparate _sapp_glBlendFuncSeparate;
typedef void  (GL_APIENTRY *PFN_glTexParameteri)(GLenum target, GLenum pname, GLint param);
static PFN_glTexParameteri _sapp_glTexParameteri;
typedef void  (GL_APIENTRY *PFN_glGetIntegerv)(GLenum pname, GLint * data);
static PFN_glGetIntegerv _sapp_glGetIntegerv;
typedef void  (GL_APIENTRY *PFN_glEnable)(GLenum cap);
static PFN_glEnable _sapp_glEnable;
typedef void  (GL_APIENTRY *PFN_glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
static PFN_glBlitFramebuffer _sapp_glBlitFramebuffer;
typedef void  (GL_APIENTRY *PFN_glStencilMask)(GLuint mask);
static PFN_glStencilMask _sapp_glStencilMask;
typedef void  (GL_APIENTRY *PFN_glAttachShader)(GLuint program, GLuint shader);
static PFN_glAttachShader _sapp_glAttachShader;
typedef GLenum (GL_APIENTRY *PFN_glGetError)();
static PFN_glGetError _sapp_glGetError;
typedef void  (GL_APIENTRY *PFN_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
static PFN_glClearColor _sapp_glClearColor;
typedef void  (GL_APIENTRY *PFN_glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
static PFN_glBlendColor _sapp_glBlendColor;
typedef void  (GL_APIENTRY *PFN_glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
static PFN_glTexParameterf _sapp_glTexParameterf;
typedef void  (GL_APIENTRY *PFN_glTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
static PFN_glTexParameterfv _sapp_glTexParameterfv;
typedef void  (GL_APIENTRY *PFN_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
static PFN_glGetShaderInfoLog _sapp_glGetShaderInfoLog;
typedef void  (GL_APIENTRY *PFN_glDepthFunc)(GLenum func);
static PFN_glDepthFunc _sapp_glDepthFunc;
typedef void  (GL_APIENTRY *PFN_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
static PFN_glStencilOp _sapp_glStencilOp;
typedef void  (GL_APIENTRY *PFN_glStencilFunc)(GLenum func, GLint ref, GLuint mask);
static PFN_glStencilFunc _sapp_glStencilFunc;
typedef void  (GL_APIENTRY *PFN_glEnableVertexAttribArray)(GLuint index);
static PFN_glEnableVertexAttribArray _sapp_glEnableVertexAttribArray;
typedef void  (GL_APIENTRY *PFN_glBlendFunc)(GLenum sfactor, GLenum dfactor);
static PFN_glBlendFunc _sapp_glBlendFunc;
typedef void  (GL_APIENTRY *PFN_glUniform1fv)(GLint location, GLsizei count, const GLfloat * value);
static PFN_glUniform1fv _sapp_glUniform1fv;
typedef void  (GL_APIENTRY *PFN_glReadBuffer)(GLenum src);
static PFN_glReadBuffer _sapp_glReadBuffer;
typedef void  (GL_APIENTRY *PFN_glClear)(GLbitfield mask);
static PFN_glClear _sapp_glClear;
typedef void  (GL_APIENTRY *PFN_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels);
static PFN_glTexImage2D _sapp_glTexImage2D;
typedef void  (GL_APIENTRY *PFN_glGenVertexArrays)(GLsizei n, GLuint * arrays);
static PFN_glGenVertexArrays _sapp_glGenVertexArrays;
typedef void  (GL_APIENTRY *PFN_glFrontFace)(GLenum mode);
static PFN_glFrontFace _sapp_glFrontFace;
typedef void  (GL_APIENTRY *PFN_glCullFace)(GLenum mode);
static PFN_glCullFace _sapp_glCullFace;

_SOKOL_PRIVATE void* _sapp_win32_glgetprocaddr(const char* name) {
    void* proc_addr = (void*) _sapp_wglGetProcAddress(name);
    if (0 == proc_addr) {
        proc_addr = (void*) GetProcAddress(_sapp_opengl32, name);
    }
    SOKOL_ASSERT(proc_addr);
    return proc_addr;
}

#define _SAPP_GLPROC(name) _sapp_ ## name = (PFN_ ## name) _sapp_win32_glgetprocaddr(#name)

_SOKOL_PRIVATE  void _sapp_win32_gl_loadfuncs(void) {
    SOKOL_ASSERT(_sapp_wglGetProcAddress);
    SOKOL_ASSERT(_sapp_opengl32);
    _SAPP_GLPROC(glBindVertexArray);
    _SAPP_GLPROC(glFramebufferTextureLayer);
    _SAPP_GLPROC(glGenFramebuffers);
    _SAPP_GLPROC(glBindFramebuffer);
    _SAPP_GLPROC(glBindRenderbuffer);
    _SAPP_GLPROC(glGetStringi);
    _SAPP_GLPROC(glClearBufferfi);
    _SAPP_GLPROC(glClearBufferfv);
    _SAPP_GLPROC(glClearBufferuiv);
    _SAPP_GLPROC(glDeleteRenderbuffers);
    _SAPP_GLPROC(glUniform4fv);
    _SAPP_GLPROC(glUniform2fv);
    _SAPP_GLPROC(glUseProgram);
    _SAPP_GLPROC(glShaderSource);
    _SAPP_GLPROC(glLinkProgram);
    _SAPP_GLPROC(glGetUniformLocation);
    _SAPP_GLPROC(glGetShaderiv);
    _SAPP_GLPROC(glGetProgramInfoLog);
    _SAPP_GLPROC(glGetAttribLocation);
    _SAPP_GLPROC(glDisableVertexAttribArray);
    _SAPP_GLPROC(glDeleteShader);
    _SAPP_GLPROC(glDeleteProgram);
    _SAPP_GLPROC(glCompileShader);
    _SAPP_GLPROC(glStencilFuncSeparate);
    _SAPP_GLPROC(glStencilOpSeparate);
    _SAPP_GLPROC(glRenderbufferStorageMultisample);
    _SAPP_GLPROC(glDrawBuffers);
    _SAPP_GLPROC(glVertexAttribDivisor);
    _SAPP_GLPROC(glBufferSubData);
    _SAPP_GLPROC(glGenBuffers);
    _SAPP_GLPROC(glCheckFramebufferStatus);
    _SAPP_GLPROC(glFramebufferRenderbuffer);
    _SAPP_GLPROC(glCompressedTexImage2D);
    _SAPP_GLPROC(glCompressedTexImage3D);
    _SAPP_GLPROC(glActiveTexture);
    _SAPP_GLPROC(glTexSubImage3D);
    _SAPP_GLPROC(glUniformMatrix4fv);
    _SAPP_GLPROC(glRenderbufferStorage);
    _SAPP_GLPROC(glGenTextures);
    _SAPP_GLPROC(glPolygonOffset);
    _SAPP_GLPROC(glDrawElements);
    _SAPP_GLPROC(glDeleteFramebuffers);
    _SAPP_GLPROC(glBlendEquationSeparate);
    _SAPP_GLPROC(glDeleteTextures);
    _SAPP_GLPROC(glGetProgramiv);
    _SAPP_GLPROC(glBindTexture);
    _SAPP_GLPROC(glTexImage3D);
    _SAPP_GLPROC(glCreateShader);
    _SAPP_GLPROC(glTexSubImage2D);
    _SAPP_GLPROC(glClearDepth);
    _SAPP_GLPROC(glFramebufferTexture2D);
    _SAPP_GLPROC(glCreateProgram);
    _SAPP_GLPROC(glViewport);
    _SAPP_GLPROC(glDeleteBuffers);
    _SAPP_GLPROC(glDrawArrays);
    _SAPP_GLPROC(glDrawElementsInstanced);
    _SAPP_GLPROC(glVertexAttribPointer);
    _SAPP_GLPROC(glUniform1i);
    _SAPP_GLPROC(glDisable);
    _SAPP_GLPROC(glColorMask);
    _SAPP_GLPROC(glBindBuffer);
    _SAPP_GLPROC(glDeleteVertexArrays);
    _SAPP_GLPROC(glDepthMask);
    _SAPP_GLPROC(glDrawArraysInstanced);
    _SAPP_GLPROC(glClearStencil);
    _SAPP_GLPROC(glScissor);
    _SAPP_GLPROC(glUniform3fv);
    _SAPP_GLPROC(glGenRenderbuffers);
    _SAPP_GLPROC(glBufferData);
    _SAPP_GLPROC(glBlendFuncSeparate);
    _SAPP_GLPROC(glTexParameteri);
    _SAPP_GLPROC(glGetIntegerv);
    _SAPP_GLPROC(glEnable);
    _SAPP_GLPROC(glBlitFramebuffer);
    _SAPP_GLPROC(glStencilMask);
    _SAPP_GLPROC(glAttachShader);
    _SAPP_GLPROC(glGetError);
    _SAPP_GLPROC(glClearColor);
    _SAPP_GLPROC(glBlendColor);
    _SAPP_GLPROC(glTexParameterf);
    _SAPP_GLPROC(glTexParameterfv);
    _SAPP_GLPROC(glGetShaderInfoLog);
    _SAPP_GLPROC(glDepthFunc);
    _SAPP_GLPROC(glStencilOp);
    _SAPP_GLPROC(glStencilFunc);
    _SAPP_GLPROC(glEnableVertexAttribArray);
    _SAPP_GLPROC(glBlendFunc);
    _SAPP_GLPROC(glUniform1fv);
    _SAPP_GLPROC(glReadBuffer);
    _SAPP_GLPROC(glClear);
    _SAPP_GLPROC(glTexImage2D);
    _SAPP_GLPROC(glGenVertexArrays);
    _SAPP_GLPROC(glFrontFace);
    _SAPP_GLPROC(glCullFace);
}
#define glBindVertexArray _sapp_glBindVertexArray
#define glFramebufferTextureLayer _sapp_glFramebufferTextureLayer
#define glGenFramebuffers _sapp_glGenFramebuffers
#define glBindFramebuffer _sapp_glBindFramebuffer
#define glBindRenderbuffer _sapp_glBindRenderbuffer
#define glGetStringi _sapp_glGetStringi
#define glClearBufferfi _sapp_glClearBufferfi
#define glClearBufferfv _sapp_glClearBufferfv
#define glClearBufferuiv _sapp_glClearBufferuiv
#define glDeleteRenderbuffers _sapp_glDeleteRenderbuffers
#define glUniform4fv _sapp_glUniform4fv
#define glUniform2fv _sapp_glUniform2fv
#define glUseProgram _sapp_glUseProgram
#define glShaderSource _sapp_glShaderSource
#define glLinkProgram _sapp_glLinkProgram
#define glGetUniformLocation _sapp_glGetUniformLocation
#define glGetShaderiv _sapp_glGetShaderiv
#define glGetProgramInfoLog _sapp_glGetProgramInfoLog
#define glGetAttribLocation _sapp_glGetAttribLocation
#define glDisableVertexAttribArray _sapp_glDisableVertexAttribArray
#define glDeleteShader _sapp_glDeleteShader
#define glDeleteProgram _sapp_glDeleteProgram
#define glCompileShader _sapp_glCompileShader
#define glStencilFuncSeparate _sapp_glStencilFuncSeparate
#define glStencilOpSeparate _sapp_glStencilOpSeparate
#define glRenderbufferStorageMultisample _sapp_glRenderbufferStorageMultisample
#define glDrawBuffers _sapp_glDrawBuffers
#define glVertexAttribDivisor _sapp_glVertexAttribDivisor
#define glBufferSubData _sapp_glBufferSubData
#define glGenBuffers _sapp_glGenBuffers
#define glCheckFramebufferStatus _sapp_glCheckFramebufferStatus
#define glFramebufferRenderbuffer _sapp_glFramebufferRenderbuffer
#define glCompressedTexImage2D _sapp_glCompressedTexImage2D
#define glCompressedTexImage3D _sapp_glCompressedTexImage3D
#define glActiveTexture _sapp_glActiveTexture
#define glTexSubImage3D _sapp_glTexSubImage3D
#define glUniformMatrix4fv _sapp_glUniformMatrix4fv
#define glRenderbufferStorage _sapp_glRenderbufferStorage
#define glGenTextures _sapp_glGenTextures
#define glPolygonOffset _sapp_glPolygonOffset
#define glDrawElements _sapp_glDrawElements
#define glDeleteFramebuffers _sapp_glDeleteFramebuffers
#define glBlendEquationSeparate _sapp_glBlendEquationSeparate
#define glDeleteTextures _sapp_glDeleteTextures
#define glGetProgramiv _sapp_glGetProgramiv
#define glBindTexture _sapp_glBindTexture
#define glTexImage3D _sapp_glTexImage3D
#define glCreateShader _sapp_glCreateShader
#define glTexSubImage2D _sapp_glTexSubImage2D
#define glClearDepth _sapp_glClearDepth
#define glFramebufferTexture2D _sapp_glFramebufferTexture2D
#define glCreateProgram _sapp_glCreateProgram
#define glViewport _sapp_glViewport
#define glDeleteBuffers _sapp_glDeleteBuffers
#define glDrawArrays _sapp_glDrawArrays
#define glDrawElementsInstanced _sapp_glDrawElementsInstanced
#define glVertexAttribPointer _sapp_glVertexAttribPointer
#define glUniform1i _sapp_glUniform1i
#define glDisable _sapp_glDisable
#define glColorMask _sapp_glColorMask
#define glBindBuffer _sapp_glBindBuffer
#define glDeleteVertexArrays _sapp_glDeleteVertexArrays
#define glDepthMask _sapp_glDepthMask
#define glDrawArraysInstanced _sapp_glDrawArraysInstanced
#define glClearStencil _sapp_glClearStencil
#define glScissor _sapp_glScissor
#define glUniform3fv _sapp_glUniform3fv
#define glGenRenderbuffers _sapp_glGenRenderbuffers
#define glBufferData _sapp_glBufferData
#define glBlendFuncSeparate _sapp_glBlendFuncSeparate
#define glTexParameteri _sapp_glTexParameteri
#define glGetIntegerv _sapp_glGetIntegerv
#define glEnable _sapp_glEnable
#define glBlitFramebuffer _sapp_glBlitFramebuffer
#define glStencilMask _sapp_glStencilMask
#define glAttachShader _sapp_glAttachShader
#define glGetError _sapp_glGetError
#define glClearColor _sapp_glClearColor
#define glBlendColor _sapp_glBlendColor
#define glTexParameterf _sapp_glTexParameterf
#define glTexParameterfv _sapp_glTexParameterfv
#define glGetShaderInfoLog _sapp_glGetShaderInfoLog
#define glDepthFunc _sapp_glDepthFunc
#define glStencilOp _sapp_glStencilOp
#define glStencilFunc _sapp_glStencilFunc
#define glEnableVertexAttribArray _sapp_glEnableVertexAttribArray
#define glBlendFunc _sapp_glBlendFunc
#define glUniform1fv _sapp_glUniform1fv
#define glReadBuffer _sapp_glReadBuffer
#define glClear _sapp_glClear
#define glTexImage2D _sapp_glTexImage2D
#define glGenVertexArrays _sapp_glGenVertexArrays
#define glFrontFace _sapp_glFrontFace
#define glCullFace _sapp_glCullFace

#endif /* SOKOL_WIN32_NO_GL_LOADER */

#endif /* SOKOL_GLCORE33 */

#if defined(SOKOL_D3D11)
#define _SAPP_SAFE_RELEASE(class, obj) if (obj) { class##_Release(obj); obj=0; }
_SOKOL_PRIVATE void _sapp_d3d11_create_device_and_swapchain(void) {
    DXGI_SWAP_CHAIN_DESC* sc_desc = &_sapp_dxgi_swap_chain_desc;
    sc_desc->BufferDesc.Width = _sapp.framebuffer_width;
    sc_desc->BufferDesc.Height = _sapp.framebuffer_height;
    sc_desc->BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sc_desc->BufferDesc.RefreshRate.Numerator = 60;
    sc_desc->BufferDesc.RefreshRate.Denominator = 1;
    sc_desc->OutputWindow = _sapp_win32_hwnd;
    sc_desc->Windowed = true;
    sc_desc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sc_desc->BufferCount = 1;
    sc_desc->SampleDesc.Count = _sapp.sample_count;
    sc_desc->SampleDesc.Quality = _sapp.sample_count > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0;
    sc_desc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    int create_flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    #if defined(SOKOL_DEBUG)
        create_flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif
    D3D_FEATURE_LEVEL feature_level;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL,                           /* pAdapter (use default) */
        D3D_DRIVER_TYPE_HARDWARE,       /* DriverType */
        NULL,                           /* Software */
        create_flags,                   /* Flags */
        NULL,                           /* pFeatureLevels */
        0,                              /* FeatureLevels */
        D3D11_SDK_VERSION,              /* SDKVersion */
        sc_desc,                        /* pSwapChainDesc */
        &_sapp_dxgi_swap_chain,         /* ppSwapChain */
        &_sapp_d3d11_device,            /* ppDevice */
        &feature_level,                 /* pFeatureLevel */
        &_sapp_d3d11_device_context);   /* ppImmediateContext */
    _SOKOL_UNUSED(hr);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp_dxgi_swap_chain && _sapp_d3d11_device && _sapp_d3d11_device_context);
}

_SOKOL_PRIVATE void _sapp_d3d11_destroy_device_and_swapchain(void) {
    _SAPP_SAFE_RELEASE(IDXGISwapChain, _sapp_dxgi_swap_chain);
    _SAPP_SAFE_RELEASE(ID3D11DeviceContext, _sapp_d3d11_device_context);
    _SAPP_SAFE_RELEASE(ID3D11Device, _sapp_d3d11_device);
}

_SOKOL_PRIVATE void _sapp_d3d11_create_default_render_target(void) {
    HRESULT hr;
    #ifdef __cplusplus
    hr = IDXGISwapChain_GetBuffer(_sapp_dxgi_swap_chain, 0, IID_ID3D11Texture2D, (void**)&_sapp_d3d11_rt);
    #else
    hr = IDXGISwapChain_GetBuffer(_sapp_dxgi_swap_chain, 0, &IID_ID3D11Texture2D, (void**)&_sapp_d3d11_rt);
    #endif
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp_d3d11_rt);
    hr = ID3D11Device_CreateRenderTargetView(_sapp_d3d11_device, (ID3D11Resource*)_sapp_d3d11_rt, NULL, &_sapp_d3d11_rtv);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp_d3d11_rtv);
    D3D11_TEXTURE2D_DESC ds_desc;
    memset(&ds_desc, 0, sizeof(ds_desc));
    ds_desc.Width = _sapp.framebuffer_width;
    ds_desc.Height = _sapp.framebuffer_height;
    ds_desc.MipLevels = 1;
    ds_desc.ArraySize = 1;
    ds_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds_desc.SampleDesc = _sapp_dxgi_swap_chain_desc.SampleDesc;
    ds_desc.Usage = D3D11_USAGE_DEFAULT;
    ds_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = ID3D11Device_CreateTexture2D(_sapp_d3d11_device, &ds_desc, NULL, &_sapp_d3d11_ds);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp_d3d11_ds);
    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    memset(&dsv_desc, 0, sizeof(dsv_desc));
    dsv_desc.Format = ds_desc.Format;
    dsv_desc.ViewDimension = _sapp.sample_count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    hr = ID3D11Device_CreateDepthStencilView(_sapp_d3d11_device, (ID3D11Resource*)_sapp_d3d11_ds, &dsv_desc, &_sapp_d3d11_dsv);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp_d3d11_dsv);
}

_SOKOL_PRIVATE void _sapp_d3d11_destroy_default_render_target(void) {
    _SAPP_SAFE_RELEASE(ID3D11Texture2D, _sapp_d3d11_rt);
    _SAPP_SAFE_RELEASE(ID3D11RenderTargetView, _sapp_d3d11_rtv);
    _SAPP_SAFE_RELEASE(ID3D11Texture2D, _sapp_d3d11_ds);
    _SAPP_SAFE_RELEASE(ID3D11DepthStencilView, _sapp_d3d11_dsv);
}

_SOKOL_PRIVATE void _sapp_d3d11_resize_default_render_target(void) {
    if (_sapp_dxgi_swap_chain) {
        _sapp_d3d11_destroy_default_render_target();
        IDXGISwapChain_ResizeBuffers(_sapp_dxgi_swap_chain, 1, _sapp.framebuffer_width, _sapp.framebuffer_height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
        _sapp_d3d11_create_default_render_target();
    }
}
#endif

#if defined(SOKOL_GLCORE33)
_SOKOL_PRIVATE void _sapp_wgl_init(void) {
    _sapp_opengl32 = LoadLibraryA("opengl32.dll");
    if (!_sapp_opengl32) {
        _sapp_fail("Failed to load opengl32.dll\n");
    }
    SOKOL_ASSERT(_sapp_opengl32);
    _sapp_wglCreateContext = (PFN_wglCreateContext) GetProcAddress(_sapp_opengl32, "wglCreateContext");
    SOKOL_ASSERT(_sapp_wglCreateContext);
    _sapp_wglDeleteContext = (PFN_wglDeleteContext) GetProcAddress(_sapp_opengl32, "wglDeleteContext");
    SOKOL_ASSERT(_sapp_wglDeleteContext);
    _sapp_wglGetProcAddress = (PFN_wglGetProcAddress) GetProcAddress(_sapp_opengl32, "wglGetProcAddress");
    SOKOL_ASSERT(_sapp_wglGetProcAddress);
    _sapp_wglGetCurrentDC = (PFN_wglGetCurrentDC) GetProcAddress(_sapp_opengl32, "wglGetCurrentDC");
    SOKOL_ASSERT(_sapp_wglGetCurrentDC);
    _sapp_wglMakeCurrent = (PFN_wglMakeCurrent) GetProcAddress(_sapp_opengl32, "wglMakeCurrent");
    SOKOL_ASSERT(_sapp_wglMakeCurrent);

    _sapp_win32_msg_hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
        L"SOKOLAPP",
        L"sokol-app message window",
        WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
        0, 0, 1, 1,
        NULL, NULL,
        GetModuleHandleW(NULL),
        NULL);
    if (!_sapp_win32_msg_hwnd) {
        _sapp_fail("Win32: failed to create helper window!\n");
    }
    ShowWindow(_sapp_win32_msg_hwnd, SW_HIDE);
    MSG msg;
    while (PeekMessageW(&msg, _sapp_win32_msg_hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    _sapp_win32_msg_dc = GetDC(_sapp_win32_msg_hwnd);
    if (!_sapp_win32_msg_dc) {
        _sapp_fail("Win32: failed to obtain helper window DC!\n");
    }
}

_SOKOL_PRIVATE void _sapp_wgl_shutdown(void) {
    SOKOL_ASSERT(_sapp_opengl32 && _sapp_win32_msg_hwnd);
    DestroyWindow(_sapp_win32_msg_hwnd); _sapp_win32_msg_hwnd = 0;
    FreeLibrary(_sapp_opengl32); _sapp_opengl32 = 0;
}

_SOKOL_PRIVATE bool _sapp_wgl_has_ext(const char* ext, const char* extensions) {
    SOKOL_ASSERT(ext && extensions);
    const char* start = extensions;
    while (true) {
        const char* where = strstr(start, ext);
        if (!where) {
            return false;
        }
        const char* terminator = where + strlen(ext);
        if ((where == start) || (*(where - 1) == ' ')) {
            if (*terminator == ' ' || *terminator == '\0') {
                break;
            }
        }
        start = terminator;
    }
    return true;
}

_SOKOL_PRIVATE bool _sapp_wgl_ext_supported(const char* ext) {
    SOKOL_ASSERT(ext);
    if (_sapp_GetExtensionsStringEXT) {
        const char* extensions = _sapp_GetExtensionsStringEXT();
        if (extensions) {
            if (_sapp_wgl_has_ext(ext, extensions)) {
                return true;
            }
        }
    }
    if (_sapp_GetExtensionsStringARB) {
        const char* extensions = _sapp_GetExtensionsStringARB(_sapp_wglGetCurrentDC());
        if (extensions) {
            if (_sapp_wgl_has_ext(ext, extensions)) {
                return true;
            }
        }
    }
    return false;
}

_SOKOL_PRIVATE void _sapp_wgl_load_extensions(void) {
    SOKOL_ASSERT(_sapp_win32_msg_dc);
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    if (!SetPixelFormat(_sapp_win32_msg_dc, ChoosePixelFormat(_sapp_win32_msg_dc, &pfd), &pfd)) {
        _sapp_fail("WGL: failed to set pixel format for dummy context\n");
    }
    HGLRC rc = _sapp_wglCreateContext(_sapp_win32_msg_dc);
    if (!rc) {
        _sapp_fail("WGL: Failed to create dummy context\n");
    }
    if (!_sapp_wglMakeCurrent(_sapp_win32_msg_dc, rc)) {
        _sapp_fail("WGL: Failed to make context current\n");
    }
    _sapp_GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) _sapp_wglGetProcAddress("wglGetExtensionsStringEXT");
    _sapp_GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) _sapp_wglGetProcAddress("wglGetExtensionsStringARB");
    _sapp_CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) _sapp_wglGetProcAddress("wglCreateContextAttribsARB");
    _sapp_SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) _sapp_wglGetProcAddress("wglSwapIntervalEXT");
    _sapp_GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC) _sapp_wglGetProcAddress("wglGetPixelFormatAttribivARB");
    _sapp_arb_multisample = _sapp_wgl_ext_supported("WGL_ARB_multisample");
    _sapp_arb_create_context = _sapp_wgl_ext_supported("WGL_ARB_create_context");
    _sapp_arb_create_context_profile = _sapp_wgl_ext_supported("WGL_ARB_create_context_profile");
    _sapp_ext_swap_control = _sapp_wgl_ext_supported("WGL_EXT_swap_control");
    _sapp_arb_pixel_format = _sapp_wgl_ext_supported("WGL_ARB_pixel_format");
    _sapp_wglMakeCurrent(_sapp_win32_msg_dc, 0);
    _sapp_wglDeleteContext(rc);
}

_SOKOL_PRIVATE int _sapp_wgl_attrib(int pixel_format, int attrib) {
    SOKOL_ASSERT(_sapp_arb_pixel_format);
    int value = 0;
    if (!_sapp_GetPixelFormatAttribivARB(_sapp_win32_dc, pixel_format, 0, 1, &attrib, &value)) {
        _sapp_fail("WGL: Failed to retrieve pixel format attribute\n");
    }
    return value;
}

_SOKOL_PRIVATE int _sapp_wgl_find_pixel_format(void) {
    SOKOL_ASSERT(_sapp_win32_dc);
    SOKOL_ASSERT(_sapp_arb_pixel_format);
    const _sapp_gl_fbconfig* closest;

    int native_count = _sapp_wgl_attrib(1, WGL_NUMBER_PIXEL_FORMATS_ARB);
    _sapp_gl_fbconfig* usable_configs = (_sapp_gl_fbconfig*) SOKOL_CALLOC(native_count, sizeof(_sapp_gl_fbconfig));
    int usable_count = 0;
    for (int i = 0; i < native_count; i++) {
        const int n = i + 1;
        _sapp_gl_fbconfig* u = usable_configs + usable_count;
        _sapp_gl_init_fbconfig(u);
        if (!_sapp_wgl_attrib(n, WGL_SUPPORT_OPENGL_ARB) || !_sapp_wgl_attrib(n, WGL_DRAW_TO_WINDOW_ARB)) {
            continue;
        }
        if (_sapp_wgl_attrib(n, WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB) {
            continue;
        }
        if (_sapp_wgl_attrib(n, WGL_ACCELERATION_ARB) == WGL_NO_ACCELERATION_ARB) {
            continue;
        }
        u->red_bits     = _sapp_wgl_attrib(n, WGL_RED_BITS_ARB);
        u->green_bits   = _sapp_wgl_attrib(n, WGL_GREEN_BITS_ARB);
        u->blue_bits    = _sapp_wgl_attrib(n, WGL_BLUE_BITS_ARB);
        u->alpha_bits   = _sapp_wgl_attrib(n, WGL_ALPHA_BITS_ARB);
        u->depth_bits   = _sapp_wgl_attrib(n, WGL_DEPTH_BITS_ARB);
        u->stencil_bits = _sapp_wgl_attrib(n, WGL_STENCIL_BITS_ARB);
        if (_sapp_wgl_attrib(n, WGL_DOUBLE_BUFFER_ARB)) {
            u->doublebuffer = true;
        }
        if (_sapp_arb_multisample) {
            u->samples = _sapp_wgl_attrib(n, WGL_SAMPLES_ARB);
        }
        u->handle = n;
        usable_count++;
    }
    SOKOL_ASSERT(usable_count > 0);
    _sapp_gl_fbconfig desired;
    _sapp_gl_init_fbconfig(&desired);
    desired.red_bits = 8;
    desired.green_bits = 8;
    desired.blue_bits = 8;
    desired.alpha_bits = 8;
    desired.depth_bits = 24;
    desired.stencil_bits = 8;
    desired.doublebuffer = true;
    desired.samples = _sapp.sample_count > 1 ? _sapp.sample_count : 0;
    closest = _sapp_gl_choose_fbconfig(&desired, usable_configs, usable_count);
    int pixel_format = 0;
    if (closest) {
        pixel_format = (int) closest->handle;
    }
    SOKOL_FREE(usable_configs);
    return pixel_format;
}

_SOKOL_PRIVATE void _sapp_wgl_create_context(void) {
    int pixel_format = _sapp_wgl_find_pixel_format();
    if (0 == pixel_format) {
        _sapp_fail("WGL: Didn't find matching pixel format.\n");
    }
    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(_sapp_win32_dc, pixel_format, sizeof(pfd), &pfd)) {
        _sapp_fail("WGL: Failed to retrieve PFD for selected pixel format!\n");
    }
    if (!SetPixelFormat(_sapp_win32_dc, pixel_format, &pfd)) {
        _sapp_fail("WGL: Failed to set selected pixel format!\n");
    }
    if (!_sapp_arb_create_context) {
        _sapp_fail("WGL: ARB_create_context required!\n");
    }
    if (!_sapp_arb_create_context_profile) {
        _sapp_fail("WGL: ARB_create_context_profile required!\n");
    }
    const int attrs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0, 0
    };
    _sapp_gl_ctx = _sapp_CreateContextAttribsARB(_sapp_win32_dc, 0, attrs);
    if (!_sapp_gl_ctx) {
        const DWORD err = GetLastError();
        if (err == (0xc0070000 | ERROR_INVALID_VERSION_ARB)) {
            _sapp_fail("WGL: Driver does not support OpenGL version 3.3\n");
        }
        else if (err == (0xc0070000 | ERROR_INVALID_PROFILE_ARB)) {
            _sapp_fail("WGL: Driver does not support the requested OpenGL profile");
        }
        else if (err == (0xc0070000 | ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB)) {
            _sapp_fail("WGL: The share context is not compatible with the requested context");
        }
        else {
            _sapp_fail("WGL: Failed to create OpenGL context");
        }
    }
    _sapp_wglMakeCurrent(_sapp_win32_dc, _sapp_gl_ctx);
    if (_sapp_ext_swap_control) {
        /* FIXME: DwmIsCompositionEnabled() (see GLFW) */
        _sapp_SwapIntervalEXT(_sapp.swap_interval);
    }
}

_SOKOL_PRIVATE void _sapp_wgl_destroy_context(void) {
    SOKOL_ASSERT(_sapp_gl_ctx);
    _sapp_wglDeleteContext(_sapp_gl_ctx);
    _sapp_gl_ctx = 0;
}

_SOKOL_PRIVATE void _sapp_wgl_swap_buffers(void) {
    SOKOL_ASSERT(_sapp_win32_dc);
    /* FIXME: DwmIsCompositionEnabled? (see GLFW) */
    SwapBuffers(_sapp_win32_dc);
}
#endif

_SOKOL_PRIVATE bool _sapp_win32_utf8_to_wide(const char* src, wchar_t* dst, int dst_num_bytes) {
    SOKOL_ASSERT(src && dst && (dst_num_bytes > 1));
    memset(dst, 0, dst_num_bytes);
    const int dst_chars = dst_num_bytes / sizeof(wchar_t);
    const int dst_needed = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);
    if ((dst_needed > 0) && (dst_needed < dst_chars)) {
        MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, dst_chars);
        return true;
    }
    else {
        /* input string doesn't fit into destination buffer */
        return false;
    }
}

_SOKOL_PRIVATE bool _sapp_win32_wide_to_utf8(const wchar_t* src, char* dst, int dst_num_bytes) {
    SOKOL_ASSERT(src && dst && (dst_num_bytes > 1));
    memset(dst, 0, dst_num_bytes);
    return 0 != WideCharToMultiByte(CP_UTF8, 0, src, -1, dst, dst_num_bytes, NULL, NULL);
}

_SOKOL_PRIVATE void _sapp_win32_toggle_fullscreen(void) {
    HMONITOR monitor = MonitorFromWindow(_sapp_win32_hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO minfo;
    memset(&minfo, 0, sizeof(minfo));
    minfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &minfo);
    const RECT mr = minfo.rcMonitor;
    const int monitor_w = mr.right - mr.left;
    const int monitor_h = mr.bottom - mr.top;

    const DWORD win_ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD win_style;
    RECT rect = { 0, 0, 0, 0 };

    _sapp.fullscreen = !_sapp.fullscreen;
    if (!_sapp.fullscreen) {
        win_style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
        rect.right = (int) ((float)_sapp.desc.width * _sapp_win32_window_scale);
        rect.bottom = (int) ((float)_sapp.desc.height * _sapp_win32_window_scale);
    }
    else {
        win_style = WS_POPUP | WS_SYSMENU | WS_VISIBLE;
        rect.right = monitor_w;
        rect.bottom = monitor_h;
    }
    AdjustWindowRectEx(&rect, win_style, FALSE, win_ex_style);
    int win_width = rect.right - rect.left;
    int win_height = rect.bottom - rect.top;
    if (!_sapp.fullscreen) {
        rect.left = (monitor_w - win_width) / 2;
        rect.top = (monitor_h - win_height) / 2;
    }

    SetWindowLongPtr(_sapp_win32_hwnd, GWL_STYLE, win_style);
    SetWindowPos(_sapp_win32_hwnd, HWND_TOP, mr.left + rect.left, mr.top + rect.top, win_width, win_height, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
}

_SOKOL_PRIVATE void _sapp_win32_show_mouse(bool shown) {
    ShowCursor((BOOL)shown);
}

_SOKOL_PRIVATE bool _sapp_win32_mouse_shown(void) {
    CURSORINFO cursor_info;
    memset(&cursor_info, 0, sizeof(CURSORINFO));
    cursor_info.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&cursor_info);
    return (cursor_info.flags & CURSOR_SHOWING) != 0;
}

_SOKOL_PRIVATE void _sapp_win32_init_keytable(void) {
    /* same as GLFW */
    _sapp.keycodes[0x00B] = SAPP_KEYCODE_0;
    _sapp.keycodes[0x002] = SAPP_KEYCODE_1;
    _sapp.keycodes[0x003] = SAPP_KEYCODE_2;
    _sapp.keycodes[0x004] = SAPP_KEYCODE_3;
    _sapp.keycodes[0x005] = SAPP_KEYCODE_4;
    _sapp.keycodes[0x006] = SAPP_KEYCODE_5;
    _sapp.keycodes[0x007] = SAPP_KEYCODE_6;
    _sapp.keycodes[0x008] = SAPP_KEYCODE_7;
    _sapp.keycodes[0x009] = SAPP_KEYCODE_8;
    _sapp.keycodes[0x00A] = SAPP_KEYCODE_9;
    _sapp.keycodes[0x01E] = SAPP_KEYCODE_A;
    _sapp.keycodes[0x030] = SAPP_KEYCODE_B;
    _sapp.keycodes[0x02E] = SAPP_KEYCODE_C;
    _sapp.keycodes[0x020] = SAPP_KEYCODE_D;
    _sapp.keycodes[0x012] = SAPP_KEYCODE_E;
    _sapp.keycodes[0x021] = SAPP_KEYCODE_F;
    _sapp.keycodes[0x022] = SAPP_KEYCODE_G;
    _sapp.keycodes[0x023] = SAPP_KEYCODE_H;
    _sapp.keycodes[0x017] = SAPP_KEYCODE_I;
    _sapp.keycodes[0x024] = SAPP_KEYCODE_J;
    _sapp.keycodes[0x025] = SAPP_KEYCODE_K;
    _sapp.keycodes[0x026] = SAPP_KEYCODE_L;
    _sapp.keycodes[0x032] = SAPP_KEYCODE_M;
    _sapp.keycodes[0x031] = SAPP_KEYCODE_N;
    _sapp.keycodes[0x018] = SAPP_KEYCODE_O;
    _sapp.keycodes[0x019] = SAPP_KEYCODE_P;
    _sapp.keycodes[0x010] = SAPP_KEYCODE_Q;
    _sapp.keycodes[0x013] = SAPP_KEYCODE_R;
    _sapp.keycodes[0x01F] = SAPP_KEYCODE_S;
    _sapp.keycodes[0x014] = SAPP_KEYCODE_T;
    _sapp.keycodes[0x016] = SAPP_KEYCODE_U;
    _sapp.keycodes[0x02F] = SAPP_KEYCODE_V;
    _sapp.keycodes[0x011] = SAPP_KEYCODE_W;
    _sapp.keycodes[0x02D] = SAPP_KEYCODE_X;
    _sapp.keycodes[0x015] = SAPP_KEYCODE_Y;
    _sapp.keycodes[0x02C] = SAPP_KEYCODE_Z;
    _sapp.keycodes[0x028] = SAPP_KEYCODE_APOSTROPHE;
    _sapp.keycodes[0x02B] = SAPP_KEYCODE_BACKSLASH;
    _sapp.keycodes[0x033] = SAPP_KEYCODE_COMMA;
    _sapp.keycodes[0x00D] = SAPP_KEYCODE_EQUAL;
    _sapp.keycodes[0x029] = SAPP_KEYCODE_GRAVE_ACCENT;
    _sapp.keycodes[0x01A] = SAPP_KEYCODE_LEFT_BRACKET;
    _sapp.keycodes[0x00C] = SAPP_KEYCODE_MINUS;
    _sapp.keycodes[0x034] = SAPP_KEYCODE_PERIOD;
    _sapp.keycodes[0x01B] = SAPP_KEYCODE_RIGHT_BRACKET;
    _sapp.keycodes[0x027] = SAPP_KEYCODE_SEMICOLON;
    _sapp.keycodes[0x035] = SAPP_KEYCODE_SLASH;
    _sapp.keycodes[0x056] = SAPP_KEYCODE_WORLD_2;
    _sapp.keycodes[0x00E] = SAPP_KEYCODE_BACKSPACE;
    _sapp.keycodes[0x153] = SAPP_KEYCODE_DELETE;
    _sapp.keycodes[0x14F] = SAPP_KEYCODE_END;
    _sapp.keycodes[0x01C] = SAPP_KEYCODE_ENTER;
    _sapp.keycodes[0x001] = SAPP_KEYCODE_ESCAPE;
    _sapp.keycodes[0x147] = SAPP_KEYCODE_HOME;
    _sapp.keycodes[0x152] = SAPP_KEYCODE_INSERT;
    _sapp.keycodes[0x15D] = SAPP_KEYCODE_MENU;
    _sapp.keycodes[0x151] = SAPP_KEYCODE_PAGE_DOWN;
    _sapp.keycodes[0x149] = SAPP_KEYCODE_PAGE_UP;
    _sapp.keycodes[0x045] = SAPP_KEYCODE_PAUSE;
    _sapp.keycodes[0x146] = SAPP_KEYCODE_PAUSE;
    _sapp.keycodes[0x039] = SAPP_KEYCODE_SPACE;
    _sapp.keycodes[0x00F] = SAPP_KEYCODE_TAB;
    _sapp.keycodes[0x03A] = SAPP_KEYCODE_CAPS_LOCK;
    _sapp.keycodes[0x145] = SAPP_KEYCODE_NUM_LOCK;
    _sapp.keycodes[0x046] = SAPP_KEYCODE_SCROLL_LOCK;
    _sapp.keycodes[0x03B] = SAPP_KEYCODE_F1;
    _sapp.keycodes[0x03C] = SAPP_KEYCODE_F2;
    _sapp.keycodes[0x03D] = SAPP_KEYCODE_F3;
    _sapp.keycodes[0x03E] = SAPP_KEYCODE_F4;
    _sapp.keycodes[0x03F] = SAPP_KEYCODE_F5;
    _sapp.keycodes[0x040] = SAPP_KEYCODE_F6;
    _sapp.keycodes[0x041] = SAPP_KEYCODE_F7;
    _sapp.keycodes[0x042] = SAPP_KEYCODE_F8;
    _sapp.keycodes[0x043] = SAPP_KEYCODE_F9;
    _sapp.keycodes[0x044] = SAPP_KEYCODE_F10;
    _sapp.keycodes[0x057] = SAPP_KEYCODE_F11;
    _sapp.keycodes[0x058] = SAPP_KEYCODE_F12;
    _sapp.keycodes[0x064] = SAPP_KEYCODE_F13;
    _sapp.keycodes[0x065] = SAPP_KEYCODE_F14;
    _sapp.keycodes[0x066] = SAPP_KEYCODE_F15;
    _sapp.keycodes[0x067] = SAPP_KEYCODE_F16;
    _sapp.keycodes[0x068] = SAPP_KEYCODE_F17;
    _sapp.keycodes[0x069] = SAPP_KEYCODE_F18;
    _sapp.keycodes[0x06A] = SAPP_KEYCODE_F19;
    _sapp.keycodes[0x06B] = SAPP_KEYCODE_F20;
    _sapp.keycodes[0x06C] = SAPP_KEYCODE_F21;
    _sapp.keycodes[0x06D] = SAPP_KEYCODE_F22;
    _sapp.keycodes[0x06E] = SAPP_KEYCODE_F23;
    _sapp.keycodes[0x076] = SAPP_KEYCODE_F24;
    _sapp.keycodes[0x038] = SAPP_KEYCODE_LEFT_ALT;
    _sapp.keycodes[0x01D] = SAPP_KEYCODE_LEFT_CONTROL;
    _sapp.keycodes[0x02A] = SAPP_KEYCODE_LEFT_SHIFT;
    _sapp.keycodes[0x15B] = SAPP_KEYCODE_LEFT_SUPER;
    _sapp.keycodes[0x137] = SAPP_KEYCODE_PRINT_SCREEN;
    _sapp.keycodes[0x138] = SAPP_KEYCODE_RIGHT_ALT;
    _sapp.keycodes[0x11D] = SAPP_KEYCODE_RIGHT_CONTROL;
    _sapp.keycodes[0x036] = SAPP_KEYCODE_RIGHT_SHIFT;
    _sapp.keycodes[0x15C] = SAPP_KEYCODE_RIGHT_SUPER;
    _sapp.keycodes[0x150] = SAPP_KEYCODE_DOWN;
    _sapp.keycodes[0x14B] = SAPP_KEYCODE_LEFT;
    _sapp.keycodes[0x14D] = SAPP_KEYCODE_RIGHT;
    _sapp.keycodes[0x148] = SAPP_KEYCODE_UP;
    _sapp.keycodes[0x052] = SAPP_KEYCODE_KP_0;
    _sapp.keycodes[0x04F] = SAPP_KEYCODE_KP_1;
    _sapp.keycodes[0x050] = SAPP_KEYCODE_KP_2;
    _sapp.keycodes[0x051] = SAPP_KEYCODE_KP_3;
    _sapp.keycodes[0x04B] = SAPP_KEYCODE_KP_4;
    _sapp.keycodes[0x04C] = SAPP_KEYCODE_KP_5;
    _sapp.keycodes[0x04D] = SAPP_KEYCODE_KP_6;
    _sapp.keycodes[0x047] = SAPP_KEYCODE_KP_7;
    _sapp.keycodes[0x048] = SAPP_KEYCODE_KP_8;
    _sapp.keycodes[0x049] = SAPP_KEYCODE_KP_9;
    _sapp.keycodes[0x04E] = SAPP_KEYCODE_KP_ADD;
    _sapp.keycodes[0x053] = SAPP_KEYCODE_KP_DECIMAL;
    _sapp.keycodes[0x135] = SAPP_KEYCODE_KP_DIVIDE;
    _sapp.keycodes[0x11C] = SAPP_KEYCODE_KP_ENTER;
    _sapp.keycodes[0x037] = SAPP_KEYCODE_KP_MULTIPLY;
    _sapp.keycodes[0x04A] = SAPP_KEYCODE_KP_SUBTRACT;
}

/* updates current window and framebuffer size from the window's client rect, returns true if size has changed */
_SOKOL_PRIVATE bool _sapp_win32_update_dimensions(void) {
    RECT rect;
    if (GetClientRect(_sapp_win32_hwnd, &rect)) {
        _sapp.window_width = (int)((float)(rect.right - rect.left) / _sapp_win32_window_scale);
        _sapp.window_height = (int)((float)(rect.bottom - rect.top) / _sapp_win32_window_scale);
        const int fb_width = (int)((float)_sapp.window_width * _sapp_win32_content_scale);
        const int fb_height = (int)((float)_sapp.window_height * _sapp_win32_content_scale);
        if ((fb_width != _sapp.framebuffer_width) || (fb_height != _sapp.framebuffer_height)) {
            _sapp.framebuffer_width = fb_width;
            _sapp.framebuffer_height = fb_height;
            /* prevent a framebuffer size of 0 when window is minimized */
            if (_sapp.framebuffer_width == 0) {
                _sapp.framebuffer_width = 1;
            }
            if (_sapp.framebuffer_height == 0) {
                _sapp.framebuffer_height = 1;
            }
            return true;
        }
    }
    else {
        _sapp.window_width = _sapp.window_height = 1;
        _sapp.framebuffer_width = _sapp.framebuffer_height = 1;
    }
    return false;
}

_SOKOL_PRIVATE uint32_t _sapp_win32_mods(void) {
    uint32_t mods = 0;
    if (GetKeyState(VK_SHIFT) & (1<<15)) {
        mods |= SAPP_MODIFIER_SHIFT;
    }
    if (GetKeyState(VK_CONTROL) & (1<<15)) {
        mods |= SAPP_MODIFIER_CTRL;
    }
    if (GetKeyState(VK_MENU) & (1<<15)) {
        mods |= SAPP_MODIFIER_ALT;
    }
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1<<15)) {
        mods |= SAPP_MODIFIER_SUPER;
    }
    return mods;
}

_SOKOL_PRIVATE void _sapp_win32_mouse_event(sapp_event_type type, sapp_mousebutton btn) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.mouse_button = btn;
        _sapp.event.mouse_x = _sapp.mouse_x;
        _sapp.event.mouse_y = _sapp.mouse_y;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_scroll_event(float x, float y) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.scroll_x = -x / 30.0f;
        _sapp.event.scroll_y = y / 30.0f;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_key_event(sapp_event_type type, int vk, bool repeat) {
    if (_sapp_events_enabled() && (vk < SAPP_MAX_KEYCODES)) {
        _sapp_init_event(type);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.key_code = _sapp.keycodes[vk];
        _sapp.event.key_repeat = repeat;
        _sapp_call_event(&_sapp.event);
        /* check if a CLIPBOARD_PASTED event must be sent too */
        if (_sapp.clipboard_enabled &&
            (type == SAPP_EVENTTYPE_KEY_DOWN) &&
            (_sapp.event.modifiers == SAPP_MODIFIER_CTRL) &&
            (_sapp.event.key_code == SAPP_KEYCODE_V))
        {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}

_SOKOL_PRIVATE void _sapp_win32_char_event(uint32_t c, bool repeat) {
    if (_sapp_events_enabled() && (c >= 32)) {
        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.char_code = c;
        _sapp.event.key_repeat = repeat;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE LRESULT CALLBACK _sapp_win32_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* FIXME: refresh rendering during resize with a WM_TIMER event */
    if (!_sapp_win32_in_create_window) {
        switch (uMsg) {
            case WM_CLOSE:
                /* only give user a chance to intervene when sapp_quit() wasn't already called */
                if (!_sapp.quit_ordered) {
                    /* if window should be closed and event handling is enabled, give user code
                        a change to intervene via sapp_cancel_quit()
                    */
                    _sapp.quit_requested = true;
                    _sapp_win32_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
                    /* if user code hasn't intervened, quit the app */
                    if (_sapp.quit_requested) {
                        _sapp.quit_ordered = true;
                    }
                }
                if (_sapp.quit_ordered) {
                    PostQuitMessage(0);
                }
                return 0;
            case WM_SYSCOMMAND:
                switch (wParam & 0xFFF0) {
                    case SC_SCREENSAVE:
                    case SC_MONITORPOWER:
                        if (_sapp.fullscreen) {
                            /* disable screen saver and blanking in fullscreen mode */
                            return 0;
                        }
                        break;
                    case SC_KEYMENU:
                        /* user trying to access menu via ALT */
                        return 0;
                }
                break;
            case WM_ERASEBKGND:
                return 1;
            case WM_SIZE:
                {
                    const bool iconified = wParam == SIZE_MINIMIZED;
                    if (iconified != _sapp_win32_iconified) {
                        _sapp_win32_iconified = iconified;
                        if (iconified) {
                            _sapp_win32_app_event(SAPP_EVENTTYPE_ICONIFIED);
                        }
                        else {
                            _sapp_win32_app_event(SAPP_EVENTTYPE_RESTORED);
                        }
                    }
                }
                break;
            case WM_SETCURSOR:
                if (_sapp.desc.user_cursor) {
                    if (LOWORD(lParam) == HTCLIENT) {
                        _sapp_win32_app_event(SAPP_EVENTTYPE_UPDATE_CURSOR);
                        return 1;
                    }
                }
                break;
            case WM_LBUTTONDOWN:
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_LEFT);
                break;
            case WM_RBUTTONDOWN:
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_RIGHT);
                break;
            case WM_MBUTTONDOWN:
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_MIDDLE);
                break;
            case WM_LBUTTONUP:
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_LEFT);
                break;
            case WM_RBUTTONUP:
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_RIGHT);
                break;
            case WM_MBUTTONUP:
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_MIDDLE);
                break;
            case WM_MOUSEMOVE:
                _sapp.mouse_x = (float)GET_X_LPARAM(lParam) * _sapp_win32_mouse_scale;
                _sapp.mouse_y = (float)GET_Y_LPARAM(lParam) * _sapp_win32_mouse_scale;
                if (!_sapp.win32_mouse_tracked) {
                    _sapp.win32_mouse_tracked = true;
                    TRACKMOUSEEVENT tme;
                    memset(&tme, 0, sizeof(tme));
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = _sapp_win32_hwnd;
                    TrackMouseEvent(&tme);
                    _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID);
                }
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE,  SAPP_MOUSEBUTTON_INVALID);
                break;
            case WM_MOUSELEAVE:
                _sapp.win32_mouse_tracked = false;
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID);
                break;
            case WM_MOUSEWHEEL:
                _sapp_win32_scroll_event(0.0f, (float)((SHORT)HIWORD(wParam)));
                break;
            case WM_MOUSEHWHEEL:
                _sapp_win32_scroll_event((float)((SHORT)HIWORD(wParam)), 0.0f);
                break;
            case WM_CHAR:
                _sapp_win32_char_event((uint32_t)wParam, !!(lParam&0x40000000));
                break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                _sapp_win32_key_event(SAPP_EVENTTYPE_KEY_DOWN, (int)(HIWORD(lParam)&0x1FF), !!(lParam&0x40000000));
                break;
            case WM_KEYUP:
            case WM_SYSKEYUP:
                _sapp_win32_key_event(SAPP_EVENTTYPE_KEY_UP, (int)(HIWORD(lParam)&0x1FF), false);
                break;
            default:
                break;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

_SOKOL_PRIVATE void _sapp_win32_create_window(void) {
    WNDCLASSW wndclassw;
    memset(&wndclassw, 0, sizeof(wndclassw));
    wndclassw.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclassw.lpfnWndProc = (WNDPROC) _sapp_win32_wndproc;
    wndclassw.hInstance = GetModuleHandleW(NULL);
    wndclassw.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclassw.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndclassw.lpszClassName = L"SOKOLAPP";
    RegisterClassW(&wndclassw);

    DWORD win_style;
    const DWORD win_ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    RECT rect = { 0, 0, 0, 0 };
    if (_sapp.fullscreen) {
        win_style = WS_POPUP | WS_SYSMENU | WS_VISIBLE;
        rect.right = GetSystemMetrics(SM_CXSCREEN);
        rect.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    else {
        win_style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
        rect.right = (int) ((float)_sapp.window_width * _sapp_win32_window_scale);
        rect.bottom = (int) ((float)_sapp.window_height * _sapp_win32_window_scale);
    }
    AdjustWindowRectEx(&rect, win_style, FALSE, win_ex_style);
    const int win_width = rect.right - rect.left;
    const int win_height = rect.bottom - rect.top;
    _sapp_win32_in_create_window = true;
    _sapp_win32_hwnd = CreateWindowExW(
        win_ex_style,               /* dwExStyle */
        L"SOKOLAPP",                /* lpClassName */
        _sapp.window_title_wide,    /* lpWindowName */
        win_style,                  /* dwStyle */
        CW_USEDEFAULT,              /* X */
        CW_USEDEFAULT,              /* Y */
        win_width,                  /* nWidth */
        win_height,                 /* nHeight */
        NULL,                       /* hWndParent */
        NULL,                       /* hMenu */
        GetModuleHandle(NULL),      /* hInstance */
        NULL);                      /* lParam */
    ShowWindow(_sapp_win32_hwnd, SW_SHOW);
    _sapp_win32_in_create_window = false;
    _sapp_win32_dc = GetDC(_sapp_win32_hwnd);
    SOKOL_ASSERT(_sapp_win32_dc);
    _sapp_win32_update_dimensions();
}

_SOKOL_PRIVATE void _sapp_win32_destroy_window(void) {
    DestroyWindow(_sapp_win32_hwnd); _sapp_win32_hwnd = 0;
    UnregisterClassW(L"SOKOLAPP", GetModuleHandleW(NULL));
}

_SOKOL_PRIVATE void _sapp_win32_init_dpi(void) {
    SOKOL_ASSERT(0 == _sapp_win32_setprocessdpiaware);
    SOKOL_ASSERT(0 == _sapp_win32_setprocessdpiawareness);
    SOKOL_ASSERT(0 == _sapp_win32_getdpiformonitor);
    HINSTANCE user32 = LoadLibraryA("user32.dll");
    if (user32) {
        _sapp_win32_setprocessdpiaware = (SETPROCESSDPIAWARE_T) GetProcAddress(user32, "SetProcessDPIAware");
    }
    HINSTANCE shcore = LoadLibraryA("shcore.dll");
    if (shcore) {
        _sapp_win32_setprocessdpiawareness = (SETPROCESSDPIAWARENESS_T) GetProcAddress(shcore, "SetProcessDpiAwareness");
        _sapp_win32_getdpiformonitor = (GETDPIFORMONITOR_T) GetProcAddress(shcore, "GetDpiForMonitor");
    }
    if (_sapp_win32_setprocessdpiawareness) {
        /* if the app didn't request HighDPI rendering, let Windows do the upscaling */
        PROCESS_DPI_AWARENESS process_dpi_awareness = PROCESS_SYSTEM_DPI_AWARE;
        _sapp_win32_dpi_aware = true;
        if (!_sapp.desc.high_dpi) {
            process_dpi_awareness = PROCESS_DPI_UNAWARE;
            _sapp_win32_dpi_aware = false;
        }
        _sapp_win32_setprocessdpiawareness(process_dpi_awareness);
    }
    else if (_sapp_win32_setprocessdpiaware) {
        _sapp_win32_setprocessdpiaware();
        _sapp_win32_dpi_aware = true;
    }
    /* get dpi scale factor for main monitor */
    if (_sapp_win32_getdpiformonitor && _sapp_win32_dpi_aware) {
        POINT pt = { 1, 1 };
        HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        UINT dpix, dpiy;
        HRESULT hr = _sapp_win32_getdpiformonitor(hm, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
        _SOKOL_UNUSED(hr);
        SOKOL_ASSERT(SUCCEEDED(hr));
        /* clamp window scale to an integer factor */
        _sapp_win32_window_scale = (float)dpix / 96.0f;
    }
    else {
        _sapp_win32_window_scale = 1.0f;
    }
    if (_sapp.desc.high_dpi) {
        _sapp_win32_content_scale = _sapp_win32_window_scale;
        _sapp_win32_mouse_scale = 1.0f;
    }
    else {
        _sapp_win32_content_scale = 1.0f;
        _sapp_win32_mouse_scale = 1.0f / _sapp_win32_window_scale;
    }
    _sapp.dpi_scale = _sapp_win32_content_scale;
    if (user32) {
        FreeLibrary(user32);
    }
    if (shcore) {
        FreeLibrary(shcore);
    }
}

_SOKOL_PRIVATE bool _sapp_win32_set_clipboard_string(const char* str) {
    SOKOL_ASSERT(str);
    SOKOL_ASSERT(_sapp_win32_hwnd);
    SOKOL_ASSERT(_sapp.clipboard_enabled && (_sapp.clipboard_size > 0));

    wchar_t* wchar_buf = 0;
    const int wchar_buf_size = _sapp.clipboard_size * sizeof(wchar_t);
    HANDLE object = GlobalAlloc(GMEM_MOVEABLE, wchar_buf_size);
    if (!object) {
        goto error;
    }
    wchar_buf = (wchar_t*) GlobalLock(object);
    if (!wchar_buf) {
        goto error;
    }
    if (!_sapp_win32_utf8_to_wide(str, wchar_buf, wchar_buf_size)) {
        goto error;
    }
    GlobalUnlock(wchar_buf);
    wchar_buf = 0;
    if (!OpenClipboard(_sapp_win32_hwnd)) {
        goto error;
    }
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, object);
    CloseClipboard();
    return true;

error:
    if (wchar_buf) {
        GlobalUnlock(object);
    }
    if (object) {
        GlobalFree(object);
    }
    return false;
}

_SOKOL_PRIVATE const char* _sapp_win32_get_clipboard_string(void) {
    SOKOL_ASSERT(_sapp.clipboard_enabled && _sapp.clipboard);
    SOKOL_ASSERT(_sapp_win32_hwnd);
    if (!OpenClipboard(_sapp_win32_hwnd)) {
        /* silently ignore any errors and just return the current
           content of the local clipboard buffer
        */
        return _sapp.clipboard;
    }
    HANDLE object = GetClipboardData(CF_UNICODETEXT);
    if (!object) {
        CloseClipboard();
        return _sapp.clipboard;
    }
    const wchar_t* wchar_buf = (const wchar_t*) GlobalLock(object);
    if (!wchar_buf) {
        CloseClipboard();
        return _sapp.clipboard;
    }
    _sapp_win32_wide_to_utf8(wchar_buf, _sapp.clipboard, _sapp.clipboard_size);
    GlobalUnlock(object);
    CloseClipboard();
    return _sapp.clipboard;
}

_SOKOL_PRIVATE void _sapp_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    _sapp_win32_init_keytable();
    _sapp_win32_utf8_to_wide(_sapp.window_title, _sapp.window_title_wide, sizeof(_sapp.window_title_wide));
    _sapp_win32_init_dpi();
    _sapp_win32_create_window();
    #if defined(SOKOL_D3D11)
        _sapp_d3d11_create_device_and_swapchain();
        _sapp_d3d11_create_default_render_target();
    #endif
    #if defined(SOKOL_GLCORE33)
        _sapp_wgl_init();
        _sapp_wgl_load_extensions();
        _sapp_wgl_create_context();
        #if !defined(SOKOL_WIN32_NO_GL_LOADER)
            _sapp_win32_gl_loadfuncs();
        #endif
    #endif
    _sapp.valid = true;

    bool done = false;
    while (!(done || _sapp.quit_ordered)) {
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                done = true;
                continue;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        _sapp_frame();
        #if defined(SOKOL_D3D11)
            IDXGISwapChain_Present(_sapp_dxgi_swap_chain, _sapp.swap_interval, 0);
            if (IsIconic(_sapp_win32_hwnd)) {
                Sleep(16 * _sapp.swap_interval);
            }
        #endif
        #if defined(SOKOL_GLCORE33)
            _sapp_wgl_swap_buffers();
        #endif
        /* check for window resized, this cannot happen in WM_SIZE as it explodes memory usage */
        if (_sapp_win32_update_dimensions()) {
            #if defined(SOKOL_D3D11)
            _sapp_d3d11_resize_default_render_target();
            #endif
            _sapp_win32_app_event(SAPP_EVENTTYPE_RESIZED);
        }
        if (_sapp.quit_requested) {
            PostMessage(_sapp_win32_hwnd, WM_CLOSE, 0, 0);
        }
    }
    _sapp_call_cleanup();

    #if defined(SOKOL_D3D11)
        _sapp_d3d11_destroy_default_render_target();
        _sapp_d3d11_destroy_device_and_swapchain();
    #else
        _sapp_wgl_destroy_context();
        _sapp_wgl_shutdown();
    #endif
    _sapp_win32_destroy_window();
    _sapp_discard_state();
}

_SOKOL_PRIVATE char** _sapp_win32_command_line_to_utf8_argv(LPWSTR w_command_line, int* o_argc) {
    int argc = 0;
    char** argv = 0;
    char* args;

    LPWSTR* w_argv = CommandLineToArgvW(w_command_line, &argc);
    if (w_argv == NULL) {
        _sapp_fail("Win32: failed to parse command line");
    } else {
        size_t size = wcslen(w_command_line) * 4;
        argv = (char**) SOKOL_CALLOC(1, (argc + 1) * sizeof(char*) + size);
        args = (char*)&argv[argc + 1];
        int n;
        for (int i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (int)size, NULL, NULL);
            if (n == 0) {
                _sapp_fail("Win32: failed to convert all arguments to utf8");
                break;
            }
            argv[i] = args;
            size -= n;
            args += n;
        }
        LocalFree(w_argv);
    }
    *o_argc = argc;
    return argv;
}

#if !defined(SOKOL_NO_ENTRY)
#if defined(SOKOL_WIN32_FORCE_MAIN)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_run(&desc);
    return 0;
}
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    _SOKOL_UNUSED(hInstance);
    _SOKOL_UNUSED(hPrevInstance);
    _SOKOL_UNUSED(lpCmdLine);
    _SOKOL_UNUSED(nCmdShow);
    int argc_utf8 = 0;
    char** argv_utf8 = _sapp_win32_command_line_to_utf8_argv(GetCommandLineW(), &argc_utf8);
    sapp_desc desc = sokol_main(argc_utf8, argv_utf8);
    _sapp_run(&desc);
    SOKOL_FREE(argv_utf8);
    return 0;
}
#endif /* SOKOL_WIN32_FORCE_MAIN */
#endif /* SOKOL_NO_ENTRY */
#undef _SAPP_SAFE_RELEASE
#endif /* WINDOWS */

/*== Android ================================================================*/
#if defined(__ANDROID__)
#include <pthread.h>
#include <unistd.h>
#include <android/native_activity.h>
#include <android/looper.h>

#include <EGL/egl.h>
#if defined(SOKOL_GLES3)
    #include <GLES3/gl3.h>
#else
    #ifndef GL_EXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

typedef struct {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int read_from_main_fd;
    int write_from_main_fd;
} _sapp_android_pt_t;

typedef struct {
    ANativeWindow* window;
    AInputQueue* input;
} _sapp_android_resources_t;

typedef enum {
    _SOKOL_ANDROID_MSG_CREATE,
    _SOKOL_ANDROID_MSG_RESUME,
    _SOKOL_ANDROID_MSG_PAUSE,
    _SOKOL_ANDROID_MSG_FOCUS,
    _SOKOL_ANDROID_MSG_NO_FOCUS,
    _SOKOL_ANDROID_MSG_SET_NATIVE_WINDOW,
    _SOKOL_ANDROID_MSG_SET_INPUT_QUEUE,
    _SOKOL_ANDROID_MSG_DESTROY,
} _sapp_android_msg_t;

typedef struct {
    ANativeActivity* activity;
    _sapp_android_pt_t pt;
    _sapp_android_resources_t pending;
    _sapp_android_resources_t current;
    ALooper* looper;
    bool is_thread_started;
    bool is_thread_stopping;
    bool is_thread_stopped;
    bool has_created;
    bool has_resumed;
    bool has_focus;
    EGLConfig config;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
} _sapp_android_state_t;

static _sapp_android_state_t _sapp_android_state;

/* android loop thread */
_SOKOL_PRIVATE bool _sapp_android_init_egl(void) {
    _sapp_android_state_t* state = &_sapp_android_state;
    SOKOL_ASSERT(state->display == EGL_NO_DISPLAY);
    SOKOL_ASSERT(state->context == EGL_NO_CONTEXT);

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return false;
    }
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE) {
        return false;
    }

    EGLint alpha_size = _sapp.desc.alpha ? 8 : 0;
    const EGLint cfg_attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, alpha_size,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 0,
        EGL_NONE,
    };
    EGLConfig available_cfgs[32];
    EGLint cfg_count;
    eglChooseConfig(display, cfg_attributes, available_cfgs, 32, &cfg_count);
    SOKOL_ASSERT(cfg_count > 0);
    SOKOL_ASSERT(cfg_count <= 32);

    /* find config with 8-bit rgb buffer if available, ndk sample does not trust egl spec */
    EGLConfig config;
    bool exact_cfg_found = false;
    for (int i = 0; i < cfg_count; ++i) {
        EGLConfig c = available_cfgs[i];
        EGLint r, g, b, a, d;
        if (eglGetConfigAttrib(display, c, EGL_RED_SIZE, &r) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_GREEN_SIZE, &g) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_BLUE_SIZE, &b) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_ALPHA_SIZE, &a) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_DEPTH_SIZE, &d) == EGL_TRUE &&
            r == 8 && g == 8 && b == 8 && (alpha_size == 0 || a == alpha_size) && d == 16) {
            exact_cfg_found = true;
            config = c;
            break;
        }
    }
    if (!exact_cfg_found) {
        config = available_cfgs[0];
    }

    EGLint ctx_attributes[] = {
        #if defined(SOKOL_GLES3)
            EGL_CONTEXT_CLIENT_VERSION, _sapp.desc.gl_force_gles2 ? 2 : 3,
        #else
            EGL_CONTEXT_CLIENT_VERSION, 2,
        #endif
        EGL_NONE,
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attributes);
    if (context == EGL_NO_CONTEXT) {
        return false;
    }

    state->config = config;
    state->display = display;
    state->context = context;
    return true;
}

_SOKOL_PRIVATE void _sapp_android_cleanup_egl(void) {
    _sapp_android_state_t* state = &_sapp_android_state;
    if (state->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (state->surface != EGL_NO_SURFACE) {
            SOKOL_LOG("Destroying egl surface");
            eglDestroySurface(state->display, state->surface);
            state->surface = EGL_NO_SURFACE;
        }
        if (state->context != EGL_NO_CONTEXT) {
            SOKOL_LOG("Destroying egl context");
            eglDestroyContext(state->display, state->context);
            state->context = EGL_NO_CONTEXT;
        }
        SOKOL_LOG("Terminating egl display");
        eglTerminate(state->display);
        state->display = EGL_NO_DISPLAY;
    }
}

_SOKOL_PRIVATE bool _sapp_android_init_egl_surface(ANativeWindow* window) {
    _sapp_android_state_t* state = &_sapp_android_state;
    SOKOL_ASSERT(state->display != EGL_NO_DISPLAY);
    SOKOL_ASSERT(state->context != EGL_NO_CONTEXT);
    SOKOL_ASSERT(state->surface == EGL_NO_SURFACE);
    SOKOL_ASSERT(window);

    /* TODO: set window flags */
    /* ANativeActivity_setWindowFlags(activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0); */

    /* create egl surface and make it current */
    EGLSurface surface = eglCreateWindowSurface(state->display, state->config, window, NULL);
    if (surface == EGL_NO_SURFACE) {
        return false;
    }
    if (eglMakeCurrent(state->display, surface, surface, state->context) == EGL_FALSE) {
        return false;
    }
    state->surface = surface;
    return true;
}

_SOKOL_PRIVATE void _sapp_android_cleanup_egl_surface(void) {
    _sapp_android_state_t* state = &_sapp_android_state;
    if (state->display == EGL_NO_DISPLAY) {
        return;
    }
    eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (state->surface != EGL_NO_SURFACE) {
        eglDestroySurface(state->display, state->surface);
        state->surface = EGL_NO_SURFACE;
    }
}

_SOKOL_PRIVATE void _sapp_android_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        SOKOL_LOG("event_cb()");
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_android_update_dimensions(ANativeWindow* window, bool force_update) {
    _sapp_android_state_t* state = &_sapp_android_state;
    SOKOL_ASSERT(state->display != EGL_NO_DISPLAY);
    SOKOL_ASSERT(state->context != EGL_NO_CONTEXT);
    SOKOL_ASSERT(state->surface != EGL_NO_SURFACE);
    SOKOL_ASSERT(window);

    const int32_t win_w = ANativeWindow_getWidth(window);
    const int32_t win_h = ANativeWindow_getHeight(window);
    SOKOL_ASSERT(win_w >= 0 && win_h >= 0);
    const bool win_changed = (win_w != _sapp.window_width) || (win_h != _sapp.window_height);
    _sapp.window_width = win_w;
    _sapp.window_height = win_h;
    if (win_changed || force_update) {
        if (!_sapp.desc.high_dpi) {
            const int32_t buf_w = win_w / 2;
            const int32_t buf_h = win_h / 2;
            EGLint format;
            EGLBoolean egl_result = eglGetConfigAttrib(state->display, state->config, EGL_NATIVE_VISUAL_ID, &format);
            SOKOL_ASSERT(egl_result == EGL_TRUE);
            /* NOTE: calling ANativeWindow_setBuffersGeometry() with the same dimensions
                as the ANativeWindow size results in weird display artefacts, that's
                why it's only called when the buffer geometry is different from
                the window size
            */
            int32_t result = ANativeWindow_setBuffersGeometry(window, buf_w, buf_h, format);
            SOKOL_ASSERT(result == 0);
        }
    }

    /* query surface size */
    EGLint fb_w, fb_h;
    EGLBoolean egl_result_w = eglQuerySurface(state->display, state->surface, EGL_WIDTH, &fb_w);
    EGLBoolean egl_result_h = eglQuerySurface(state->display, state->surface, EGL_HEIGHT, &fb_h);
    SOKOL_ASSERT(egl_result_w == EGL_TRUE);
    SOKOL_ASSERT(egl_result_h == EGL_TRUE);
    const bool fb_changed = (fb_w != _sapp.framebuffer_width) || (fb_h != _sapp.framebuffer_height);
    _sapp.framebuffer_width = fb_w;
    _sapp.framebuffer_height = fb_h;
    _sapp.dpi_scale = (float)_sapp.framebuffer_width / (float)_sapp.window_width;
    if (win_changed || fb_changed || force_update) {
        if (!_sapp.first_frame) {
            SOKOL_LOG("SAPP_EVENTTYPE_RESIZED");
            _sapp_android_app_event(SAPP_EVENTTYPE_RESIZED);
        }
    }
}

_SOKOL_PRIVATE void _sapp_android_cleanup(void) {
    _sapp_android_state_t* state = &_sapp_android_state;
    SOKOL_LOG("Cleaning up");
    if (state->surface != EGL_NO_SURFACE) {
        /* egl context is bound, cleanup gracefully */
        if (_sapp.init_called && !_sapp.cleanup_called) {
            SOKOL_LOG("cleanup_cb()");
            _sapp_call_cleanup();
        }
    }
    /* always try to cleanup by destroying egl context */
    _sapp_android_cleanup_egl();
}

_SOKOL_PRIVATE void _sapp_android_shutdown(void) {
    /* try to cleanup while we still have a surface and can call cleanup_cb() */
    _sapp_android_cleanup();
    /* request exit */
    ANativeActivity_finish(_sapp_android_state.activity);
}

_SOKOL_PRIVATE void _sapp_android_frame(void) {
    _sapp_android_state_t* state = &_sapp_android_state;
    SOKOL_ASSERT(state->display != EGL_NO_DISPLAY);
    SOKOL_ASSERT(state->context != EGL_NO_CONTEXT);
    SOKOL_ASSERT(state->surface != EGL_NO_SURFACE);
    _sapp_android_update_dimensions(state->current.window, false);
    _sapp_frame();
    eglSwapBuffers(state->display, _sapp_android_state.surface);
}

_SOKOL_PRIVATE bool _sapp_android_touch_event(const AInputEvent* e) {
    if (AInputEvent_getType(e) != AINPUT_EVENT_TYPE_MOTION) {
        return false;
    }
    if (!_sapp_events_enabled()) {
        return false;
    }
    int32_t action_idx = AMotionEvent_getAction(e);
    int32_t action = action_idx & AMOTION_EVENT_ACTION_MASK;
    sapp_event_type type = SAPP_EVENTTYPE_INVALID;
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN:
            SOKOL_LOG("Touch: down");
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            SOKOL_LOG("Touch: ptr down");
            type = SAPP_EVENTTYPE_TOUCHES_BEGAN;
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            type = SAPP_EVENTTYPE_TOUCHES_MOVED;
            break;
        case AMOTION_EVENT_ACTION_UP:
            SOKOL_LOG("Touch: up");
        case AMOTION_EVENT_ACTION_POINTER_UP:
            SOKOL_LOG("Touch: ptr up");
            type = SAPP_EVENTTYPE_TOUCHES_ENDED;
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            SOKOL_LOG("Touch: cancel");
            type = SAPP_EVENTTYPE_TOUCHES_CANCELLED;
            break;
        default:
            break;
    }
    if (type == SAPP_EVENTTYPE_INVALID) {
        return false;
    }
    int32_t idx = action_idx >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    _sapp_init_event(type);
    _sapp.event.num_touches = AMotionEvent_getPointerCount(e);
    if (_sapp.event.num_touches > SAPP_MAX_TOUCHPOINTS) {
        _sapp.event.num_touches = SAPP_MAX_TOUCHPOINTS;
    }
    for (int32_t i = 0; i < _sapp.event.num_touches; i++) {
        sapp_touchpoint* dst = &_sapp.event.touches[i];
        dst->identifier = AMotionEvent_getPointerId(e, i);
        dst->pos_x = (AMotionEvent_getRawX(e, i) / _sapp.window_width) * _sapp.framebuffer_width;
        dst->pos_y = (AMotionEvent_getRawY(e, i) / _sapp.window_height) * _sapp.framebuffer_height;

        if (action == AMOTION_EVENT_ACTION_POINTER_DOWN ||
            action == AMOTION_EVENT_ACTION_POINTER_UP) {
            dst->changed = i == idx;
        } else {
            dst->changed = true;
        }
    }
    _sapp_call_event(&_sapp.event);
    return true;
}

_SOKOL_PRIVATE bool _sapp_android_key_event(const AInputEvent* e) {
    if (AInputEvent_getType(e) != AINPUT_EVENT_TYPE_KEY) {
        return false;
    }
    if (AKeyEvent_getKeyCode(e) == AKEYCODE_BACK) {
        /* FIXME: this should be hooked into a "really quit?" mechanism
           so the app can ask the user for confirmation, this is currently
           generally missing in sokol_app.h
        */
        _sapp_android_shutdown();
        return true;
    }
    return false;
}

_SOKOL_PRIVATE int _sapp_android_input_cb(int fd, int events, void* data) {
    if ((events & ALOOPER_EVENT_INPUT) == 0) {
        SOKOL_LOG("_sapp_android_input_cb() encountered unsupported event");
        return 1;
    }
    _sapp_android_state_t* state = &_sapp_android_state;;
    SOKOL_ASSERT(state->current.input);
    AInputEvent* event = NULL;
    while (AInputQueue_getEvent(state->current.input, &event) >= 0) {
        if (AInputQueue_preDispatchEvent(state->current.input, event) != 0) {
            continue;
        }
        int32_t handled = 0;
        if (_sapp_android_touch_event(event) || _sapp_android_key_event(event)) {
            handled = 1;
        }
        AInputQueue_finishEvent(state->current.input, event, handled);
    }
    return 1;
}

_SOKOL_PRIVATE int _sapp_android_main_cb(int fd, int events, void* data) {
    if ((events & ALOOPER_EVENT_INPUT) == 0) {
        SOKOL_LOG("_sapp_android_main_cb() encountered unsupported event");
        return 1;
    }
    _sapp_android_state_t* state = &_sapp_android_state;

    _sapp_android_msg_t msg;
    if (read(fd, &msg, sizeof(msg)) != sizeof(msg)) {
        SOKOL_LOG("Could not write to read_from_main_fd");
        return 1;
    }

    pthread_mutex_lock(&state->pt.mutex);
    switch (msg) {
        case _SOKOL_ANDROID_MSG_CREATE:
            {
                SOKOL_LOG("MSG_CREATE");
                SOKOL_ASSERT(!_sapp.valid);
                bool result = _sapp_android_init_egl();
                SOKOL_ASSERT(result);
                _sapp.valid = true;
                state->has_created = true;
            }
            break;
        case _SOKOL_ANDROID_MSG_RESUME:
            SOKOL_LOG("MSG_RESUME");
            state->has_resumed = true;
            _sapp_android_app_event(SAPP_EVENTTYPE_RESUMED);
            break;
        case _SOKOL_ANDROID_MSG_PAUSE:
            SOKOL_LOG("MSG_PAUSE");
            state->has_resumed = false;
            _sapp_android_app_event(SAPP_EVENTTYPE_SUSPENDED);
            break;
        case _SOKOL_ANDROID_MSG_FOCUS:
            SOKOL_LOG("MSG_FOCUS");
            state->has_focus = true;
            break;
        case _SOKOL_ANDROID_MSG_NO_FOCUS:
            SOKOL_LOG("MSG_NO_FOCUS");
            state->has_focus = false;
            break;
        case _SOKOL_ANDROID_MSG_SET_NATIVE_WINDOW:
            SOKOL_LOG("MSG_SET_NATIVE_WINDOW");
            if (state->current.window != state->pending.window) {
                if (state->current.window != NULL) {
                    _sapp_android_cleanup_egl_surface();
                }
                if (state->pending.window != NULL) {
                    SOKOL_LOG("Creating egl surface ...");
                    if (_sapp_android_init_egl_surface(state->pending.window)) {
                        SOKOL_LOG("... ok!");
                        _sapp_android_update_dimensions(state->pending.window, true);
                    } else {
                        SOKOL_LOG("... failed!");
                        _sapp_android_shutdown();
                    }
                }
            }
            state->current.window = state->pending.window;
            break;
        case _SOKOL_ANDROID_MSG_SET_INPUT_QUEUE:
            SOKOL_LOG("MSG_SET_INPUT_QUEUE");
            if (state->current.input != state->pending.input) {
                if (state->current.input != NULL) {
                    AInputQueue_detachLooper(state->current.input);
                }
                if (state->pending.input != NULL) {
                    AInputQueue_attachLooper(
                        state->pending.input,
                        state->looper,
                        ALOOPER_POLL_CALLBACK,
                        _sapp_android_input_cb,
                        NULL); /* data */
                }
            }
            state->current.input = state->pending.input;
            break;
        case _SOKOL_ANDROID_MSG_DESTROY:
            SOKOL_LOG("MSG_DESTROY");
            _sapp_android_cleanup();
            _sapp.valid = false;
            state->is_thread_stopping = true;
            break;
        default:
            SOKOL_LOG("Unknown msg type received");
            break;
    }
    pthread_cond_broadcast(&state->pt.cond); /* signal "received" */
    pthread_mutex_unlock(&state->pt.mutex);
    return 1;
}

_SOKOL_PRIVATE bool _sapp_android_should_update(void) {
    bool is_in_front = _sapp_android_state.has_resumed && _sapp_android_state.has_focus;
    bool has_surface = _sapp_android_state.surface != EGL_NO_SURFACE;
    return is_in_front && has_surface;
}

_SOKOL_PRIVATE void _sapp_android_show_keyboard(bool shown) {
    SOKOL_ASSERT(_sapp.valid);
    /* This seems to be broken in the NDK, but there is (a very cumbersome) workaround... */
    if (shown) {
        SOKOL_LOG("Showing keyboard");
        ANativeActivity_showSoftInput(_sapp_android_state.activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
    } else {
        SOKOL_LOG("Hiding keyboard");
        ANativeActivity_hideSoftInput(_sapp_android_state.activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
    }
}

_SOKOL_PRIVATE void* _sapp_android_loop(void* obj) {
    SOKOL_LOG("Loop thread started");
    _sapp_android_state_t* state = (_sapp_android_state_t*)obj;

    state->looper = ALooper_prepare(0 /* or ALOOPER_PREPARE_ALLOW_NON_CALLBACKS*/);
    ALooper_addFd(state->looper,
        state->pt.read_from_main_fd,
        ALOOPER_POLL_CALLBACK,
        ALOOPER_EVENT_INPUT,
        _sapp_android_main_cb,
        NULL); /* data */

    /* signal start to main thread */
    pthread_mutex_lock(&state->pt.mutex);
    state->is_thread_started = true;
    pthread_cond_broadcast(&state->pt.cond);
    pthread_mutex_unlock(&state->pt.mutex);

    /* main loop */
    while (!state->is_thread_stopping) {
        /* sokol frame */
        if (_sapp_android_should_update()) {
            _sapp_android_frame();
        }

        /* process all events (or stop early if app is requested to quit) */
        bool process_events = true;
        while (process_events && !state->is_thread_stopping) {
            bool block_until_event = !state->is_thread_stopping && !_sapp_android_should_update();
            process_events = ALooper_pollOnce(block_until_event ? -1 : 0, NULL, NULL, NULL) == ALOOPER_POLL_CALLBACK;
        }
    }

    /* cleanup thread */
    if (state->current.input != NULL) {
        AInputQueue_detachLooper(state->current.input);
    }

    /* the following causes heap corruption on exit, why??
    ALooper_removeFd(state->looper, state->pt.read_from_main_fd);
    ALooper_release(state->looper);*/

    /* signal "destroyed" */
    pthread_mutex_lock(&state->pt.mutex);
    state->is_thread_stopped = true;
    pthread_cond_broadcast(&state->pt.cond);
    pthread_mutex_unlock(&state->pt.mutex);
    SOKOL_LOG("Loop thread done");
    return NULL;
}

/* android main/ui thread */
_SOKOL_PRIVATE void _sapp_android_msg(_sapp_android_state_t* state, _sapp_android_msg_t msg) {
    if (write(state->pt.write_from_main_fd, &msg, sizeof(msg)) != sizeof(msg)) {
        SOKOL_LOG("Could not write to write_from_main_fd");
    }
}

_SOKOL_PRIVATE void _sapp_android_on_start(ANativeActivity* activity) {
    SOKOL_LOG("NativeActivity onStart()");
}

_SOKOL_PRIVATE void _sapp_android_on_resume(ANativeActivity* activity) {
    SOKOL_LOG("NativeActivity onResume()");
    _sapp_android_msg(&_sapp_android_state, _SOKOL_ANDROID_MSG_RESUME);
}

_SOKOL_PRIVATE void* _sapp_android_on_save_instance_state(ANativeActivity* activity, size_t* out_size) {
    SOKOL_LOG("NativeActivity onSaveInstanceState()");
    *out_size = 0;
    return NULL;
}

_SOKOL_PRIVATE void _sapp_android_on_window_focus_changed(ANativeActivity* activity, int has_focus) {
    SOKOL_LOG("NativeActivity onWindowFocusChanged()");
    if (has_focus) {
        _sapp_android_msg(&_sapp_android_state, _SOKOL_ANDROID_MSG_FOCUS);
    } else {
        _sapp_android_msg(&_sapp_android_state, _SOKOL_ANDROID_MSG_NO_FOCUS);
    }
}

_SOKOL_PRIVATE void _sapp_android_on_pause(ANativeActivity* activity) {
    SOKOL_LOG("NativeActivity onPause()");
    _sapp_android_msg(&_sapp_android_state, _SOKOL_ANDROID_MSG_PAUSE);
}

_SOKOL_PRIVATE void _sapp_android_on_stop(ANativeActivity* activity) {
    SOKOL_LOG("NativeActivity onStop()");
}

_SOKOL_PRIVATE void _sapp_android_msg_set_native_window(_sapp_android_state_t* state, ANativeWindow* window) {
    pthread_mutex_lock(&state->pt.mutex);
    state->pending.window = window;
    _sapp_android_msg(state, _SOKOL_ANDROID_MSG_SET_NATIVE_WINDOW);
    while (state->current.window != window) {
        pthread_cond_wait(&state->pt.cond, &state->pt.mutex);
    }
    pthread_mutex_unlock(&state->pt.mutex);
}

_SOKOL_PRIVATE void _sapp_android_on_native_window_created(ANativeActivity* activity, ANativeWindow* window) {
    SOKOL_LOG("NativeActivity onNativeWindowCreated()");
    _sapp_android_msg_set_native_window(&_sapp_android_state, window);
}

_SOKOL_PRIVATE void _sapp_android_on_native_window_destroyed(ANativeActivity* activity, ANativeWindow* window) {
    SOKOL_LOG("NativeActivity onNativeWindowDestroyed()");
    _sapp_android_msg_set_native_window(&_sapp_android_state, NULL);
}

_SOKOL_PRIVATE void _sapp_android_msg_set_input_queue(_sapp_android_state_t* state, AInputQueue* input) {
    pthread_mutex_lock(&state->pt.mutex);
    state->pending.input = input;
    _sapp_android_msg(state, _SOKOL_ANDROID_MSG_SET_INPUT_QUEUE);
    while (state->current.input != input) {
        pthread_cond_wait(&state->pt.cond, &state->pt.mutex);
    }
    pthread_mutex_unlock(&state->pt.mutex);
}

_SOKOL_PRIVATE void _sapp_android_on_input_queue_created(ANativeActivity* activity, AInputQueue* queue) {
    SOKOL_LOG("NativeActivity onInputQueueCreated()");
    _sapp_android_msg_set_input_queue(&_sapp_android_state, queue);
}

_SOKOL_PRIVATE void _sapp_android_on_input_queue_destroyed(ANativeActivity* activity, AInputQueue* queue) {
    SOKOL_LOG("NativeActivity onInputQueueDestroyed()");
    _sapp_android_msg_set_input_queue(&_sapp_android_state, NULL);
}

_SOKOL_PRIVATE void _sapp_android_on_config_changed(ANativeActivity* activity) {
    SOKOL_LOG("NativeActivity onConfigurationChanged()");
    /* see android:configChanges in manifest */
}

_SOKOL_PRIVATE void _sapp_android_on_low_memory(ANativeActivity* activity) {
    SOKOL_LOG("NativeActivity onLowMemory()");
}

_SOKOL_PRIVATE void _sapp_android_on_destroy(ANativeActivity* activity) {
    /*
     * For some reason even an empty app using nativeactivity.h will crash (WIN DEATH)
     * on my device (Moto X 2nd gen) when the app is removed from the task view
     * (TaskStackView: onTaskViewDismissed).
     *
     * However, if ANativeActivity_finish() is explicitly called from for example
     * _sapp_android_on_stop(), the crash disappears. Is this a bug in NativeActivity?
     */
    SOKOL_LOG("NativeActivity onDestroy()");
    _sapp_android_state_t* state = &_sapp_android_state;

    /* send destroy msg */
    pthread_mutex_lock(&state->pt.mutex);
    _sapp_android_msg(state, _SOKOL_ANDROID_MSG_DESTROY);
    while (!_sapp_android_state.is_thread_stopped) {
        pthread_cond_wait(&state->pt.cond, &state->pt.mutex);
    }
    pthread_mutex_unlock(&state->pt.mutex);

    /* clean up main thread */
    pthread_cond_destroy(&state->pt.cond);
    pthread_mutex_destroy(&state->pt.mutex);

    close(state->pt.read_from_main_fd);
    close(state->pt.write_from_main_fd);

    SOKOL_LOG("NativeActivity done");

    /* this is a bit naughty, but causes a clean restart of the app (static globals are reset) */
    exit(0);
}

JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* saved_state, size_t saved_state_size) {
    SOKOL_LOG("NativeActivity onCreate()");

    sapp_desc desc = sokol_main(0, NULL);
    _sapp_init_state(&desc);

    /* start loop thread */
    _sapp_android_state = (_sapp_android_state_t){0};
    _sapp_android_state_t* state = &_sapp_android_state;

    state->activity = activity;

    int pipe_fd[2];
    if (pipe(pipe_fd) != 0) {
        SOKOL_LOG("Could not create thread pipe");
        return;
    }
    state->pt.read_from_main_fd = pipe_fd[0];
    state->pt.write_from_main_fd = pipe_fd[1];

    pthread_mutex_init(&state->pt.mutex, NULL);
    pthread_cond_init(&state->pt.cond, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&state->pt.thread, &attr, _sapp_android_loop, state);
    pthread_attr_destroy(&attr);

    /* wait until main loop has started */
    pthread_mutex_lock(&state->pt.mutex);
    while (!state->is_thread_started) {
        pthread_cond_wait(&state->pt.cond, &state->pt.mutex);
    }
    pthread_mutex_unlock(&state->pt.mutex);

    /* send create msg */
    pthread_mutex_lock(&state->pt.mutex);
    _sapp_android_msg(state, _SOKOL_ANDROID_MSG_CREATE);
    while (!state->has_created) {
        pthread_cond_wait(&state->pt.cond, &state->pt.mutex);
    }
    pthread_mutex_unlock(&state->pt.mutex);

    /* register for callbacks */
    activity->instance = state;
    activity->callbacks->onStart = _sapp_android_on_start;
    activity->callbacks->onResume = _sapp_android_on_resume;
    activity->callbacks->onSaveInstanceState = _sapp_android_on_save_instance_state;
    activity->callbacks->onWindowFocusChanged = _sapp_android_on_window_focus_changed;
    activity->callbacks->onPause = _sapp_android_on_pause;
    activity->callbacks->onStop = _sapp_android_on_stop;
    activity->callbacks->onDestroy = _sapp_android_on_destroy;
    activity->callbacks->onNativeWindowCreated = _sapp_android_on_native_window_created;
    /* activity->callbacks->onNativeWindowResized = _sapp_android_on_native_window_resized; */
    /* activity->callbacks->onNativeWindowRedrawNeeded = _sapp_android_on_native_window_redraw_needed; */
    activity->callbacks->onNativeWindowDestroyed = _sapp_android_on_native_window_destroyed;
    activity->callbacks->onInputQueueCreated = _sapp_android_on_input_queue_created;
    activity->callbacks->onInputQueueDestroyed = _sapp_android_on_input_queue_destroyed;
    /* activity->callbacks->onContentRectChanged = _sapp_android_on_content_rect_changed; */
    activity->callbacks->onConfigurationChanged = _sapp_android_on_config_changed;
    activity->callbacks->onLowMemory = _sapp_android_on_low_memory;

    SOKOL_LOG("NativeActivity successfully created");

    /* NOT A BUG: do NOT call sapp_discard_state() */
}

#endif /* Android */

/*== LINUX ==================================================================*/
#if (defined(__linux__) || defined(__unix__)) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
#define GL_GLEXT_PROTOTYPES
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xresource.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xmd.h> /* CARD32 */
#include <GL/gl.h>
#include <dlfcn.h> /* dlopen, dlsym, dlclose */
#include <limits.h> /* LONG_MAX */

#define GLX_VENDOR 1
#define GLX_RGBA_BIT 0x00000001
#define GLX_WINDOW_BIT 0x00000001
#define GLX_DRAWABLE_TYPE 0x8010
#define GLX_RENDER_TYPE	0x8011
#define GLX_RGBA_TYPE 0x8014
#define GLX_DOUBLEBUFFER 5
#define GLX_STEREO 6
#define GLX_AUX_BUFFERS	7
#define GLX_RED_SIZE 8
#define GLX_GREEN_SIZE 9
#define GLX_BLUE_SIZE 10
#define GLX_ALPHA_SIZE 11
#define GLX_DEPTH_SIZE 12
#define GLX_STENCIL_SIZE 13
#define GLX_ACCUM_RED_SIZE 14
#define GLX_ACCUM_GREEN_SIZE 15
#define GLX_ACCUM_BLUE_SIZE	16
#define GLX_ACCUM_ALPHA_SIZE 17
#define GLX_SAMPLES 0x186a1
#define GLX_VISUAL_ID 0x800b

#define GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20b2
#define GLX_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#define GLX_CONTEXT_FLAGS_ARB 0x2094
#define GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define GLX_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GLX_NO_RESET_NOTIFICATION_ARB 0x8261
#define GLX_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define GLX_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define GLX_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098

typedef XID GLXWindow;
typedef XID GLXDrawable;
typedef struct __GLXFBConfig* GLXFBConfig;
typedef struct __GLXcontext* GLXContext;
typedef void (*__GLXextproc)(void);

typedef int (*PFNGLXGETFBCONFIGATTRIBPROC)(Display*,GLXFBConfig,int,int*);
typedef const char* (*PFNGLXGETCLIENTSTRINGPROC)(Display*,int);
typedef Bool (*PFNGLXQUERYEXTENSIONPROC)(Display*,int*,int*);
typedef Bool (*PFNGLXQUERYVERSIONPROC)(Display*,int*,int*);
typedef void (*PFNGLXDESTROYCONTEXTPROC)(Display*,GLXContext);
typedef Bool (*PFNGLXMAKECURRENTPROC)(Display*,GLXDrawable,GLXContext);
typedef void (*PFNGLXSWAPBUFFERSPROC)(Display*,GLXDrawable);
typedef const char* (*PFNGLXQUERYEXTENSIONSSTRINGPROC)(Display*,int);
typedef GLXFBConfig* (*PFNGLXGETFBCONFIGSPROC)(Display*,int,int*);
typedef GLXContext (*PFNGLXCREATENEWCONTEXTPROC)(Display*,GLXFBConfig,int,GLXContext,Bool);
typedef __GLXextproc (* PFNGLXGETPROCADDRESSPROC)(const GLubyte *procName);
typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*,GLXDrawable,int);
typedef XVisualInfo* (*PFNGLXGETVISUALFROMFBCONFIGPROC)(Display*,GLXFBConfig);
typedef GLXWindow (*PFNGLXCREATEWINDOWPROC)(Display*,GLXFBConfig,Window,const int*);
typedef void (*PFNGLXDESTROYWINDOWPROC)(Display*,GLXWindow);

typedef int (*PFNGLXSWAPINTERVALMESAPROC)(int);
typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display*,GLXFBConfig,GLXContext,Bool,const int*);

static Display* _sapp_x11_display;
static int _sapp_x11_screen;
static Window _sapp_x11_root;
static Colormap _sapp_x11_colormap;
static Window _sapp_x11_window;
static float _sapp_x11_dpi;
static int _sapp_x11_window_state;
static unsigned char _sapp_x11_error_code;
static void* _sapp_glx_libgl;
static int _sapp_glx_major;
static int _sapp_glx_minor;
static int _sapp_glx_eventbase;
static int _sapp_glx_errorbase;
static GLXContext _sapp_glx_ctx;
static GLXWindow _sapp_glx_window;
static Atom _sapp_x11_UTF8_STRING;
static Atom _sapp_x11_WM_PROTOCOLS;
static Atom _sapp_x11_WM_DELETE_WINDOW;
static Atom _sapp_x11_WM_STATE;
static Atom _sapp_x11_NET_WM_NAME;
static Atom _sapp_x11_NET_WM_ICON_NAME;
// GLX 1.3 functions
static PFNGLXGETFBCONFIGSPROC              _sapp_glx_GetFBConfigs;
static PFNGLXGETFBCONFIGATTRIBPROC         _sapp_glx_GetFBConfigAttrib;
static PFNGLXGETCLIENTSTRINGPROC           _sapp_glx_GetClientString;
static PFNGLXQUERYEXTENSIONPROC            _sapp_glx_QueryExtension;
static PFNGLXQUERYVERSIONPROC              _sapp_glx_QueryVersion;
static PFNGLXDESTROYCONTEXTPROC            _sapp_glx_DestroyContext;
static PFNGLXMAKECURRENTPROC               _sapp_glx_MakeCurrent;
static PFNGLXSWAPBUFFERSPROC               _sapp_glx_SwapBuffers;
static PFNGLXQUERYEXTENSIONSSTRINGPROC     _sapp_glx_QueryExtensionsString;
static PFNGLXCREATENEWCONTEXTPROC          _sapp_glx_CreateNewContext;
static PFNGLXGETVISUALFROMFBCONFIGPROC     _sapp_glx_GetVisualFromFBConfig;
static PFNGLXCREATEWINDOWPROC              _sapp_glx_CreateWindow;
static PFNGLXDESTROYWINDOWPROC             _sapp_glx_DestroyWindow;

// GLX 1.4 and extension functions
static PFNGLXGETPROCADDRESSPROC            _sapp_glx_GetProcAddress;
static PFNGLXGETPROCADDRESSPROC            _sapp_glx_GetProcAddressARB;
static PFNGLXSWAPINTERVALEXTPROC           _sapp_glx_SwapIntervalEXT;
static PFNGLXSWAPINTERVALMESAPROC          _sapp_glx_SwapIntervalMESA;
static PFNGLXCREATECONTEXTATTRIBSARBPROC   _sapp_glx_CreateContextAttribsARB;
static bool _sapp_glx_EXT_swap_control;
static bool _sapp_glx_MESA_swap_control;
static bool _sapp_glx_ARB_multisample;
static bool _sapp_glx_ARB_framebuffer_sRGB;
static bool _sapp_glx_EXT_framebuffer_sRGB;
static bool _sapp_glx_ARB_create_context;
static bool _sapp_glx_ARB_create_context_profile;

/* see GLFW's xkb_unicode.c */
static const struct _sapp_x11_codepair {
  uint16_t keysym;
  uint16_t ucs;
} _sapp_x11_keysymtab[] = {
  { 0x01a1, 0x0104 },
  { 0x01a2, 0x02d8 },
  { 0x01a3, 0x0141 },
  { 0x01a5, 0x013d },
  { 0x01a6, 0x015a },
  { 0x01a9, 0x0160 },
  { 0x01aa, 0x015e },
  { 0x01ab, 0x0164 },
  { 0x01ac, 0x0179 },
  { 0x01ae, 0x017d },
  { 0x01af, 0x017b },
  { 0x01b1, 0x0105 },
  { 0x01b2, 0x02db },
  { 0x01b3, 0x0142 },
  { 0x01b5, 0x013e },
  { 0x01b6, 0x015b },
  { 0x01b7, 0x02c7 },
  { 0x01b9, 0x0161 },
  { 0x01ba, 0x015f },
  { 0x01bb, 0x0165 },
  { 0x01bc, 0x017a },
  { 0x01bd, 0x02dd },
  { 0x01be, 0x017e },
  { 0x01bf, 0x017c },
  { 0x01c0, 0x0154 },
  { 0x01c3, 0x0102 },
  { 0x01c5, 0x0139 },
  { 0x01c6, 0x0106 },
  { 0x01c8, 0x010c },
  { 0x01ca, 0x0118 },
  { 0x01cc, 0x011a },
  { 0x01cf, 0x010e },
  { 0x01d0, 0x0110 },
  { 0x01d1, 0x0143 },
  { 0x01d2, 0x0147 },
  { 0x01d5, 0x0150 },
  { 0x01d8, 0x0158 },
  { 0x01d9, 0x016e },
  { 0x01db, 0x0170 },
  { 0x01de, 0x0162 },
  { 0x01e0, 0x0155 },
  { 0x01e3, 0x0103 },
  { 0x01e5, 0x013a },
  { 0x01e6, 0x0107 },
  { 0x01e8, 0x010d },
  { 0x01ea, 0x0119 },
  { 0x01ec, 0x011b },
  { 0x01ef, 0x010f },
  { 0x01f0, 0x0111 },
  { 0x01f1, 0x0144 },
  { 0x01f2, 0x0148 },
  { 0x01f5, 0x0151 },
  { 0x01f8, 0x0159 },
  { 0x01f9, 0x016f },
  { 0x01fb, 0x0171 },
  { 0x01fe, 0x0163 },
  { 0x01ff, 0x02d9 },
  { 0x02a1, 0x0126 },
  { 0x02a6, 0x0124 },
  { 0x02a9, 0x0130 },
  { 0x02ab, 0x011e },
  { 0x02ac, 0x0134 },
  { 0x02b1, 0x0127 },
  { 0x02b6, 0x0125 },
  { 0x02b9, 0x0131 },
  { 0x02bb, 0x011f },
  { 0x02bc, 0x0135 },
  { 0x02c5, 0x010a },
  { 0x02c6, 0x0108 },
  { 0x02d5, 0x0120 },
  { 0x02d8, 0x011c },
  { 0x02dd, 0x016c },
  { 0x02de, 0x015c },
  { 0x02e5, 0x010b },
  { 0x02e6, 0x0109 },
  { 0x02f5, 0x0121 },
  { 0x02f8, 0x011d },
  { 0x02fd, 0x016d },
  { 0x02fe, 0x015d },
  { 0x03a2, 0x0138 },
  { 0x03a3, 0x0156 },
  { 0x03a5, 0x0128 },
  { 0x03a6, 0x013b },
  { 0x03aa, 0x0112 },
  { 0x03ab, 0x0122 },
  { 0x03ac, 0x0166 },
  { 0x03b3, 0x0157 },
  { 0x03b5, 0x0129 },
  { 0x03b6, 0x013c },
  { 0x03ba, 0x0113 },
  { 0x03bb, 0x0123 },
  { 0x03bc, 0x0167 },
  { 0x03bd, 0x014a },
  { 0x03bf, 0x014b },
  { 0x03c0, 0x0100 },
  { 0x03c7, 0x012e },
  { 0x03cc, 0x0116 },
  { 0x03cf, 0x012a },
  { 0x03d1, 0x0145 },
  { 0x03d2, 0x014c },
  { 0x03d3, 0x0136 },
  { 0x03d9, 0x0172 },
  { 0x03dd, 0x0168 },
  { 0x03de, 0x016a },
  { 0x03e0, 0x0101 },
  { 0x03e7, 0x012f },
  { 0x03ec, 0x0117 },
  { 0x03ef, 0x012b },
  { 0x03f1, 0x0146 },
  { 0x03f2, 0x014d },
  { 0x03f3, 0x0137 },
  { 0x03f9, 0x0173 },
  { 0x03fd, 0x0169 },
  { 0x03fe, 0x016b },
  { 0x047e, 0x203e },
  { 0x04a1, 0x3002 },
  { 0x04a2, 0x300c },
  { 0x04a3, 0x300d },
  { 0x04a4, 0x3001 },
  { 0x04a5, 0x30fb },
  { 0x04a6, 0x30f2 },
  { 0x04a7, 0x30a1 },
  { 0x04a8, 0x30a3 },
  { 0x04a9, 0x30a5 },
  { 0x04aa, 0x30a7 },
  { 0x04ab, 0x30a9 },
  { 0x04ac, 0x30e3 },
  { 0x04ad, 0x30e5 },
  { 0x04ae, 0x30e7 },
  { 0x04af, 0x30c3 },
  { 0x04b0, 0x30fc },
  { 0x04b1, 0x30a2 },
  { 0x04b2, 0x30a4 },
  { 0x04b3, 0x30a6 },
  { 0x04b4, 0x30a8 },
  { 0x04b5, 0x30aa },
  { 0x04b6, 0x30ab },
  { 0x04b7, 0x30ad },
  { 0x04b8, 0x30af },
  { 0x04b9, 0x30b1 },
  { 0x04ba, 0x30b3 },
  { 0x04bb, 0x30b5 },
  { 0x04bc, 0x30b7 },
  { 0x04bd, 0x30b9 },
  { 0x04be, 0x30bb },
  { 0x04bf, 0x30bd },
  { 0x04c0, 0x30bf },
  { 0x04c1, 0x30c1 },
  { 0x04c2, 0x30c4 },
  { 0x04c3, 0x30c6 },
  { 0x04c4, 0x30c8 },
  { 0x04c5, 0x30ca },
  { 0x04c6, 0x30cb },
  { 0x04c7, 0x30cc },
  { 0x04c8, 0x30cd },
  { 0x04c9, 0x30ce },
  { 0x04ca, 0x30cf },
  { 0x04cb, 0x30d2 },
  { 0x04cc, 0x30d5 },
  { 0x04cd, 0x30d8 },
  { 0x04ce, 0x30db },
  { 0x04cf, 0x30de },
  { 0x04d0, 0x30df },
  { 0x04d1, 0x30e0 },
  { 0x04d2, 0x30e1 },
  { 0x04d3, 0x30e2 },
  { 0x04d4, 0x30e4 },
  { 0x04d5, 0x30e6 },
  { 0x04d6, 0x30e8 },
  { 0x04d7, 0x30e9 },
  { 0x04d8, 0x30ea },
  { 0x04d9, 0x30eb },
  { 0x04da, 0x30ec },
  { 0x04db, 0x30ed },
  { 0x04dc, 0x30ef },
  { 0x04dd, 0x30f3 },
  { 0x04de, 0x309b },
  { 0x04df, 0x309c },
  { 0x05ac, 0x060c },
  { 0x05bb, 0x061b },
  { 0x05bf, 0x061f },
  { 0x05c1, 0x0621 },
  { 0x05c2, 0x0622 },
  { 0x05c3, 0x0623 },
  { 0x05c4, 0x0624 },
  { 0x05c5, 0x0625 },
  { 0x05c6, 0x0626 },
  { 0x05c7, 0x0627 },
  { 0x05c8, 0x0628 },
  { 0x05c9, 0x0629 },
  { 0x05ca, 0x062a },
  { 0x05cb, 0x062b },
  { 0x05cc, 0x062c },
  { 0x05cd, 0x062d },
  { 0x05ce, 0x062e },
  { 0x05cf, 0x062f },
  { 0x05d0, 0x0630 },
  { 0x05d1, 0x0631 },
  { 0x05d2, 0x0632 },
  { 0x05d3, 0x0633 },
  { 0x05d4, 0x0634 },
  { 0x05d5, 0x0635 },
  { 0x05d6, 0x0636 },
  { 0x05d7, 0x0637 },
  { 0x05d8, 0x0638 },
  { 0x05d9, 0x0639 },
  { 0x05da, 0x063a },
  { 0x05e0, 0x0640 },
  { 0x05e1, 0x0641 },
  { 0x05e2, 0x0642 },
  { 0x05e3, 0x0643 },
  { 0x05e4, 0x0644 },
  { 0x05e5, 0x0645 },
  { 0x05e6, 0x0646 },
  { 0x05e7, 0x0647 },
  { 0x05e8, 0x0648 },
  { 0x05e9, 0x0649 },
  { 0x05ea, 0x064a },
  { 0x05eb, 0x064b },
  { 0x05ec, 0x064c },
  { 0x05ed, 0x064d },
  { 0x05ee, 0x064e },
  { 0x05ef, 0x064f },
  { 0x05f0, 0x0650 },
  { 0x05f1, 0x0651 },
  { 0x05f2, 0x0652 },
  { 0x06a1, 0x0452 },
  { 0x06a2, 0x0453 },
  { 0x06a3, 0x0451 },
  { 0x06a4, 0x0454 },
  { 0x06a5, 0x0455 },
  { 0x06a6, 0x0456 },
  { 0x06a7, 0x0457 },
  { 0x06a8, 0x0458 },
  { 0x06a9, 0x0459 },
  { 0x06aa, 0x045a },
  { 0x06ab, 0x045b },
  { 0x06ac, 0x045c },
  { 0x06ae, 0x045e },
  { 0x06af, 0x045f },
  { 0x06b0, 0x2116 },
  { 0x06b1, 0x0402 },
  { 0x06b2, 0x0403 },
  { 0x06b3, 0x0401 },
  { 0x06b4, 0x0404 },
  { 0x06b5, 0x0405 },
  { 0x06b6, 0x0406 },
  { 0x06b7, 0x0407 },
  { 0x06b8, 0x0408 },
  { 0x06b9, 0x0409 },
  { 0x06ba, 0x040a },
  { 0x06bb, 0x040b },
  { 0x06bc, 0x040c },
  { 0x06be, 0x040e },
  { 0x06bf, 0x040f },
  { 0x06c0, 0x044e },
  { 0x06c1, 0x0430 },
  { 0x06c2, 0x0431 },
  { 0x06c3, 0x0446 },
  { 0x06c4, 0x0434 },
  { 0x06c5, 0x0435 },
  { 0x06c6, 0x0444 },
  { 0x06c7, 0x0433 },
  { 0x06c8, 0x0445 },
  { 0x06c9, 0x0438 },
  { 0x06ca, 0x0439 },
  { 0x06cb, 0x043a },
  { 0x06cc, 0x043b },
  { 0x06cd, 0x043c },
  { 0x06ce, 0x043d },
  { 0x06cf, 0x043e },
  { 0x06d0, 0x043f },
  { 0x06d1, 0x044f },
  { 0x06d2, 0x0440 },
  { 0x06d3, 0x0441 },
  { 0x06d4, 0x0442 },
  { 0x06d5, 0x0443 },
  { 0x06d6, 0x0436 },
  { 0x06d7, 0x0432 },
  { 0x06d8, 0x044c },
  { 0x06d9, 0x044b },
  { 0x06da, 0x0437 },
  { 0x06db, 0x0448 },
  { 0x06dc, 0x044d },
  { 0x06dd, 0x0449 },
  { 0x06de, 0x0447 },
  { 0x06df, 0x044a },
  { 0x06e0, 0x042e },
  { 0x06e1, 0x0410 },
  { 0x06e2, 0x0411 },
  { 0x06e3, 0x0426 },
  { 0x06e4, 0x0414 },
  { 0x06e5, 0x0415 },
  { 0x06e6, 0x0424 },
  { 0x06e7, 0x0413 },
  { 0x06e8, 0x0425 },
  { 0x06e9, 0x0418 },
  { 0x06ea, 0x0419 },
  { 0x06eb, 0x041a },
  { 0x06ec, 0x041b },
  { 0x06ed, 0x041c },
  { 0x06ee, 0x041d },
  { 0x06ef, 0x041e },
  { 0x06f0, 0x041f },
  { 0x06f1, 0x042f },
  { 0x06f2, 0x0420 },
  { 0x06f3, 0x0421 },
  { 0x06f4, 0x0422 },
  { 0x06f5, 0x0423 },
  { 0x06f6, 0x0416 },
  { 0x06f7, 0x0412 },
  { 0x06f8, 0x042c },
  { 0x06f9, 0x042b },
  { 0x06fa, 0x0417 },
  { 0x06fb, 0x0428 },
  { 0x06fc, 0x042d },
  { 0x06fd, 0x0429 },
  { 0x06fe, 0x0427 },
  { 0x06ff, 0x042a },
  { 0x07a1, 0x0386 },
  { 0x07a2, 0x0388 },
  { 0x07a3, 0x0389 },
  { 0x07a4, 0x038a },
  { 0x07a5, 0x03aa },
  { 0x07a7, 0x038c },
  { 0x07a8, 0x038e },
  { 0x07a9, 0x03ab },
  { 0x07ab, 0x038f },
  { 0x07ae, 0x0385 },
  { 0x07af, 0x2015 },
  { 0x07b1, 0x03ac },
  { 0x07b2, 0x03ad },
  { 0x07b3, 0x03ae },
  { 0x07b4, 0x03af },
  { 0x07b5, 0x03ca },
  { 0x07b6, 0x0390 },
  { 0x07b7, 0x03cc },
  { 0x07b8, 0x03cd },
  { 0x07b9, 0x03cb },
  { 0x07ba, 0x03b0 },
  { 0x07bb, 0x03ce },
  { 0x07c1, 0x0391 },
  { 0x07c2, 0x0392 },
  { 0x07c3, 0x0393 },
  { 0x07c4, 0x0394 },
  { 0x07c5, 0x0395 },
  { 0x07c6, 0x0396 },
  { 0x07c7, 0x0397 },
  { 0x07c8, 0x0398 },
  { 0x07c9, 0x0399 },
  { 0x07ca, 0x039a },
  { 0x07cb, 0x039b },
  { 0x07cc, 0x039c },
  { 0x07cd, 0x039d },
  { 0x07ce, 0x039e },
  { 0x07cf, 0x039f },
  { 0x07d0, 0x03a0 },
  { 0x07d1, 0x03a1 },
  { 0x07d2, 0x03a3 },
  { 0x07d4, 0x03a4 },
  { 0x07d5, 0x03a5 },
  { 0x07d6, 0x03a6 },
  { 0x07d7, 0x03a7 },
  { 0x07d8, 0x03a8 },
  { 0x07d9, 0x03a9 },
  { 0x07e1, 0x03b1 },
  { 0x07e2, 0x03b2 },
  { 0x07e3, 0x03b3 },
  { 0x07e4, 0x03b4 },
  { 0x07e5, 0x03b5 },
  { 0x07e6, 0x03b6 },
  { 0x07e7, 0x03b7 },
  { 0x07e8, 0x03b8 },
  { 0x07e9, 0x03b9 },
  { 0x07ea, 0x03ba },
  { 0x07eb, 0x03bb },
  { 0x07ec, 0x03bc },
  { 0x07ed, 0x03bd },
  { 0x07ee, 0x03be },
  { 0x07ef, 0x03bf },
  { 0x07f0, 0x03c0 },
  { 0x07f1, 0x03c1 },
  { 0x07f2, 0x03c3 },
  { 0x07f3, 0x03c2 },
  { 0x07f4, 0x03c4 },
  { 0x07f5, 0x03c5 },
  { 0x07f6, 0x03c6 },
  { 0x07f7, 0x03c7 },
  { 0x07f8, 0x03c8 },
  { 0x07f9, 0x03c9 },
  { 0x08a1, 0x23b7 },
  { 0x08a2, 0x250c },
  { 0x08a3, 0x2500 },
  { 0x08a4, 0x2320 },
  { 0x08a5, 0x2321 },
  { 0x08a6, 0x2502 },
  { 0x08a7, 0x23a1 },
  { 0x08a8, 0x23a3 },
  { 0x08a9, 0x23a4 },
  { 0x08aa, 0x23a6 },
  { 0x08ab, 0x239b },
  { 0x08ac, 0x239d },
  { 0x08ad, 0x239e },
  { 0x08ae, 0x23a0 },
  { 0x08af, 0x23a8 },
  { 0x08b0, 0x23ac },
  { 0x08bc, 0x2264 },
  { 0x08bd, 0x2260 },
  { 0x08be, 0x2265 },
  { 0x08bf, 0x222b },
  { 0x08c0, 0x2234 },
  { 0x08c1, 0x221d },
  { 0x08c2, 0x221e },
  { 0x08c5, 0x2207 },
  { 0x08c8, 0x223c },
  { 0x08c9, 0x2243 },
  { 0x08cd, 0x21d4 },
  { 0x08ce, 0x21d2 },
  { 0x08cf, 0x2261 },
  { 0x08d6, 0x221a },
  { 0x08da, 0x2282 },
  { 0x08db, 0x2283 },
  { 0x08dc, 0x2229 },
  { 0x08dd, 0x222a },
  { 0x08de, 0x2227 },
  { 0x08df, 0x2228 },
  { 0x08ef, 0x2202 },
  { 0x08f6, 0x0192 },
  { 0x08fb, 0x2190 },
  { 0x08fc, 0x2191 },
  { 0x08fd, 0x2192 },
  { 0x08fe, 0x2193 },
  { 0x09e0, 0x25c6 },
  { 0x09e1, 0x2592 },
  { 0x09e2, 0x2409 },
  { 0x09e3, 0x240c },
  { 0x09e4, 0x240d },
  { 0x09e5, 0x240a },
  { 0x09e8, 0x2424 },
  { 0x09e9, 0x240b },
  { 0x09ea, 0x2518 },
  { 0x09eb, 0x2510 },
  { 0x09ec, 0x250c },
  { 0x09ed, 0x2514 },
  { 0x09ee, 0x253c },
  { 0x09ef, 0x23ba },
  { 0x09f0, 0x23bb },
  { 0x09f1, 0x2500 },
  { 0x09f2, 0x23bc },
  { 0x09f3, 0x23bd },
  { 0x09f4, 0x251c },
  { 0x09f5, 0x2524 },
  { 0x09f6, 0x2534 },
  { 0x09f7, 0x252c },
  { 0x09f8, 0x2502 },
  { 0x0aa1, 0x2003 },
  { 0x0aa2, 0x2002 },
  { 0x0aa3, 0x2004 },
  { 0x0aa4, 0x2005 },
  { 0x0aa5, 0x2007 },
  { 0x0aa6, 0x2008 },
  { 0x0aa7, 0x2009 },
  { 0x0aa8, 0x200a },
  { 0x0aa9, 0x2014 },
  { 0x0aaa, 0x2013 },
  { 0x0aae, 0x2026 },
  { 0x0aaf, 0x2025 },
  { 0x0ab0, 0x2153 },
  { 0x0ab1, 0x2154 },
  { 0x0ab2, 0x2155 },
  { 0x0ab3, 0x2156 },
  { 0x0ab4, 0x2157 },
  { 0x0ab5, 0x2158 },
  { 0x0ab6, 0x2159 },
  { 0x0ab7, 0x215a },
  { 0x0ab8, 0x2105 },
  { 0x0abb, 0x2012 },
  { 0x0abc, 0x2329 },
  { 0x0abe, 0x232a },
  { 0x0ac3, 0x215b },
  { 0x0ac4, 0x215c },
  { 0x0ac5, 0x215d },
  { 0x0ac6, 0x215e },
  { 0x0ac9, 0x2122 },
  { 0x0aca, 0x2613 },
  { 0x0acc, 0x25c1 },
  { 0x0acd, 0x25b7 },
  { 0x0ace, 0x25cb },
  { 0x0acf, 0x25af },
  { 0x0ad0, 0x2018 },
  { 0x0ad1, 0x2019 },
  { 0x0ad2, 0x201c },
  { 0x0ad3, 0x201d },
  { 0x0ad4, 0x211e },
  { 0x0ad6, 0x2032 },
  { 0x0ad7, 0x2033 },
  { 0x0ad9, 0x271d },
  { 0x0adb, 0x25ac },
  { 0x0adc, 0x25c0 },
  { 0x0add, 0x25b6 },
  { 0x0ade, 0x25cf },
  { 0x0adf, 0x25ae },
  { 0x0ae0, 0x25e6 },
  { 0x0ae1, 0x25ab },
  { 0x0ae2, 0x25ad },
  { 0x0ae3, 0x25b3 },
  { 0x0ae4, 0x25bd },
  { 0x0ae5, 0x2606 },
  { 0x0ae6, 0x2022 },
  { 0x0ae7, 0x25aa },
  { 0x0ae8, 0x25b2 },
  { 0x0ae9, 0x25bc },
  { 0x0aea, 0x261c },
  { 0x0aeb, 0x261e },
  { 0x0aec, 0x2663 },
  { 0x0aed, 0x2666 },
  { 0x0aee, 0x2665 },
  { 0x0af0, 0x2720 },
  { 0x0af1, 0x2020 },
  { 0x0af2, 0x2021 },
  { 0x0af3, 0x2713 },
  { 0x0af4, 0x2717 },
  { 0x0af5, 0x266f },
  { 0x0af6, 0x266d },
  { 0x0af7, 0x2642 },
  { 0x0af8, 0x2640 },
  { 0x0af9, 0x260e },
  { 0x0afa, 0x2315 },
  { 0x0afb, 0x2117 },
  { 0x0afc, 0x2038 },
  { 0x0afd, 0x201a },
  { 0x0afe, 0x201e },
  { 0x0ba3, 0x003c },
  { 0x0ba6, 0x003e },
  { 0x0ba8, 0x2228 },
  { 0x0ba9, 0x2227 },
  { 0x0bc0, 0x00af },
  { 0x0bc2, 0x22a5 },
  { 0x0bc3, 0x2229 },
  { 0x0bc4, 0x230a },
  { 0x0bc6, 0x005f },
  { 0x0bca, 0x2218 },
  { 0x0bcc, 0x2395 },
  { 0x0bce, 0x22a4 },
  { 0x0bcf, 0x25cb },
  { 0x0bd3, 0x2308 },
  { 0x0bd6, 0x222a },
  { 0x0bd8, 0x2283 },
  { 0x0bda, 0x2282 },
  { 0x0bdc, 0x22a2 },
  { 0x0bfc, 0x22a3 },
  { 0x0cdf, 0x2017 },
  { 0x0ce0, 0x05d0 },
  { 0x0ce1, 0x05d1 },
  { 0x0ce2, 0x05d2 },
  { 0x0ce3, 0x05d3 },
  { 0x0ce4, 0x05d4 },
  { 0x0ce5, 0x05d5 },
  { 0x0ce6, 0x05d6 },
  { 0x0ce7, 0x05d7 },
  { 0x0ce8, 0x05d8 },
  { 0x0ce9, 0x05d9 },
  { 0x0cea, 0x05da },
  { 0x0ceb, 0x05db },
  { 0x0cec, 0x05dc },
  { 0x0ced, 0x05dd },
  { 0x0cee, 0x05de },
  { 0x0cef, 0x05df },
  { 0x0cf0, 0x05e0 },
  { 0x0cf1, 0x05e1 },
  { 0x0cf2, 0x05e2 },
  { 0x0cf3, 0x05e3 },
  { 0x0cf4, 0x05e4 },
  { 0x0cf5, 0x05e5 },
  { 0x0cf6, 0x05e6 },
  { 0x0cf7, 0x05e7 },
  { 0x0cf8, 0x05e8 },
  { 0x0cf9, 0x05e9 },
  { 0x0cfa, 0x05ea },
  { 0x0da1, 0x0e01 },
  { 0x0da2, 0x0e02 },
  { 0x0da3, 0x0e03 },
  { 0x0da4, 0x0e04 },
  { 0x0da5, 0x0e05 },
  { 0x0da6, 0x0e06 },
  { 0x0da7, 0x0e07 },
  { 0x0da8, 0x0e08 },
  { 0x0da9, 0x0e09 },
  { 0x0daa, 0x0e0a },
  { 0x0dab, 0x0e0b },
  { 0x0dac, 0x0e0c },
  { 0x0dad, 0x0e0d },
  { 0x0dae, 0x0e0e },
  { 0x0daf, 0x0e0f },
  { 0x0db0, 0x0e10 },
  { 0x0db1, 0x0e11 },
  { 0x0db2, 0x0e12 },
  { 0x0db3, 0x0e13 },
  { 0x0db4, 0x0e14 },
  { 0x0db5, 0x0e15 },
  { 0x0db6, 0x0e16 },
  { 0x0db7, 0x0e17 },
  { 0x0db8, 0x0e18 },
  { 0x0db9, 0x0e19 },
  { 0x0dba, 0x0e1a },
  { 0x0dbb, 0x0e1b },
  { 0x0dbc, 0x0e1c },
  { 0x0dbd, 0x0e1d },
  { 0x0dbe, 0x0e1e },
  { 0x0dbf, 0x0e1f },
  { 0x0dc0, 0x0e20 },
  { 0x0dc1, 0x0e21 },
  { 0x0dc2, 0x0e22 },
  { 0x0dc3, 0x0e23 },
  { 0x0dc4, 0x0e24 },
  { 0x0dc5, 0x0e25 },
  { 0x0dc6, 0x0e26 },
  { 0x0dc7, 0x0e27 },
  { 0x0dc8, 0x0e28 },
  { 0x0dc9, 0x0e29 },
  { 0x0dca, 0x0e2a },
  { 0x0dcb, 0x0e2b },
  { 0x0dcc, 0x0e2c },
  { 0x0dcd, 0x0e2d },
  { 0x0dce, 0x0e2e },
  { 0x0dcf, 0x0e2f },
  { 0x0dd0, 0x0e30 },
  { 0x0dd1, 0x0e31 },
  { 0x0dd2, 0x0e32 },
  { 0x0dd3, 0x0e33 },
  { 0x0dd4, 0x0e34 },
  { 0x0dd5, 0x0e35 },
  { 0x0dd6, 0x0e36 },
  { 0x0dd7, 0x0e37 },
  { 0x0dd8, 0x0e38 },
  { 0x0dd9, 0x0e39 },
  { 0x0dda, 0x0e3a },
  { 0x0ddf, 0x0e3f },
  { 0x0de0, 0x0e40 },
  { 0x0de1, 0x0e41 },
  { 0x0de2, 0x0e42 },
  { 0x0de3, 0x0e43 },
  { 0x0de4, 0x0e44 },
  { 0x0de5, 0x0e45 },
  { 0x0de6, 0x0e46 },
  { 0x0de7, 0x0e47 },
  { 0x0de8, 0x0e48 },
  { 0x0de9, 0x0e49 },
  { 0x0dea, 0x0e4a },
  { 0x0deb, 0x0e4b },
  { 0x0dec, 0x0e4c },
  { 0x0ded, 0x0e4d },
  { 0x0df0, 0x0e50 },
  { 0x0df1, 0x0e51 },
  { 0x0df2, 0x0e52 },
  { 0x0df3, 0x0e53 },
  { 0x0df4, 0x0e54 },
  { 0x0df5, 0x0e55 },
  { 0x0df6, 0x0e56 },
  { 0x0df7, 0x0e57 },
  { 0x0df8, 0x0e58 },
  { 0x0df9, 0x0e59 },
  { 0x0ea1, 0x3131 },
  { 0x0ea2, 0x3132 },
  { 0x0ea3, 0x3133 },
  { 0x0ea4, 0x3134 },
  { 0x0ea5, 0x3135 },
  { 0x0ea6, 0x3136 },
  { 0x0ea7, 0x3137 },
  { 0x0ea8, 0x3138 },
  { 0x0ea9, 0x3139 },
  { 0x0eaa, 0x313a },
  { 0x0eab, 0x313b },
  { 0x0eac, 0x313c },
  { 0x0ead, 0x313d },
  { 0x0eae, 0x313e },
  { 0x0eaf, 0x313f },
  { 0x0eb0, 0x3140 },
  { 0x0eb1, 0x3141 },
  { 0x0eb2, 0x3142 },
  { 0x0eb3, 0x3143 },
  { 0x0eb4, 0x3144 },
  { 0x0eb5, 0x3145 },
  { 0x0eb6, 0x3146 },
  { 0x0eb7, 0x3147 },
  { 0x0eb8, 0x3148 },
  { 0x0eb9, 0x3149 },
  { 0x0eba, 0x314a },
  { 0x0ebb, 0x314b },
  { 0x0ebc, 0x314c },
  { 0x0ebd, 0x314d },
  { 0x0ebe, 0x314e },
  { 0x0ebf, 0x314f },
  { 0x0ec0, 0x3150 },
  { 0x0ec1, 0x3151 },
  { 0x0ec2, 0x3152 },
  { 0x0ec3, 0x3153 },
  { 0x0ec4, 0x3154 },
  { 0x0ec5, 0x3155 },
  { 0x0ec6, 0x3156 },
  { 0x0ec7, 0x3157 },
  { 0x0ec8, 0x3158 },
  { 0x0ec9, 0x3159 },
  { 0x0eca, 0x315a },
  { 0x0ecb, 0x315b },
  { 0x0ecc, 0x315c },
  { 0x0ecd, 0x315d },
  { 0x0ece, 0x315e },
  { 0x0ecf, 0x315f },
  { 0x0ed0, 0x3160 },
  { 0x0ed1, 0x3161 },
  { 0x0ed2, 0x3162 },
  { 0x0ed3, 0x3163 },
  { 0x0ed4, 0x11a8 },
  { 0x0ed5, 0x11a9 },
  { 0x0ed6, 0x11aa },
  { 0x0ed7, 0x11ab },
  { 0x0ed8, 0x11ac },
  { 0x0ed9, 0x11ad },
  { 0x0eda, 0x11ae },
  { 0x0edb, 0x11af },
  { 0x0edc, 0x11b0 },
  { 0x0edd, 0x11b1 },
  { 0x0ede, 0x11b2 },
  { 0x0edf, 0x11b3 },
  { 0x0ee0, 0x11b4 },
  { 0x0ee1, 0x11b5 },
  { 0x0ee2, 0x11b6 },
  { 0x0ee3, 0x11b7 },
  { 0x0ee4, 0x11b8 },
  { 0x0ee5, 0x11b9 },
  { 0x0ee6, 0x11ba },
  { 0x0ee7, 0x11bb },
  { 0x0ee8, 0x11bc },
  { 0x0ee9, 0x11bd },
  { 0x0eea, 0x11be },
  { 0x0eeb, 0x11bf },
  { 0x0eec, 0x11c0 },
  { 0x0eed, 0x11c1 },
  { 0x0eee, 0x11c2 },
  { 0x0eef, 0x316d },
  { 0x0ef0, 0x3171 },
  { 0x0ef1, 0x3178 },
  { 0x0ef2, 0x317f },
  { 0x0ef3, 0x3181 },
  { 0x0ef4, 0x3184 },
  { 0x0ef5, 0x3186 },
  { 0x0ef6, 0x318d },
  { 0x0ef7, 0x318e },
  { 0x0ef8, 0x11eb },
  { 0x0ef9, 0x11f0 },
  { 0x0efa, 0x11f9 },
  { 0x0eff, 0x20a9 },
  { 0x13a4, 0x20ac },
  { 0x13bc, 0x0152 },
  { 0x13bd, 0x0153 },
  { 0x13be, 0x0178 },
  { 0x20ac, 0x20ac },
  { 0xfe50,    '`' },
  { 0xfe51, 0x00b4 },
  { 0xfe52,    '^' },
  { 0xfe53,    '~' },
  { 0xfe54, 0x00af },
  { 0xfe55, 0x02d8 },
  { 0xfe56, 0x02d9 },
  { 0xfe57, 0x00a8 },
  { 0xfe58, 0x02da },
  { 0xfe59, 0x02dd },
  { 0xfe5a, 0x02c7 },
  { 0xfe5b, 0x00b8 },
  { 0xfe5c, 0x02db },
  { 0xfe5d, 0x037a },
  { 0xfe5e, 0x309b },
  { 0xfe5f, 0x309c },
  { 0xfe63,    '/' },
  { 0xfe64, 0x02bc },
  { 0xfe65, 0x02bd },
  { 0xfe66, 0x02f5 },
  { 0xfe67, 0x02f3 },
  { 0xfe68, 0x02cd },
  { 0xfe69, 0xa788 },
  { 0xfe6a, 0x02f7 },
  { 0xfe6e,    ',' },
  { 0xfe6f, 0x00a4 },
  { 0xfe80,    'a' }, /* XK_dead_a */
  { 0xfe81,    'A' }, /* XK_dead_A */
  { 0xfe82,    'e' }, /* XK_dead_e */
  { 0xfe83,    'E' }, /* XK_dead_E */
  { 0xfe84,    'i' }, /* XK_dead_i */
  { 0xfe85,    'I' }, /* XK_dead_I */
  { 0xfe86,    'o' }, /* XK_dead_o */
  { 0xfe87,    'O' }, /* XK_dead_O */
  { 0xfe88,    'u' }, /* XK_dead_u */
  { 0xfe89,    'U' }, /* XK_dead_U */
  { 0xfe8a, 0x0259 },
  { 0xfe8b, 0x018f },
  { 0xfe8c, 0x00b5 },
  { 0xfe90,    '_' },
  { 0xfe91, 0x02c8 },
  { 0xfe92, 0x02cc },
  { 0xff80 /*XKB_KEY_KP_Space*/,     ' ' },
  { 0xff95 /*XKB_KEY_KP_7*/, 0x0037 },
  { 0xff96 /*XKB_KEY_KP_4*/, 0x0034 },
  { 0xff97 /*XKB_KEY_KP_8*/, 0x0038 },
  { 0xff98 /*XKB_KEY_KP_6*/, 0x0036 },
  { 0xff99 /*XKB_KEY_KP_2*/, 0x0032 },
  { 0xff9a /*XKB_KEY_KP_9*/, 0x0039 },
  { 0xff9b /*XKB_KEY_KP_3*/, 0x0033 },
  { 0xff9c /*XKB_KEY_KP_1*/, 0x0031 },
  { 0xff9d /*XKB_KEY_KP_5*/, 0x0035 },
  { 0xff9e /*XKB_KEY_KP_0*/, 0x0030 },
  { 0xffaa /*XKB_KEY_KP_Multiply*/,  '*' },
  { 0xffab /*XKB_KEY_KP_Add*/,       '+' },
  { 0xffac /*XKB_KEY_KP_Separator*/, ',' },
  { 0xffad /*XKB_KEY_KP_Subtract*/,  '-' },
  { 0xffae /*XKB_KEY_KP_Decimal*/,   '.' },
  { 0xffaf /*XKB_KEY_KP_Divide*/,    '/' },
  { 0xffb0 /*XKB_KEY_KP_0*/, 0x0030 },
  { 0xffb1 /*XKB_KEY_KP_1*/, 0x0031 },
  { 0xffb2 /*XKB_KEY_KP_2*/, 0x0032 },
  { 0xffb3 /*XKB_KEY_KP_3*/, 0x0033 },
  { 0xffb4 /*XKB_KEY_KP_4*/, 0x0034 },
  { 0xffb5 /*XKB_KEY_KP_5*/, 0x0035 },
  { 0xffb6 /*XKB_KEY_KP_6*/, 0x0036 },
  { 0xffb7 /*XKB_KEY_KP_7*/, 0x0037 },
  { 0xffb8 /*XKB_KEY_KP_8*/, 0x0038 },
  { 0xffb9 /*XKB_KEY_KP_9*/, 0x0039 },
  { 0xffbd /*XKB_KEY_KP_Equal*/,     '=' }
};

_SOKOL_PRIVATE int _sapp_x11_error_handler(Display* display, XErrorEvent* event) {
    _SOKOL_UNUSED(display);
    _sapp_x11_error_code = event->error_code;
    return 0;
}

_SOKOL_PRIVATE void _sapp_x11_grab_error_handler(void) {
    _sapp_x11_error_code = Success;
    XSetErrorHandler(_sapp_x11_error_handler);
}

_SOKOL_PRIVATE void _sapp_x11_release_error_handler(void) {
    XSync(_sapp_x11_display, False);
    XSetErrorHandler(NULL);
}

_SOKOL_PRIVATE void _sapp_x11_init_extensions(void) {
    _sapp_x11_UTF8_STRING       = XInternAtom(_sapp_x11_display, "UTF8_STRING", False);
    _sapp_x11_WM_PROTOCOLS      = XInternAtom(_sapp_x11_display, "WM_PROTOCOLS", False);
    _sapp_x11_WM_DELETE_WINDOW  = XInternAtom(_sapp_x11_display, "WM_DELETE_WINDOW", False);
    _sapp_x11_WM_STATE          = XInternAtom(_sapp_x11_display, "WM_STATE", False);
    _sapp_x11_NET_WM_NAME    = XInternAtom(_sapp_x11_display, "_NET_WM_NAME", False);
    _sapp_x11_NET_WM_ICON_NAME = XInternAtom(_sapp_x11_display, "_NET_WM_ICON_NAME", False);
}

_SOKOL_PRIVATE void _sapp_x11_query_system_dpi(void) {
    /* from GLFW:

       NOTE: Default to the display-wide DPI as we don't currently have a policy
             for which monitor a window is considered to be on
    _sapp_x11_dpi = DisplayWidth(_sapp_x11_display, _sapp_x11_screen) *
        25.4f / DisplayWidthMM(_sapp_x11_display, _sapp_x11_screen);

       NOTE: Basing the scale on Xft.dpi where available should provide the most
             consistent user experience (matches Qt, Gtk, etc), although not
             always the most accurate one
    */
    char* rms = XResourceManagerString(_sapp_x11_display);
    if (rms) {
        XrmDatabase db = XrmGetStringDatabase(rms);
        if (db) {
            XrmValue value;
            char* type = NULL;
            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value)) {
                if (type && strcmp(type, "String") == 0) {
                    _sapp_x11_dpi = atof(value.addr);
                }
            }
            XrmDestroyDatabase(db);
        }
    }
}

_SOKOL_PRIVATE bool _sapp_glx_has_ext(const char* ext, const char* extensions) {
    SOKOL_ASSERT(ext);
    const char* start = extensions;
    while (true) {
        const char* where = strstr(start, ext);
        if (!where) {
            return false;
        }
        const char* terminator = where + strlen(ext);
        if ((where == start) || (*(where - 1) == ' ')) {
            if (*terminator == ' ' || *terminator == '\0') {
                break;
            }
        }
        start = terminator;
    }
    return true;
}

_SOKOL_PRIVATE bool _sapp_glx_extsupported(const char* ext, const char* extensions) {
    if (extensions) {
        return _sapp_glx_has_ext(ext, extensions);
    }
    else {
        return false;
    }
}

_SOKOL_PRIVATE void* _sapp_glx_getprocaddr(const char* procname)
{
    if (_sapp_glx_GetProcAddress) {
        return (void*) _sapp_glx_GetProcAddress((const GLubyte*) procname);
    }
    else if (_sapp_glx_GetProcAddressARB) {
        return (void*) _sapp_glx_GetProcAddressARB((const GLubyte*) procname);
    }
    else {
        return dlsym(_sapp_glx_libgl, procname);
    }
}

_SOKOL_PRIVATE void _sapp_glx_init() {
    const char* sonames[] = { "libGL.so.1", "libGL.so", 0 };
    for (int i = 0; sonames[i]; i++) {
        _sapp_glx_libgl = dlopen(sonames[i], RTLD_LAZY|RTLD_GLOBAL);
        if (_sapp_glx_libgl) {
            break;
        }
    }
    if (!_sapp_glx_libgl) {
        _sapp_fail("GLX: failed to load libGL");
    }
    _sapp_glx_GetFBConfigs          = (PFNGLXGETFBCONFIGSPROC)          dlsym(_sapp_glx_libgl, "glXGetFBConfigs");
    _sapp_glx_GetFBConfigAttrib     = (PFNGLXGETFBCONFIGATTRIBPROC)     dlsym(_sapp_glx_libgl, "glXGetFBConfigAttrib");
    _sapp_glx_GetClientString       = (PFNGLXGETCLIENTSTRINGPROC)       dlsym(_sapp_glx_libgl, "glXGetClientString");
    _sapp_glx_QueryExtension        = (PFNGLXQUERYEXTENSIONPROC)        dlsym(_sapp_glx_libgl, "glXQueryExtension");
    _sapp_glx_QueryVersion          = (PFNGLXQUERYVERSIONPROC)          dlsym(_sapp_glx_libgl, "glXQueryVersion");
    _sapp_glx_DestroyContext        = (PFNGLXDESTROYCONTEXTPROC)        dlsym(_sapp_glx_libgl, "glXDestroyContext");
    _sapp_glx_MakeCurrent           = (PFNGLXMAKECURRENTPROC)           dlsym(_sapp_glx_libgl, "glXMakeCurrent");
    _sapp_glx_SwapBuffers           = (PFNGLXSWAPBUFFERSPROC)           dlsym(_sapp_glx_libgl, "glXSwapBuffers");
    _sapp_glx_QueryExtensionsString = (PFNGLXQUERYEXTENSIONSSTRINGPROC) dlsym(_sapp_glx_libgl, "glXQueryExtensionsString");
    _sapp_glx_CreateNewContext      = (PFNGLXCREATENEWCONTEXTPROC)      dlsym(_sapp_glx_libgl, "glXCreateNewContext");
    _sapp_glx_CreateWindow          = (PFNGLXCREATEWINDOWPROC)          dlsym(_sapp_glx_libgl, "glXCreateWindow");
    _sapp_glx_DestroyWindow         = (PFNGLXDESTROYWINDOWPROC)         dlsym(_sapp_glx_libgl, "glXDestroyWindow");
    _sapp_glx_GetProcAddress        = (PFNGLXGETPROCADDRESSPROC)        dlsym(_sapp_glx_libgl, "glXGetProcAddress");
    _sapp_glx_GetProcAddressARB     = (PFNGLXGETPROCADDRESSPROC)        dlsym(_sapp_glx_libgl, "glXGetProcAddressARB");
    _sapp_glx_GetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC) dlsym(_sapp_glx_libgl, "glXGetVisualFromFBConfig");
    if (!_sapp_glx_GetFBConfigs ||
        !_sapp_glx_GetFBConfigAttrib ||
        !_sapp_glx_GetClientString ||
        !_sapp_glx_QueryExtension ||
        !_sapp_glx_QueryVersion ||
        !_sapp_glx_DestroyContext ||
        !_sapp_glx_MakeCurrent ||
        !_sapp_glx_SwapBuffers ||
        !_sapp_glx_QueryExtensionsString ||
        !_sapp_glx_CreateNewContext ||
        !_sapp_glx_CreateWindow ||
        !_sapp_glx_DestroyWindow ||
        !_sapp_glx_GetProcAddress ||
        !_sapp_glx_GetProcAddressARB ||
        !_sapp_glx_GetVisualFromFBConfig)
    {
        _sapp_fail("GLX: failed to load required entry points");
    }

    if (!_sapp_glx_QueryExtension(_sapp_x11_display,
                           &_sapp_glx_errorbase,
                           &_sapp_glx_eventbase))
    {
        _sapp_fail("GLX: GLX extension not found");
    }
    if (!_sapp_glx_QueryVersion(_sapp_x11_display, &_sapp_glx_major, &_sapp_glx_minor)) {
        _sapp_fail("GLX: Failed to query GLX version");
    }
    if (_sapp_glx_major == 1 && _sapp_glx_minor < 3) {
        _sapp_fail("GLX: GLX version 1.3 is required");
    }
    const char* exts = _sapp_glx_QueryExtensionsString(_sapp_x11_display, _sapp_x11_screen);
    if (_sapp_glx_extsupported("GLX_EXT_swap_control", exts)) {
        _sapp_glx_SwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) _sapp_glx_getprocaddr("glXSwapIntervalEXT");
        _sapp_glx_EXT_swap_control = 0 != _sapp_glx_SwapIntervalEXT;
    }
    if (_sapp_glx_extsupported("GLX_MESA_swap_control", exts)) {
        _sapp_glx_SwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC) _sapp_glx_getprocaddr("glXSwapIntervalMESA");
        _sapp_glx_MESA_swap_control = 0 != _sapp_glx_SwapIntervalMESA;
    }
    _sapp_glx_ARB_multisample = _sapp_glx_extsupported("GLX_ARB_multisample", exts);
    _sapp_glx_ARB_framebuffer_sRGB = _sapp_glx_extsupported("GLX_ARB_framebuffer_sRGB", exts);
    _sapp_glx_EXT_framebuffer_sRGB = _sapp_glx_extsupported("GLX_EXT_framebuffer_sRGB", exts);
    if (_sapp_glx_extsupported("GLX_ARB_create_context", exts)) {
        _sapp_glx_CreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) _sapp_glx_getprocaddr("glXCreateContextAttribsARB");
        _sapp_glx_ARB_create_context = 0 != _sapp_glx_CreateContextAttribsARB;
    }
    _sapp_glx_ARB_create_context_profile = _sapp_glx_extsupported("GLX_ARB_create_context_profile", exts);
}

_SOKOL_PRIVATE int _sapp_glx_attrib(GLXFBConfig fbconfig, int attrib) {
    int value;
    _sapp_glx_GetFBConfigAttrib(_sapp_x11_display, fbconfig, attrib, &value);
    return value;
}

_SOKOL_PRIVATE GLXFBConfig _sapp_glx_choosefbconfig() {
    GLXFBConfig* native_configs;
    _sapp_gl_fbconfig* usable_configs;
    const _sapp_gl_fbconfig* closest;
    int i, native_count, usable_count;
    const char* vendor;
    bool trust_window_bit = true;

    /* HACK: This is a (hopefully temporary) workaround for Chromium
           (VirtualBox GL) not setting the window bit on any GLXFBConfigs
    */
    vendor = _sapp_glx_GetClientString(_sapp_x11_display, GLX_VENDOR);
    if (vendor && strcmp(vendor, "Chromium") == 0) {
        trust_window_bit = false;
    }

    native_configs = _sapp_glx_GetFBConfigs(_sapp_x11_display, _sapp_x11_screen, &native_count);
    if (!native_configs || !native_count) {
        _sapp_fail("GLX: No GLXFBConfigs returned");
    }

    usable_configs = (_sapp_gl_fbconfig*) SOKOL_CALLOC(native_count, sizeof(_sapp_gl_fbconfig));
    usable_count = 0;
    for (i = 0;  i < native_count;  i++) {
        const GLXFBConfig n = native_configs[i];
        _sapp_gl_fbconfig* u = usable_configs + usable_count;
        _sapp_gl_init_fbconfig(u);

        /* Only consider RGBA GLXFBConfigs */
        if (0 == (_sapp_glx_attrib(n, GLX_RENDER_TYPE) & GLX_RGBA_BIT)) {
            continue;
        }
        /* Only consider window GLXFBConfigs */
        if (0 == (_sapp_glx_attrib(n, GLX_DRAWABLE_TYPE) & GLX_WINDOW_BIT)) {
            if (trust_window_bit) {
                continue;
            }
        }
        u->red_bits = _sapp_glx_attrib(n, GLX_RED_SIZE);
        u->green_bits = _sapp_glx_attrib(n, GLX_GREEN_SIZE);
        u->blue_bits = _sapp_glx_attrib(n, GLX_BLUE_SIZE);
        u->alpha_bits = _sapp_glx_attrib(n, GLX_ALPHA_SIZE);
        u->depth_bits = _sapp_glx_attrib(n, GLX_DEPTH_SIZE);
        u->stencil_bits = _sapp_glx_attrib(n, GLX_STENCIL_SIZE);
        if (_sapp_glx_attrib(n, GLX_DOUBLEBUFFER)) {
            u->doublebuffer = true;
        }
        if (_sapp_glx_ARB_multisample) {
            u->samples = _sapp_glx_attrib(n, GLX_SAMPLES);
        }
        u->handle = (uintptr_t) n;
        usable_count++;
    }
    _sapp_gl_fbconfig desired;
    _sapp_gl_init_fbconfig(&desired);
    desired.red_bits = 8;
    desired.green_bits = 8;
    desired.blue_bits = 8;
    desired.alpha_bits = 8;
    desired.depth_bits = 24;
    desired.stencil_bits = 8;
    desired.doublebuffer = true;
    desired.samples = _sapp.sample_count > 1 ? _sapp.sample_count : 0;
    closest = _sapp_gl_choose_fbconfig(&desired, usable_configs, usable_count);
    GLXFBConfig result = 0;
    if (closest) {
        result = (GLXFBConfig) closest->handle;
    }
    XFree(native_configs);
    SOKOL_FREE(usable_configs);
    return result;
}

_SOKOL_PRIVATE void _sapp_glx_choose_visual(Visual** visual, int* depth) {
    GLXFBConfig native = _sapp_glx_choosefbconfig();
    if (0 == native) {
        _sapp_fail("GLX: Failed to find a suitable GLXFBConfig");
    }
    XVisualInfo* result = _sapp_glx_GetVisualFromFBConfig(_sapp_x11_display, native);
    if (!result) {
        _sapp_fail("GLX: Failed to retrieve Visual for GLXFBConfig");
    }
    *visual = result->visual;
    *depth = result->depth;
    XFree(result);
}

_SOKOL_PRIVATE void _sapp_glx_create_context(void) {
    GLXFBConfig native = _sapp_glx_choosefbconfig();
    if (0 == native){
        _sapp_fail("GLX: Failed to find a suitable GLXFBConfig (2)");
    }
    if (!(_sapp_glx_ARB_create_context && _sapp_glx_ARB_create_context_profile)) {
        _sapp_fail("GLX: ARB_create_context and ARB_create_context_profile required");
    }
    _sapp_x11_grab_error_handler();
    const int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0, 0
    };
    _sapp_glx_ctx = _sapp_glx_CreateContextAttribsARB(_sapp_x11_display, native, NULL, True, attribs);
    if (!_sapp_glx_ctx) {
        _sapp_fail("GLX: failed to create GL context");
    }
    _sapp_x11_release_error_handler();
    _sapp_glx_window = _sapp_glx_CreateWindow(_sapp_x11_display, native, _sapp_x11_window, NULL);
    if (!_sapp_glx_window) {
        _sapp_fail("GLX: failed to create window");
    }
}

_SOKOL_PRIVATE void _sapp_glx_destroy_context(void) {
    if (_sapp_glx_window) {
        _sapp_glx_DestroyWindow(_sapp_x11_display, _sapp_glx_window);
        _sapp_glx_window = 0;
    }
    if (_sapp_glx_ctx) {
        _sapp_glx_DestroyContext(_sapp_x11_display, _sapp_glx_ctx);
        _sapp_glx_ctx = 0;
    }
}

_SOKOL_PRIVATE void _sapp_glx_make_current(void) {
    _sapp_glx_MakeCurrent(_sapp_x11_display, _sapp_glx_window, _sapp_glx_ctx);
}

_SOKOL_PRIVATE void _sapp_glx_swap_buffers(void) {
    _sapp_glx_SwapBuffers(_sapp_x11_display, _sapp_glx_window);
}

_SOKOL_PRIVATE void _sapp_glx_swapinterval(int interval) {
    _sapp_glx_make_current();
    if (_sapp_glx_EXT_swap_control) {
        _sapp_glx_SwapIntervalEXT(_sapp_x11_display, _sapp_glx_window, interval);
    }
    else if (_sapp_glx_MESA_swap_control) {
        _sapp_glx_SwapIntervalMESA(interval);
    }
}

_SOKOL_PRIVATE void _sapp_x11_update_window_title(void) {
    Xutf8SetWMProperties(_sapp_x11_display,
        _sapp_x11_window,
        _sapp.window_title, _sapp.window_title,
        NULL, 0, NULL, NULL, NULL);
    XChangeProperty(_sapp_x11_display, _sapp_x11_window,
        _sapp_x11_NET_WM_NAME, _sapp_x11_UTF8_STRING, 8,
        PropModeReplace,
        (unsigned char*)_sapp.window_title,
        strlen(_sapp.window_title));
    XChangeProperty(_sapp_x11_display, _sapp_x11_window,
        _sapp_x11_NET_WM_ICON_NAME, _sapp_x11_UTF8_STRING, 8,
        PropModeReplace,
        (unsigned char*)_sapp.window_title,
        strlen(_sapp.window_title));
    XFlush(_sapp_x11_display);
}

_SOKOL_PRIVATE void _sapp_x11_query_window_size(void) {
    XWindowAttributes attribs;
    XGetWindowAttributes(_sapp_x11_display, _sapp_x11_window, &attribs);
    _sapp.window_width = attribs.width;
    _sapp.window_height = attribs.height;
    _sapp.framebuffer_width = _sapp.window_width;
    _sapp.framebuffer_height = _sapp.framebuffer_height;
}

_SOKOL_PRIVATE void _sapp_x11_create_window(Visual* visual, int depth) {
    _sapp_x11_colormap = XCreateColormap(_sapp_x11_display, _sapp_x11_root, visual, AllocNone);
    XSetWindowAttributes wa;
    memset(&wa, 0, sizeof(wa));
    const uint32_t wamask = CWBorderPixel | CWColormap | CWEventMask;
    wa.colormap = _sapp_x11_colormap;
    wa.border_pixel = 0;
    wa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                    PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                    ExposureMask | FocusChangeMask | VisibilityChangeMask |
                    EnterWindowMask | LeaveWindowMask | PropertyChangeMask;
    _sapp_x11_grab_error_handler();
    _sapp_x11_window = XCreateWindow(_sapp_x11_display,
                                     _sapp_x11_root,
                                     0, 0,
                                     _sapp.window_width,
                                     _sapp.window_height,
                                     0,     /* border width */
                                     depth, /* color depth */
                                     InputOutput,
                                     visual,
                                     wamask,
                                     &wa);
    _sapp_x11_release_error_handler();
    if (!_sapp_x11_window) {
        _sapp_fail("X11: Failed to create window");
    }

    Atom protocols[] = {
        _sapp_x11_WM_DELETE_WINDOW
    };
    XSetWMProtocols(_sapp_x11_display, _sapp_x11_window, protocols, 1);

    XSizeHints* hints = XAllocSizeHints();
    hints->flags |= PWinGravity;
    hints->win_gravity = StaticGravity;
    XSetWMNormalHints(_sapp_x11_display, _sapp_x11_window, hints);
    XFree(hints);

    _sapp_x11_update_window_title();
    _sapp_x11_query_window_size();
}

_SOKOL_PRIVATE void _sapp_x11_destroy_window(void) {
    if (_sapp_x11_window) {
        XUnmapWindow(_sapp_x11_display, _sapp_x11_window);
        XDestroyWindow(_sapp_x11_display, _sapp_x11_window);
        _sapp_x11_window = 0;
    }
    if (_sapp_x11_colormap) {
        XFreeColormap(_sapp_x11_display, _sapp_x11_colormap);
        _sapp_x11_colormap = 0;
    }
    XFlush(_sapp_x11_display);
}

_SOKOL_PRIVATE bool _sapp_x11_window_visible(void) {
    XWindowAttributes wa;
    XGetWindowAttributes(_sapp_x11_display, _sapp_x11_window, &wa);
    return wa.map_state == IsViewable;
}

_SOKOL_PRIVATE void _sapp_x11_show_window(void) {
    if (!_sapp_x11_window_visible()) {
        XMapWindow(_sapp_x11_display, _sapp_x11_window);
        XRaiseWindow(_sapp_x11_display, _sapp_x11_window);
        XFlush(_sapp_x11_display);
    }
}

_SOKOL_PRIVATE void _sapp_x11_hide_window(void) {
    XUnmapWindow(_sapp_x11_display, _sapp_x11_window);
    XFlush(_sapp_x11_display);
}

_SOKOL_PRIVATE unsigned long _sapp_x11_get_window_property(Atom property, Atom type, unsigned char** value) {
    Atom actualType;
    int actualFormat;
    unsigned long itemCount, bytesAfter;
    XGetWindowProperty(_sapp_x11_display,
                       _sapp_x11_window,
                       property,
                       0,
                       LONG_MAX,
                       False,
                       type,
                       &actualType,
                       &actualFormat,
                       &itemCount,
                       &bytesAfter,
                       value);
    return itemCount;
}

_SOKOL_PRIVATE int _sapp_x11_get_window_state(void) {
    int result = WithdrawnState;
    struct {
        CARD32 state;
        Window icon;
    } *state = NULL;

    if (_sapp_x11_get_window_property(_sapp_x11_WM_STATE, _sapp_x11_WM_STATE, (unsigned char**)&state) >= 2) {
        result = state->state;
    }
    if (state) {
        XFree(state);
    }
    return result;
}

_SOKOL_PRIVATE uint32_t _sapp_x11_mod(int x11_mods) {
    uint32_t mods = 0;
    if (x11_mods & ShiftMask) {
        mods |= SAPP_MODIFIER_SHIFT;
    }
    if (x11_mods & ControlMask) {
        mods |= SAPP_MODIFIER_CTRL;
    }
    if (x11_mods & Mod1Mask) {
        mods |= SAPP_MODIFIER_ALT;
    }
    if (x11_mods & Mod4Mask) {
        mods |= SAPP_MODIFIER_SUPER;
    }
    return mods;
}

_SOKOL_PRIVATE void _sapp_x11_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE sapp_mousebutton _sapp_x11_translate_button(const XEvent* event) {
    switch (event->xbutton.button) {
        case Button1: return SAPP_MOUSEBUTTON_LEFT;
        case Button2: return SAPP_MOUSEBUTTON_MIDDLE;
        case Button3: return SAPP_MOUSEBUTTON_RIGHT;
        default:      return SAPP_MOUSEBUTTON_INVALID;
    }
}

_SOKOL_PRIVATE void _sapp_x11_mouse_event(sapp_event_type type, sapp_mousebutton btn, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.mouse_button = btn;
        _sapp.event.modifiers = mods;
        _sapp.event.mouse_x = _sapp.mouse_x;
        _sapp.event.mouse_y = _sapp.mouse_y;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_x11_scroll_event(float x, float y, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
        _sapp.event.modifiers = mods;
        _sapp.event.scroll_x = x;
        _sapp.event.scroll_y = y;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_x11_key_event(sapp_event_type type, sapp_keycode key, bool repeat, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.key_code = key;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = mods;
        _sapp_call_event(&_sapp.event);
        /* check if a CLIPBOARD_PASTED event must be sent too */
        if (_sapp.clipboard_enabled &&
            (type == SAPP_EVENTTYPE_KEY_DOWN) &&
            (_sapp.event.modifiers == SAPP_MODIFIER_CTRL) &&
            (_sapp.event.key_code == SAPP_KEYCODE_V))
        {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}

_SOKOL_PRIVATE void _sapp_x11_char_event(uint32_t chr, bool repeat, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
        _sapp.event.char_code = chr;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = mods;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE sapp_keycode _sapp_x11_translate_key(int scancode) {
    int dummy;
    KeySym* keysyms = XGetKeyboardMapping(_sapp_x11_display, scancode, 1, &dummy);
    SOKOL_ASSERT(keysyms);
    KeySym keysym = keysyms[0];
    XFree(keysyms);
    switch (keysym) {
        case XK_Escape:         return SAPP_KEYCODE_ESCAPE;
        case XK_Tab:            return SAPP_KEYCODE_TAB;
        case XK_Shift_L:        return SAPP_KEYCODE_LEFT_SHIFT;
        case XK_Shift_R:        return SAPP_KEYCODE_RIGHT_SHIFT;
        case XK_Control_L:      return SAPP_KEYCODE_LEFT_CONTROL;
        case XK_Control_R:      return SAPP_KEYCODE_RIGHT_CONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return SAPP_KEYCODE_LEFT_ALT;
        case XK_Mode_switch:    /* Mapped to Alt_R on many keyboards */
        case XK_ISO_Level3_Shift: /* AltGr on at least some machines */
        case XK_Meta_R:
        case XK_Alt_R:          return SAPP_KEYCODE_RIGHT_ALT;
        case XK_Super_L:        return SAPP_KEYCODE_LEFT_SUPER;
        case XK_Super_R:        return SAPP_KEYCODE_RIGHT_SUPER;
        case XK_Menu:           return SAPP_KEYCODE_MENU;
        case XK_Num_Lock:       return SAPP_KEYCODE_NUM_LOCK;
        case XK_Caps_Lock:      return SAPP_KEYCODE_CAPS_LOCK;
        case XK_Print:          return SAPP_KEYCODE_PRINT_SCREEN;
        case XK_Scroll_Lock:    return SAPP_KEYCODE_SCROLL_LOCK;
        case XK_Pause:          return SAPP_KEYCODE_PAUSE;
        case XK_Delete:         return SAPP_KEYCODE_DELETE;
        case XK_BackSpace:      return SAPP_KEYCODE_BACKSPACE;
        case XK_Return:         return SAPP_KEYCODE_ENTER;
        case XK_Home:           return SAPP_KEYCODE_HOME;
        case XK_End:            return SAPP_KEYCODE_END;
        case XK_Page_Up:        return SAPP_KEYCODE_PAGE_UP;
        case XK_Page_Down:      return SAPP_KEYCODE_PAGE_DOWN;
        case XK_Insert:         return SAPP_KEYCODE_INSERT;
        case XK_Left:           return SAPP_KEYCODE_LEFT;
        case XK_Right:          return SAPP_KEYCODE_RIGHT;
        case XK_Down:           return SAPP_KEYCODE_DOWN;
        case XK_Up:             return SAPP_KEYCODE_UP;
        case XK_F1:             return SAPP_KEYCODE_F1;
        case XK_F2:             return SAPP_KEYCODE_F2;
        case XK_F3:             return SAPP_KEYCODE_F3;
        case XK_F4:             return SAPP_KEYCODE_F4;
        case XK_F5:             return SAPP_KEYCODE_F5;
        case XK_F6:             return SAPP_KEYCODE_F6;
        case XK_F7:             return SAPP_KEYCODE_F7;
        case XK_F8:             return SAPP_KEYCODE_F8;
        case XK_F9:             return SAPP_KEYCODE_F9;
        case XK_F10:            return SAPP_KEYCODE_F10;
        case XK_F11:            return SAPP_KEYCODE_F11;
        case XK_F12:            return SAPP_KEYCODE_F12;
        case XK_F13:            return SAPP_KEYCODE_F13;
        case XK_F14:            return SAPP_KEYCODE_F14;
        case XK_F15:            return SAPP_KEYCODE_F15;
        case XK_F16:            return SAPP_KEYCODE_F16;
        case XK_F17:            return SAPP_KEYCODE_F17;
        case XK_F18:            return SAPP_KEYCODE_F18;
        case XK_F19:            return SAPP_KEYCODE_F19;
        case XK_F20:            return SAPP_KEYCODE_F20;
        case XK_F21:            return SAPP_KEYCODE_F21;
        case XK_F22:            return SAPP_KEYCODE_F22;
        case XK_F23:            return SAPP_KEYCODE_F23;
        case XK_F24:            return SAPP_KEYCODE_F24;
        case XK_F25:            return SAPP_KEYCODE_F25;

        case XK_KP_Divide:      return SAPP_KEYCODE_KP_DIVIDE;
        case XK_KP_Multiply:    return SAPP_KEYCODE_KP_MULTIPLY;
        case XK_KP_Subtract:    return SAPP_KEYCODE_KP_SUBTRACT;
        case XK_KP_Add:         return SAPP_KEYCODE_KP_ADD;

        case XK_KP_Insert:      return SAPP_KEYCODE_KP_0;
        case XK_KP_End:         return SAPP_KEYCODE_KP_1;
        case XK_KP_Down:        return SAPP_KEYCODE_KP_2;
        case XK_KP_Page_Down:   return SAPP_KEYCODE_KP_3;
        case XK_KP_Left:        return SAPP_KEYCODE_KP_4;
        case XK_KP_Begin:       return SAPP_KEYCODE_KP_5;
        case XK_KP_Right:       return SAPP_KEYCODE_KP_6;
        case XK_KP_Home:        return SAPP_KEYCODE_KP_7;
        case XK_KP_Up:          return SAPP_KEYCODE_KP_8;
        case XK_KP_Page_Up:     return SAPP_KEYCODE_KP_9;
        case XK_KP_Delete:      return SAPP_KEYCODE_KP_DECIMAL;
        case XK_KP_Equal:       return SAPP_KEYCODE_KP_EQUAL;
        case XK_KP_Enter:       return SAPP_KEYCODE_KP_ENTER;

        case XK_a:              return SAPP_KEYCODE_A;
        case XK_b:              return SAPP_KEYCODE_B;
        case XK_c:              return SAPP_KEYCODE_C;
        case XK_d:              return SAPP_KEYCODE_D;
        case XK_e:              return SAPP_KEYCODE_E;
        case XK_f:              return SAPP_KEYCODE_F;
        case XK_g:              return SAPP_KEYCODE_G;
        case XK_h:              return SAPP_KEYCODE_H;
        case XK_i:              return SAPP_KEYCODE_I;
        case XK_j:              return SAPP_KEYCODE_J;
        case XK_k:              return SAPP_KEYCODE_K;
        case XK_l:              return SAPP_KEYCODE_L;
        case XK_m:              return SAPP_KEYCODE_M;
        case XK_n:              return SAPP_KEYCODE_N;
        case XK_o:              return SAPP_KEYCODE_O;
        case XK_p:              return SAPP_KEYCODE_P;
        case XK_q:              return SAPP_KEYCODE_Q;
        case XK_r:              return SAPP_KEYCODE_R;
        case XK_s:              return SAPP_KEYCODE_S;
        case XK_t:              return SAPP_KEYCODE_T;
        case XK_u:              return SAPP_KEYCODE_U;
        case XK_v:              return SAPP_KEYCODE_V;
        case XK_w:              return SAPP_KEYCODE_W;
        case XK_x:              return SAPP_KEYCODE_X;
        case XK_y:              return SAPP_KEYCODE_Y;
        case XK_z:              return SAPP_KEYCODE_Z;
        case XK_1:              return SAPP_KEYCODE_1;
        case XK_2:              return SAPP_KEYCODE_2;
        case XK_3:              return SAPP_KEYCODE_3;
        case XK_4:              return SAPP_KEYCODE_4;
        case XK_5:              return SAPP_KEYCODE_5;
        case XK_6:              return SAPP_KEYCODE_6;
        case XK_7:              return SAPP_KEYCODE_7;
        case XK_8:              return SAPP_KEYCODE_8;
        case XK_9:              return SAPP_KEYCODE_9;
        case XK_0:              return SAPP_KEYCODE_0;
        case XK_space:          return SAPP_KEYCODE_SPACE;
        case XK_minus:          return SAPP_KEYCODE_MINUS;
        case XK_equal:          return SAPP_KEYCODE_EQUAL;
        case XK_bracketleft:    return SAPP_KEYCODE_LEFT_BRACKET;
        case XK_bracketright:   return SAPP_KEYCODE_RIGHT_BRACKET;
        case XK_backslash:      return SAPP_KEYCODE_BACKSLASH;
        case XK_semicolon:      return SAPP_KEYCODE_SEMICOLON;
        case XK_apostrophe:     return SAPP_KEYCODE_APOSTROPHE;
        case XK_grave:          return SAPP_KEYCODE_GRAVE_ACCENT;
        case XK_comma:          return SAPP_KEYCODE_COMMA;
        case XK_period:         return SAPP_KEYCODE_PERIOD;
        case XK_slash:          return SAPP_KEYCODE_SLASH;
        case XK_less:           return SAPP_KEYCODE_WORLD_1; /* At least in some layouts... */
        default:                return SAPP_KEYCODE_INVALID;
    }
}

_SOKOL_PRIVATE int32_t _sapp_x11_keysym_to_unicode(KeySym keysym) {
    int min = 0;
    int max = sizeof(_sapp_x11_keysymtab) / sizeof(struct _sapp_x11_codepair) - 1;
    int mid;

    /* First check for Latin-1 characters (1:1 mapping) */
    if ((keysym >= 0x0020 && keysym <= 0x007e) ||
        (keysym >= 0x00a0 && keysym <= 0x00ff))
    {
        return keysym;
    }

    /* Also check for directly encoded 24-bit UCS characters */
    if ((keysym & 0xff000000) == 0x01000000) {
        return keysym & 0x00ffffff;
    }

    /* Binary search in table */
    while (max >= min) {
        mid = (min + max) / 2;
        if (_sapp_x11_keysymtab[mid].keysym < keysym) {
            min = mid + 1;
        }
        else if (_sapp_x11_keysymtab[mid].keysym > keysym) {
            max = mid - 1;
        }
        else {
            return _sapp_x11_keysymtab[mid].ucs;
        }
    }

    /* No matching Unicode value found */
    return -1;
}

// XLib manual says keycodes are in the range [8, 255] inclusive.
// https://tronche.com/gui/x/xlib/input/keyboard-encoding.html
static bool _sapp_x11_keycodes[256];

_SOKOL_PRIVATE void _sapp_x11_process_event(XEvent* event) {
    switch (event->type) {
        case KeyPress:
            {
                int keycode = event->xkey.keycode;
                const sapp_keycode key = _sapp_x11_translate_key(keycode);
                bool repeat = _sapp_x11_keycodes[keycode & 0xFF];
                _sapp_x11_keycodes[keycode & 0xFF] = true;
                const uint32_t mods = _sapp_x11_mod(event->xkey.state);
                if (key != SAPP_KEYCODE_INVALID) {
                    _sapp_x11_key_event(SAPP_EVENTTYPE_KEY_DOWN, key, repeat, mods);
                }
                KeySym keysym;
                XLookupString(&event->xkey, NULL, 0, &keysym, NULL);
                int32_t chr = _sapp_x11_keysym_to_unicode(keysym);
                if (chr > 0) {
                    _sapp_x11_char_event((uint32_t)chr, repeat, mods);
                }
            }
            break;
        case KeyRelease:
            {
                int keycode = event->xkey.keycode;
                const sapp_keycode key = _sapp_x11_translate_key(keycode);
                _sapp_x11_keycodes[keycode & 0xFF] = false;
                if (key != SAPP_KEYCODE_INVALID) {
                    const uint32_t mods = _sapp_x11_mod(event->xkey.state);
                    _sapp_x11_key_event(SAPP_EVENTTYPE_KEY_UP, key, false, mods);
                }
            }
            break;
        case ButtonPress:
            {
                const sapp_mousebutton btn = _sapp_x11_translate_button(event);
                const uint32_t mods = _sapp_x11_mod(event->xbutton.state);
                if (btn != SAPP_MOUSEBUTTON_INVALID) {
                    _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, btn, mods);
                }
                else {
                    /* might be a scroll event */
                    switch (event->xbutton.button) {
                        case 4: _sapp_x11_scroll_event(0.0f, 1.0f, mods); break;
                        case 5: _sapp_x11_scroll_event(0.0f, -1.0f, mods); break;
                        case 6: _sapp_x11_scroll_event(1.0f, 0.0f, mods); break;
                        case 7: _sapp_x11_scroll_event(-1.0f, 0.0f, mods); break;
                    }
                }
            }
            break;
        case ButtonRelease:
            {
                const sapp_mousebutton btn = _sapp_x11_translate_button(event);
                if (btn != SAPP_MOUSEBUTTON_INVALID) {
                    _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, btn, _sapp_x11_mod(event->xbutton.state));
                }
            }
            break;
        case EnterNotify:
            _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mod(event->xcrossing.state));
            break;
        case LeaveNotify:
            _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mod(event->xcrossing.state));
            break;
        case MotionNotify:
            _sapp.mouse_x = event->xmotion.x;
            _sapp.mouse_y = event->xmotion.y;
            _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mod(event->xmotion.state));
            break;
        case ConfigureNotify:
            if ((event->xconfigure.width != _sapp.window_width) || (event->xconfigure.height != _sapp.window_height)) {
                _sapp.window_width = event->xconfigure.width;
                _sapp.window_height = event->xconfigure.height;
                _sapp.framebuffer_width = _sapp.window_width;
                _sapp.framebuffer_height = _sapp.window_height;
                _sapp_x11_app_event(SAPP_EVENTTYPE_RESIZED);
            }
            break;
        case PropertyNotify:
            if (event->xproperty.state == PropertyNewValue) {
                if (event->xproperty.atom == _sapp_x11_WM_STATE) {
                    const int state = _sapp_x11_get_window_state();
                    if (state != _sapp_x11_window_state) {
                        _sapp_x11_window_state = state;
                        if (state == IconicState) {
                            _sapp_x11_app_event(SAPP_EVENTTYPE_ICONIFIED);
                        }
                        else if (state == NormalState) {
                            _sapp_x11_app_event(SAPP_EVENTTYPE_RESTORED);
                        }
                    }
                }
            }
            break;
        case ClientMessage:
            if (event->xclient.message_type == _sapp_x11_WM_PROTOCOLS) {
                const Atom protocol = event->xclient.data.l[0];
                if (protocol == _sapp_x11_WM_DELETE_WINDOW) {
                    _sapp.quit_requested = true;
                }
            }
            break;
        case DestroyNotify:
            break;
    }
}

_SOKOL_PRIVATE void _sapp_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    _sapp_x11_window_state = NormalState;

    XInitThreads();
    XrmInitialize();
    _sapp_x11_display = XOpenDisplay(NULL);
    if (!_sapp_x11_display) {
        _sapp_fail("XOpenDisplay() failed!\n");
    }
    _sapp_x11_screen = DefaultScreen(_sapp_x11_display);
    _sapp_x11_root = DefaultRootWindow(_sapp_x11_display);
    XkbSetDetectableAutoRepeat(_sapp_x11_display, true, NULL);
    _sapp_x11_query_system_dpi();
    _sapp.dpi_scale = _sapp_x11_dpi / 96.0f;
    _sapp_x11_init_extensions();
    _sapp_glx_init();
    Visual* visual = 0;
    int depth = 0;
    _sapp_glx_choose_visual(&visual, &depth);
    _sapp_x11_create_window(visual, depth);
    _sapp_glx_create_context();
    _sapp.valid = true;
    _sapp_x11_show_window();
    _sapp_glx_swapinterval(_sapp.swap_interval);
    XFlush(_sapp_x11_display);
    while (!_sapp.quit_ordered) {
        _sapp_glx_make_current();
        int count = XPending(_sapp_x11_display);
        while (count--) {
            XEvent event;
            XNextEvent(_sapp_x11_display, &event);
            _sapp_x11_process_event(&event);
        }
        _sapp_frame();
        _sapp_glx_swap_buffers();
        XFlush(_sapp_x11_display);
        /* handle quit-requested, either from window or from sapp_request_quit() */
        if (_sapp.quit_requested && !_sapp.quit_ordered) {
            /* give user code a chance to intervene */
            _sapp_x11_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
            /* if user code hasn't intervened, quit the app */
            if (_sapp.quit_requested) {
                _sapp.quit_ordered = true;
            }
        }
    }
    _sapp_call_cleanup();
    _sapp_glx_destroy_context();
    _sapp_x11_destroy_window();
    XCloseDisplay(_sapp_x11_display);
    _sapp_discard_state();
}

#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */
#endif /* LINUX */

/*== PUBLIC API FUNCTIONS ====================================================*/
#if defined(SOKOL_NO_ENTRY)
SOKOL_API_IMPL int sapp_run(const sapp_desc* desc) {
    SOKOL_ASSERT(desc);
    _sapp_run(desc);
    return 0;
}

/* this is just a stub so the linker doesn't complain */
sapp_desc sokol_main(int argc, char* argv[]) {
    _SOKOL_UNUSED(argc);
    _SOKOL_UNUSED(argv);
    sapp_desc desc;
    memset(&desc, 0, sizeof(desc));
    return desc;
}
#else
/* likewise, in normal mode, sapp_run() is just an empty stub */
SOKOL_API_IMPL int sapp_run(const sapp_desc* desc) {
    _SOKOL_UNUSED(desc);
    return 0;
}
#endif

SOKOL_API_IMPL bool sapp_isvalid(void) {
    return _sapp.valid;
}

SOKOL_API_IMPL void* sapp_userdata(void) {
    return _sapp.desc.user_data;
}

SOKOL_API_IMPL sapp_desc sapp_query_desc(void) {
    return _sapp.desc;
}

SOKOL_API_IMPL uint64_t sapp_frame_count(void) {
    return _sapp.frame_count;
}

SOKOL_API_IMPL int sapp_width(void) {
    return (_sapp.framebuffer_width > 0) ? _sapp.framebuffer_width : 1;
}

SOKOL_API_IMPL int sapp_color_format(void) {
    #if defined(SOKOL_WGPU)
        switch (_sapp_emsc.wgpu.render_format) {
            case WGPUTextureFormat_RGBA8Unorm:
                return _SAPP_PIXELFORMAT_RGBA8;
            case WGPUTextureFormat_BGRA8Unorm:
                return _SAPP_PIXELFORMAT_BGRA8;
            default:
                SOKOL_UNREACHABLE;
                return 0;
        }
    #elif defined(SOKOL_METAL) || defined(SOKOL_D3D11)
        return _SAPP_PIXELFORMAT_BGRA8;
    #else
        return _SAPP_PIXELFORMAT_RGBA8;
    #endif
}

SOKOL_API_IMPL int sapp_depth_format(void) {
    return _SAPP_PIXELFORMAT_DEPTH_STENCIL;
}

SOKOL_API_IMPL int sapp_sample_count(void) {
    return _sapp.sample_count;
}

SOKOL_API_IMPL int sapp_height(void) {
    return (_sapp.framebuffer_height > 0) ? _sapp.framebuffer_height : 1;
}

SOKOL_API_IMPL bool sapp_high_dpi(void) {
    return _sapp.desc.high_dpi && (_sapp.dpi_scale > 1.5f);
}

SOKOL_API_IMPL float sapp_dpi_scale(void) {
    return _sapp.dpi_scale;
}

SOKOL_API_IMPL bool sapp_gles2(void) {
    return _sapp.gles2_fallback;
}

SOKOL_API_IMPL void sapp_show_keyboard(bool shown) {
    #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    _sapp_ios_show_keyboard(shown);
    #elif defined(__EMSCRIPTEN__)
    _sapp_emsc_show_keyboard(shown);
    #elif defined(__ANDROID__)
    _sapp_android_show_keyboard(shown);
    #else
    _SOKOL_UNUSED(shown);
    #endif
}

SOKOL_API_IMPL bool sapp_keyboard_shown(void) {
    return _sapp.onscreen_keyboard_shown;
}

SOKOL_API_DECL bool sapp_is_fullscreen(void) {
    return _sapp.fullscreen;
}

SOKOL_API_DECL void sapp_toggle_fullscreen(void) {
    #if defined(__APPLE__) && !TARGET_OS_IPHONE
    _sapp_macos_toggle_fullscreen();
    #elif defined(_WIN32)
    _sapp_win32_toggle_fullscreen();
    #endif
}

SOKOL_API_IMPL void sapp_show_mouse(bool shown) {
    #if defined(__APPLE__) && !TARGET_OS_IPHONE
    _sapp_macos_show_mouse(shown);
    #elif defined(_WIN32)
    _sapp_win32_show_mouse(shown);
    #else
    _SOKOL_UNUSED(shown);
    #endif
}

SOKOL_API_IMPL bool sapp_mouse_shown(void) {
    #if defined(_WIN32)
    return _sapp_win32_mouse_shown();
    #else
    return false;
    #endif
}

SOKOL_API_IMPL void sapp_request_quit(void) {
    _sapp.quit_requested = true;
}

SOKOL_API_IMPL void sapp_cancel_quit(void) {
    _sapp.quit_requested = false;
}

SOKOL_API_IMPL void sapp_quit(void) {
    _sapp.quit_ordered = true;
}

SOKOL_API_IMPL void sapp_consume_event(void) {
    _sapp.event_consumed = true;
}

/* NOTE: on HTML5, sapp_set_clipboard_string() must be called from within event handler! */
SOKOL_API_IMPL void sapp_set_clipboard_string(const char* str) {
    if (!_sapp.clipboard_enabled) {
        return;
    }
    SOKOL_ASSERT(str);
    #if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
        _sapp_macos_set_clipboard_string(str);
    #elif defined(__EMSCRIPTEN__)
        _sapp_emsc_set_clipboard_string(str);
    #elif defined(_WIN32)
        _sapp_win32_set_clipboard_string(str);
    #else
        /* not implemented */
    #endif
    _sapp_strcpy(str, _sapp.clipboard, _sapp.clipboard_size);
}

SOKOL_API_IMPL const char* sapp_get_clipboard_string(void) {
    if (!_sapp.clipboard_enabled) {
        return "";
    }
    #if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
        return _sapp_macos_get_clipboard_string();
    #elif defined(__EMSCRIPTEN__)
        return _sapp.clipboard;
    #elif defined(_WIN32)
        return _sapp_win32_get_clipboard_string();
    #else
        /* not implemented */
        return _sapp.clipboard;
    #endif
}

SOKOL_API_IMPL const void* sapp_metal_get_device(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        const void* obj = (__bridge const void*) _sapp_mtl_device_obj;
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_metal_get_renderpass_descriptor(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        const void* obj =  (__bridge const void*) [_sapp_view_obj currentRenderPassDescriptor];
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_metal_get_drawable(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        const void* obj = (__bridge const void*) [_sapp_view_obj currentDrawable];
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_macos_get_window(void) {
    #if defined(__APPLE__) && !TARGET_OS_IPHONE
        const void* obj = (__bridge const void*) _sapp_macos_window_obj;
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_ios_get_window(void) {
    #if defined(__APPLE__) && TARGET_OS_IPHONE
        const void* obj = (__bridge const void*) _sapp_ios_window_obj;
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif

}

SOKOL_API_IMPL const void* sapp_d3d11_get_device(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp_d3d11_device;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_device_context(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp_d3d11_device_context;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_render_target_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp_d3d11_rtv;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_depth_stencil_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp_d3d11_dsv;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_win32_get_hwnd(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(_WIN32)
        return _sapp_win32_hwnd;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_wgpu_get_device(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_WGPU)
        return (const void*) _sapp_emsc.wgpu.device;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_wgpu_get_render_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_WGPU)
        if (_sapp.sample_count > 1) {
            return (const void*) _sapp_emsc.wgpu.msaa_view;
        }
        else {
            return (const void*) _sapp_emsc.wgpu.swapchain_view;
        }
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_wgpu_get_resolve_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_WGPU)
        if (_sapp.sample_count > 1) {
            return (const void*) _sapp_emsc.wgpu.swapchain_view;
        }
        else {
            return 0;
        }
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_wgpu_get_depth_stencil_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_WGPU)
        return (const void*) _sapp_emsc.wgpu.depth_stencil_view;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_android_get_native_activity(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(__ANDROID__)
        return (void*)_sapp_android_state.activity;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL void sapp_html5_ask_leave_site(bool ask) {
    _sapp.html5_ask_leave_site = ask;
}

#undef _sapp_def

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* SOKOL_IMPL */
#pragma endregion

#pragma region sokol-time
#ifndef SOKOL_TIME_INCLUDED
/*
    sokol_time.h    -- simple cross-platform time measurement

    Project URL: https://github.com/floooh/sokol

    Do this:
        #define SOKOL_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.

    Optionally provide the following defines with your own implementations:
    SOKOL_ASSERT(c)     - your own assert macro (default: assert(c))
    SOKOL_API_DECL      - public function declaration prefix (default: extern)
    SOKOL_API_IMPL      - public function implementation prefix (default: -)

    If sokol_time.h is compiled as a DLL, define the following before
    including the declaration or implementation:

    SOKOL_DLL

    On Windows, SOKOL_DLL will define SOKOL_API_DECL as __declspec(dllexport)
    or __declspec(dllimport) as needed.

    void stm_setup();
        Call once before any other functions to initialize sokol_time
        (this calls for instance QueryPerformanceFrequency on Windows)

    uint64_t stm_now();
        Get current point in time in unspecified 'ticks'. The value that
        is returned has no relation to the 'wall-clock' time and is
        not in a specific time unit, it is only useful to compute
        time differences.

    uint64_t stm_diff(uint64_t new, uint64_t old);
        Computes the time difference between new and old. This will always
        return a positive, non-zero value.

    uint64_t stm_since(uint64_t start);
        Takes the current time, and returns the elapsed time since start
        (this is a shortcut for "stm_diff(stm_now(), start)")

    uint64_t stm_laptime(uint64_t* last_time);
        This is useful for measuring frame time and other recurring
        events. It takes the current time, returns the time difference
        to the value in last_time, and stores the current time in
        last_time for the next call. If the value in last_time is 0,
        the return value will be zero (this usually happens on the
        very first call).

    uint64_t stm_round_to_common_refresh_rate(uint64_t duration)
        This oddly named function takes a measured frame time and
        returns the closest "nearby" common display refresh rate frame duration
        in ticks. If the input duration isn't close to any common display
        refresh rate, the input duration will be returned unchanged as a fallback.
        The main purpose of this function is to remove jitter/inaccuracies from
        measured frame times, and instead use the display refresh rate as
        frame duration.

    Use the following functions to convert a duration in ticks into
    useful time units:

    double stm_sec(uint64_t ticks);
    double stm_ms(uint64_t ticks);
    double stm_us(uint64_t ticks);
    double stm_ns(uint64_t ticks);
        Converts a tick value into seconds, milliseconds, microseconds
        or nanoseconds. Note that not all platforms will have nanosecond
        or even microsecond precision.

    Uses the following time measurement functions under the hood:

    Windows:        QueryPerformanceFrequency() / QueryPerformanceCounter()
    MacOS/iOS:      mach_absolute_time()
    emscripten:     performance.now()
    Linux+others:   clock_gettime(CLOCK_MONOTONIC)

    zlib/libpng license

    Copyright (c) 2018 Andre Weissflog

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.

        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.

        3. This notice may not be removed or altered from any source
        distribution.
*/
#define SOKOL_TIME_INCLUDED (1)
#include <stdint.h>

#ifndef SOKOL_API_DECL
#if defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_IMPL)
#define SOKOL_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(SOKOL_DLL)
#define SOKOL_API_DECL __declspec(dllimport)
#else
#define SOKOL_API_DECL extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

SOKOL_API_DECL void stm_setup(void);
SOKOL_API_DECL uint64_t stm_now(void);
SOKOL_API_DECL uint64_t stm_diff(uint64_t new_ticks, uint64_t old_ticks);
SOKOL_API_DECL uint64_t stm_since(uint64_t start_ticks);
SOKOL_API_DECL uint64_t stm_laptime(uint64_t* last_time);
SOKOL_API_DECL uint64_t stm_round_to_common_refresh_rate(uint64_t frame_ticks);
SOKOL_API_DECL double stm_sec(uint64_t ticks);
SOKOL_API_DECL double stm_ms(uint64_t ticks);
SOKOL_API_DECL double stm_us(uint64_t ticks);
SOKOL_API_DECL double stm_ns(uint64_t ticks);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // SOKOL_TIME_INCLUDED

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef SOKOL_IMPL
#define SOKOL_TIME_IMPL_INCLUDED (1)
#include <string.h> /* memset */

#ifndef SOKOL_API_IMPL
    #define SOKOL_API_IMPL
#endif
#ifndef SOKOL_ASSERT
    #include <assert.h>
    #define SOKOL_ASSERT(c) assert(c)
#endif
#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
typedef struct {
    uint32_t initialized;
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
} _stm_state_t;
#elif defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
typedef struct {
    uint32_t initialized;
    mach_timebase_info_data_t timebase;
    uint64_t start;
} _stm_state_t;
#elif defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
typedef struct {
    uint32_t initialized;
    double start;
} _stm_state_t;
#else /* anything else, this will need more care for non-Linux platforms */
#ifdef ESP8266
// On the ESP8266, clock_gettime ignores the first argument and CLOCK_MONOTONIC isn't defined
#define CLOCK_MONOTONIC 0
#endif
#include <time.h>
typedef struct {
    uint32_t initialized;
    uint64_t start;
} _stm_state_t;
#endif
static _stm_state_t _stm;

/* prevent 64-bit overflow when computing relative timestamp
    see https://gist.github.com/jspohr/3dc4f00033d79ec5bdaf67bc46c813e3
*/
#if defined(_WIN32) || (defined(__APPLE__) && defined(__MACH__))
_SOKOL_PRIVATE int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

#if defined(__EMSCRIPTEN__)
EM_JS(double, stm_js_perfnow, (void), {
    return performance.now();
});
#endif

SOKOL_API_IMPL void stm_setup(void) {
    memset(&_stm, 0, sizeof(_stm));
    _stm.initialized = 0xABCDABCD;
    #if defined(_WIN32)
        QueryPerformanceFrequency(&_stm.freq);
        QueryPerformanceCounter(&_stm.start);
    #elif defined(__APPLE__) && defined(__MACH__)
        mach_timebase_info(&_stm.timebase);
        _stm.start = mach_absolute_time();
    #elif defined(__EMSCRIPTEN__)
        _stm.start = stm_js_perfnow();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        _stm.start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec;
    #endif
}

SOKOL_API_IMPL uint64_t stm_now(void) {
    SOKOL_ASSERT(_stm.initialized == 0xABCDABCD);
    uint64_t now;
    #if defined(_WIN32)
        LARGE_INTEGER qpc_t;
        QueryPerformanceCounter(&qpc_t);
        now = int64_muldiv(qpc_t.QuadPart - _stm.start.QuadPart, 1000000000, _stm.freq.QuadPart);
    #elif defined(__APPLE__) && defined(__MACH__)
        const uint64_t mach_now = mach_absolute_time() - _stm.start;
        now = int64_muldiv(mach_now, _stm.timebase.numer, _stm.timebase.denom);
    #elif defined(__EMSCRIPTEN__)
        double js_now = stm_js_perfnow() - _stm.start;
        SOKOL_ASSERT(js_now >= 0.0);
        now = (uint64_t) (js_now * 1000000.0);
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec) - _stm.start;
    #endif
    return now;
}

SOKOL_API_IMPL uint64_t stm_diff(uint64_t new_ticks, uint64_t old_ticks) {
    if (new_ticks > old_ticks) {
        return new_ticks - old_ticks;
    }
    else {
        return 1;
    }
}

SOKOL_API_IMPL uint64_t stm_since(uint64_t start_ticks) {
    return stm_diff(stm_now(), start_ticks);
}

SOKOL_API_IMPL uint64_t stm_laptime(uint64_t* last_time) {
    SOKOL_ASSERT(last_time);
    uint64_t dt = 0;
    uint64_t now = stm_now();
    if (0 != *last_time) {
        dt = stm_diff(now, *last_time);
    }
    *last_time = now;
    return dt;
}

// first number is frame duration in ns, second number is tolerance in ns,
// the resulting min/max values must not overlap!
static const uint64_t _stm_refresh_rates[][2] = {
    { 16666667, 1000000 },  //  60 Hz: 16.6667 +- 1ms
    { 13888889,  250000 },  //  72 Hz: 13.8889 +- 0.25ms
    { 13333333,  250000 },  //  75 Hz: 13.3333 +- 0.25ms
    { 11764706,  250000 },  //  85 Hz: 11.7647 +- 0.25
    { 11111111,  250000 },  //  90 Hz: 11.1111 +- 0.25ms
    {  8333333,  500000 },  // 120 Hz:  8.3333 +- 0.5ms
    {  6944445,  500000 },  // 144 Hz:  6.9445 +- 0.5ms
    {  4166667, 1000000 },  // 240 Hz:  4.1666 +- 1ms
    {        0,       0 },  // keep the last element always at zero
};

SOKOL_API_IMPL uint64_t stm_round_to_common_refresh_rate(uint64_t ticks) {
    uint64_t ns;
    int i = 0;
    while (0 != (ns = _stm_refresh_rates[i][0])) {
        uint64_t tol = _stm_refresh_rates[i][1];
        if ((ticks > (ns - tol)) && (ticks < (ns + tol))) {
            return ns;
        }
        i++;
    }
    // fallthough: didn't fit into any buckets
    return ticks;
}

SOKOL_API_IMPL double stm_sec(uint64_t ticks) {
    return (double)ticks / 1000000000.0;
}

SOKOL_API_IMPL double stm_ms(uint64_t ticks) {
    return (double)ticks / 1000000.0;
}

SOKOL_API_IMPL double stm_us(uint64_t ticks) {
    return (double)ticks / 1000.0;
}

SOKOL_API_IMPL double stm_ns(uint64_t ticks) {
    return (double)ticks;
}
#endif /* SOKOL_IMPL */

#pragma endregion

#pragma region glad

#pragma region header

/*

    OpenGL loader generated by glad 0.1.33 on Tue Jun  2 18:14:13 2020.

    Language/Generator: C/C++
    Specification: gl
    APIs: gl=3.3
    Profile: compatibility
    Extensions:

    Loader: True
    Local files: False
    Omit khrplatform: False
    Reproducible: False

    Commandline:
        --profile="compatibility" --api="gl=3.3" --generator="c" --spec="gl" --extensions=""
    Online:
        https://glad.dav1d.de/#profile=compatibility&language=c&specification=gl&loader=on&api=gl%3D3.3
*/


#ifndef __glad_h_
#define __glad_h_

#ifdef __gl_h_
#error OpenGL header already included, remove this include, glad already provides it
#endif
#define __gl_h_

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#define APIENTRY __stdcall
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY APIENTRY
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct gladGLversionStruct {
    int major;
    int minor;
};

typedef void* (* GLADloadproc)(const char *name);

#ifndef GLAPI
# if defined(GLAD_GLAPI_EXPORT)
#  if defined(_WIN32) || defined(__CYGWIN__)
#   if defined(GLAD_GLAPI_EXPORT_BUILD)
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllexport)) extern
#    else
#     define GLAPI __declspec(dllexport) extern
#    endif
#   else
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllimport)) extern
#    else
#     define GLAPI __declspec(dllimport) extern
#    endif
#   endif
#  elif defined(__GNUC__) && defined(GLAD_GLAPI_EXPORT_BUILD)
#   define GLAPI __attribute__ ((visibility ("default"))) extern
#  else
#   define GLAPI extern
#  endif
# else
#  define GLAPI extern
# endif
#endif

GLAPI struct gladGLversionStruct GLVersion;

GLAPI int gladLoadGL(void);

GLAPI int gladLoadGLLoader(GLADloadproc);

#pragma region #include <KHR/khrplatform.h>
#ifndef __khrplatform_h_
#define __khrplatform_h_

/*
** Copyright (c) 2008-2018 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/* Khronos platform-specific types and definitions.
 *
 * The master copy of khrplatform.h is maintained in the Khronos EGL
 * Registry repository at https://github.com/KhronosGroup/EGL-Registry
 * The last semantic modification to khrplatform.h was at commit ID:
 *      67a3e0864c2d75ea5287b9f3d2eb74a745936692
 *
 * Adopters may modify this file to suit their platform. Adopters are
 * encouraged to submit platform specific modifications to the Khronos
 * group so that they can be included in future versions of this file.
 * Please submit changes by filing pull requests or issues on
 * the EGL Registry repository linked above.
 *
 *
 * See the Implementer's Guidelines for information about where this file
 * should be located on your system and for more details of its use:
 *    http://www.khronos.org/registry/implementers_guide.pdf
 *
 * This file should be included as
 *        #include <KHR/khrplatform.h>
 * by Khronos client API header files that use its types and defines.
 *
 * The types in khrplatform.h should only be used to define API-specific types.
 *
 * Types defined in khrplatform.h:
 *    khronos_int8_t              signed   8  bit
 *    khronos_uint8_t             unsigned 8  bit
 *    khronos_int16_t             signed   16 bit
 *    khronos_uint16_t            unsigned 16 bit
 *    khronos_int32_t             signed   32 bit
 *    khronos_uint32_t            unsigned 32 bit
 *    khronos_int64_t             signed   64 bit
 *    khronos_uint64_t            unsigned 64 bit
 *    khronos_intptr_t            signed   same number of bits as a pointer
 *    khronos_uintptr_t           unsigned same number of bits as a pointer
 *    khronos_ssize_t             signed   size
 *    khronos_usize_t             unsigned size
 *    khronos_float_t             signed   32 bit floating point
 *    khronos_time_ns_t           unsigned 64 bit time in nanoseconds
 *    khronos_utime_nanoseconds_t unsigned time interval or absolute time in
 *                                         nanoseconds
 *    khronos_stime_nanoseconds_t signed time interval in nanoseconds
 *    khronos_boolean_enum_t      enumerated boolean type. This should
 *      only be used as a base type when a client API's boolean type is
 *      an enum. Client APIs which use an integer or other type for
 *      booleans cannot use this as the base type for their boolean.
 *
 * Tokens defined in khrplatform.h:
 *
 *    KHRONOS_FALSE, KHRONOS_TRUE Enumerated boolean false/true values.
 *
 *    KHRONOS_SUPPORT_INT64 is 1 if 64 bit integers are supported; otherwise 0.
 *    KHRONOS_SUPPORT_FLOAT is 1 if floats are supported; otherwise 0.
 *
 * Calling convention macros defined in this file:
 *    KHRONOS_APICALL
 *    KHRONOS_APIENTRY
 *    KHRONOS_APIATTRIBUTES
 *
 * These may be used in function prototypes as:
 *
 *      KHRONOS_APICALL void KHRONOS_APIENTRY funcname(
 *                                  int arg1,
 *                                  int arg2) KHRONOS_APIATTRIBUTES;
 */

#if defined(__SCITECH_SNAP__) && !defined(KHRONOS_STATIC)
#   define KHRONOS_STATIC 1
#endif

/*-------------------------------------------------------------------------
 * Definition of KHRONOS_APICALL
 *-------------------------------------------------------------------------
 * This precedes the return type of the function in the function prototype.
 */
#if defined(KHRONOS_STATIC)
    /* If the preprocessor constant KHRONOS_STATIC is defined, make the
     * header compatible with static linking. */
#   define KHRONOS_APICALL
#elif defined(_WIN32)
#   define KHRONOS_APICALL __declspec(dllimport)
#elif defined (__SYMBIAN32__)
#   define KHRONOS_APICALL IMPORT_C
#elif defined(__ANDROID__)
#   define KHRONOS_APICALL __attribute__((visibility("default")))
#else
#   define KHRONOS_APICALL
#endif

/*-------------------------------------------------------------------------
 * Definition of KHRONOS_APIENTRY
 *-------------------------------------------------------------------------
 * This follows the return type of the function  and precedes the function
 * name in the function prototype.
 */
#if defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__SCITECH_SNAP__)
    /* Win32 but not WinCE */
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APIENTRY
#endif

/*-------------------------------------------------------------------------
 * Definition of KHRONOS_APIATTRIBUTES
 *-------------------------------------------------------------------------
 * This follows the closing parenthesis of the function prototype arguments.
 */
#if defined (__ARMCC_2__)
#define KHRONOS_APIATTRIBUTES __softfp
#else
#define KHRONOS_APIATTRIBUTES
#endif

/*-------------------------------------------------------------------------
 * basic type definitions
 *-----------------------------------------------------------------------*/
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__GNUC__) || defined(__SCO__) || defined(__USLC__)


/*
 * Using <stdint.h>
 */
#include <stdint.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif defined(__VMS ) || defined(__sgi)

/*
 * Using <inttypes.h>
 */
#include <inttypes.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif defined(_WIN32) && !defined(__SCITECH_SNAP__)

/*
 * Win32
 */
typedef __int32                 khronos_int32_t;
typedef unsigned __int32        khronos_uint32_t;
typedef __int64                 khronos_int64_t;
typedef unsigned __int64        khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif defined(__sun__) || defined(__digital__)

/*
 * Sun or Digital
 */
typedef int                     khronos_int32_t;
typedef unsigned int            khronos_uint32_t;
#if defined(__arch64__) || defined(_LP64)
typedef long int                khronos_int64_t;
typedef unsigned long int       khronos_uint64_t;
#else
typedef long long int           khronos_int64_t;
typedef unsigned long long int  khronos_uint64_t;
#endif /* __arch64__ */
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#elif 0

/*
 * Hypothetical platform with no float or int64 support
 */
typedef int                     khronos_int32_t;
typedef unsigned int            khronos_uint32_t;
#define KHRONOS_SUPPORT_INT64   0
#define KHRONOS_SUPPORT_FLOAT   0

#else

/*
 * Generic fallback
 */
#include <stdint.h>
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#endif


/*
 * Types that are (so far) the same on all platforms
 */
typedef signed   char          khronos_int8_t;
typedef unsigned char          khronos_uint8_t;
typedef signed   short int     khronos_int16_t;
typedef unsigned short int     khronos_uint16_t;

/*
 * Types that differ between LLP64 and LP64 architectures - in LLP64,
 * pointers are 64 bits, but 'long' is still 32 bits. Win64 appears
 * to be the only LLP64 architecture in current use.
 */
#ifdef _WIN64
typedef signed   long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef unsigned long  int     khronos_uintptr_t;
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif

#if KHRONOS_SUPPORT_FLOAT
/*
 * Float type
 */
typedef          float         khronos_float_t;
#endif

#if KHRONOS_SUPPORT_INT64
/* Time types
 *
 * These types can be used to represent a time interval in nanoseconds or
 * an absolute Unadjusted System Time.  Unadjusted System Time is the number
 * of nanoseconds since some arbitrary system event (e.g. since the last
 * time the system booted).  The Unadjusted System Time is an unsigned
 * 64 bit value that wraps back to 0 every 584 years.  Time intervals
 * may be either signed or unsigned.
 */
typedef khronos_uint64_t       khronos_utime_nanoseconds_t;
typedef khronos_int64_t        khronos_stime_nanoseconds_t;
#endif

/*
 * Dummy value used to pad enum types to 32 bits.
 */
#ifndef KHRONOS_MAX_ENUM
#define KHRONOS_MAX_ENUM 0x7FFFFFFF
#endif

/*
 * Enumerated boolean type
 *
 * Values other than zero should be considered to be true.  Therefore
 * comparisons should not be made against KHRONOS_TRUE.
 */
typedef enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1,
    KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = KHRONOS_MAX_ENUM
} khronos_boolean_enum_t;

#endif /* __khrplatform_h_ */
#pragma endregion

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef khronos_int8_t GLbyte;
typedef khronos_uint8_t GLubyte;
typedef khronos_int16_t GLshort;
typedef khronos_uint16_t GLushort;
typedef int GLint;
typedef unsigned int GLuint;
typedef khronos_int32_t GLclampx;
typedef int GLsizei;
typedef khronos_float_t GLfloat;
typedef khronos_float_t GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void *GLeglClientBufferEXT;
typedef void *GLeglImageOES;
typedef char GLchar;
typedef char GLcharARB;
#ifdef __APPLE__
typedef void *GLhandleARB;
#else
typedef unsigned int GLhandleARB;
#endif
typedef khronos_uint16_t GLhalf;
typedef khronos_uint16_t GLhalfARB;
typedef khronos_int32_t GLfixed;
typedef khronos_intptr_t GLintptr;
typedef khronos_intptr_t GLintptrARB;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_ssize_t GLsizeiptrARB;
typedef khronos_int64_t GLint64;
typedef khronos_int64_t GLint64EXT;
typedef khronos_uint64_t GLuint64;
typedef khronos_uint64_t GLuint64EXT;
typedef struct __GLsync *GLsync;
struct _cl_context;
struct _cl_event;
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;
typedef void (APIENTRY *GLVULKANPROCNV)(void);
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_NONE 0
#define GL_FRONT_LEFT 0x0400
#define GL_FRONT_RIGHT 0x0401
#define GL_BACK_LEFT 0x0402
#define GL_BACK_RIGHT 0x0403
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_LEFT 0x0406
#define GL_RIGHT 0x0407
#define GL_FRONT_AND_BACK 0x0408
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_POINT_SIZE 0x0B11
#define GL_POINT_SIZE_RANGE 0x0B12
#define GL_POINT_SIZE_GRANULARITY 0x0B13
#define GL_LINE_SMOOTH 0x0B20
#define GL_LINE_WIDTH 0x0B21
#define GL_LINE_WIDTH_RANGE 0x0B22
#define GL_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_POLYGON_MODE 0x0B40
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_CULL_FACE 0x0B44
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_TEST 0x0B71
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_STENCIL_TEST 0x0B90
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_VIEWPORT 0x0BA2
#define GL_DITHER 0x0BD0
#define GL_BLEND_DST 0x0BE0
#define GL_BLEND_SRC 0x0BE1
#define GL_BLEND 0x0BE2
#define GL_LOGIC_OP_MODE 0x0BF0
#define GL_DRAW_BUFFER 0x0C01
#define GL_READ_BUFFER 0x0C02
#define GL_SCISSOR_BOX 0x0C10
#define GL_SCISSOR_TEST 0x0C11
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_DOUBLEBUFFER 0x0C32
#define GL_STEREO 0x0C33
#define GL_LINE_SMOOTH_HINT 0x0C52
#define GL_POLYGON_SMOOTH_HINT 0x0C53
#define GL_UNPACK_SWAP_BYTES 0x0CF0
#define GL_UNPACK_LSB_FIRST 0x0CF1
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_SWAP_BYTES 0x0D00
#define GL_PACK_LSB_FIRST 0x0D01
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_DONT_CARE 0x1100
#define GL_FASTEST 0x1101
#define GL_NICEST 0x1102
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_CLEAR 0x1500
#define GL_AND 0x1501
#define GL_AND_REVERSE 0x1502
#define GL_COPY 0x1503
#define GL_AND_INVERTED 0x1504
#define GL_NOOP 0x1505
#define GL_XOR 0x1506
#define GL_OR 0x1507
#define GL_NOR 0x1508
#define GL_EQUIV 0x1509
#define GL_INVERT 0x150A
#define GL_OR_REVERSE 0x150B
#define GL_COPY_INVERTED 0x150C
#define GL_OR_INVERTED 0x150D
#define GL_NAND 0x150E
#define GL_SET 0x150F
#define GL_TEXTURE 0x1702
#define GL_COLOR 0x1800
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_STENCIL_INDEX 0x1901
#define GL_DEPTH_COMPONENT 0x1902
#define GL_RED 0x1903
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_REPEAT 0x2901
#define GL_CURRENT_BIT 0x00000001
#define GL_POINT_BIT 0x00000002
#define GL_LINE_BIT 0x00000004
#define GL_POLYGON_BIT 0x00000008
#define GL_POLYGON_STIPPLE_BIT 0x00000010
#define GL_PIXEL_MODE_BIT 0x00000020
#define GL_LIGHTING_BIT 0x00000040
#define GL_FOG_BIT 0x00000080
#define GL_ACCUM_BUFFER_BIT 0x00000200
#define GL_VIEWPORT_BIT 0x00000800
#define GL_TRANSFORM_BIT 0x00001000
#define GL_ENABLE_BIT 0x00002000
#define GL_HINT_BIT 0x00008000
#define GL_EVAL_BIT 0x00010000
#define GL_LIST_BIT 0x00020000
#define GL_TEXTURE_BIT 0x00040000
#define GL_SCISSOR_BIT 0x00080000
#define GL_ALL_ATTRIB_BITS 0xFFFFFFFF
#define GL_QUAD_STRIP 0x0008
#define GL_POLYGON 0x0009
#define GL_ACCUM 0x0100
#define GL_LOAD 0x0101
#define GL_RETURN 0x0102
#define GL_MULT 0x0103
#define GL_ADD 0x0104
#define GL_AUX0 0x0409
#define GL_AUX1 0x040A
#define GL_AUX2 0x040B
#define GL_AUX3 0x040C
#define GL_2D 0x0600
#define GL_3D 0x0601
#define GL_3D_COLOR 0x0602
#define GL_3D_COLOR_TEXTURE 0x0603
#define GL_4D_COLOR_TEXTURE 0x0604
#define GL_PASS_THROUGH_TOKEN 0x0700
#define GL_POINT_TOKEN 0x0701
#define GL_LINE_TOKEN 0x0702
#define GL_POLYGON_TOKEN 0x0703
#define GL_BITMAP_TOKEN 0x0704
#define GL_DRAW_PIXEL_TOKEN 0x0705
#define GL_COPY_PIXEL_TOKEN 0x0706
#define GL_LINE_RESET_TOKEN 0x0707
#define GL_EXP 0x0800
#define GL_EXP2 0x0801
#define GL_COEFF 0x0A00
#define GL_ORDER 0x0A01
#define GL_DOMAIN 0x0A02
#define GL_PIXEL_MAP_I_TO_I 0x0C70
#define GL_PIXEL_MAP_S_TO_S 0x0C71
#define GL_PIXEL_MAP_I_TO_R 0x0C72
#define GL_PIXEL_MAP_I_TO_G 0x0C73
#define GL_PIXEL_MAP_I_TO_B 0x0C74
#define GL_PIXEL_MAP_I_TO_A 0x0C75
#define GL_PIXEL_MAP_R_TO_R 0x0C76
#define GL_PIXEL_MAP_G_TO_G 0x0C77
#define GL_PIXEL_MAP_B_TO_B 0x0C78
#define GL_PIXEL_MAP_A_TO_A 0x0C79
#define GL_CURRENT_COLOR 0x0B00
#define GL_CURRENT_INDEX 0x0B01
#define GL_CURRENT_NORMAL 0x0B02
#define GL_CURRENT_TEXTURE_COORDS 0x0B03
#define GL_CURRENT_RASTER_COLOR 0x0B04
#define GL_CURRENT_RASTER_INDEX 0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS 0x0B06
#define GL_CURRENT_RASTER_POSITION 0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID 0x0B08
#define GL_CURRENT_RASTER_DISTANCE 0x0B09
#define GL_POINT_SMOOTH 0x0B10
#define GL_LINE_STIPPLE 0x0B24
#define GL_LINE_STIPPLE_PATTERN 0x0B25
#define GL_LINE_STIPPLE_REPEAT 0x0B26
#define GL_LIST_MODE 0x0B30
#define GL_MAX_LIST_NESTING 0x0B31
#define GL_LIST_BASE 0x0B32
#define GL_LIST_INDEX 0x0B33
#define GL_POLYGON_STIPPLE 0x0B42
#define GL_EDGE_FLAG 0x0B43
#define GL_LIGHTING 0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE 0x0B52
#define GL_LIGHT_MODEL_AMBIENT 0x0B53
#define GL_SHADE_MODEL 0x0B54
#define GL_COLOR_MATERIAL_FACE 0x0B55
#define GL_COLOR_MATERIAL_PARAMETER 0x0B56
#define GL_COLOR_MATERIAL 0x0B57
#define GL_FOG 0x0B60
#define GL_FOG_INDEX 0x0B61
#define GL_FOG_DENSITY 0x0B62
#define GL_FOG_START 0x0B63
#define GL_FOG_END 0x0B64
#define GL_FOG_MODE 0x0B65
#define GL_FOG_COLOR 0x0B66
#define GL_ACCUM_CLEAR_VALUE 0x0B80
#define GL_MATRIX_MODE 0x0BA0
#define GL_NORMALIZE 0x0BA1
#define GL_MODELVIEW_STACK_DEPTH 0x0BA3
#define GL_PROJECTION_STACK_DEPTH 0x0BA4
#define GL_TEXTURE_STACK_DEPTH 0x0BA5
#define GL_MODELVIEW_MATRIX 0x0BA6
#define GL_PROJECTION_MATRIX 0x0BA7
#define GL_TEXTURE_MATRIX 0x0BA8
#define GL_ATTRIB_STACK_DEPTH 0x0BB0
#define GL_ALPHA_TEST 0x0BC0
#define GL_ALPHA_TEST_FUNC 0x0BC1
#define GL_ALPHA_TEST_REF 0x0BC2
#define GL_LOGIC_OP 0x0BF1
#define GL_AUX_BUFFERS 0x0C00
#define GL_INDEX_CLEAR_VALUE 0x0C20
#define GL_INDEX_WRITEMASK 0x0C21
#define GL_INDEX_MODE 0x0C30
#define GL_RGBA_MODE 0x0C31
#define GL_RENDER_MODE 0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT 0x0C50
#define GL_POINT_SMOOTH_HINT 0x0C51
#define GL_FOG_HINT 0x0C54
#define GL_TEXTURE_GEN_S 0x0C60
#define GL_TEXTURE_GEN_T 0x0C61
#define GL_TEXTURE_GEN_R 0x0C62
#define GL_TEXTURE_GEN_Q 0x0C63
#define GL_PIXEL_MAP_I_TO_I_SIZE 0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE 0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE 0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE 0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE 0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE 0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE 0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE 0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE 0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE 0x0CB9
#define GL_MAP_COLOR 0x0D10
#define GL_MAP_STENCIL 0x0D11
#define GL_INDEX_SHIFT 0x0D12
#define GL_INDEX_OFFSET 0x0D13
#define GL_RED_SCALE 0x0D14
#define GL_RED_BIAS 0x0D15
#define GL_ZOOM_X 0x0D16
#define GL_ZOOM_Y 0x0D17
#define GL_GREEN_SCALE 0x0D18
#define GL_GREEN_BIAS 0x0D19
#define GL_BLUE_SCALE 0x0D1A
#define GL_BLUE_BIAS 0x0D1B
#define GL_ALPHA_SCALE 0x0D1C
#define GL_ALPHA_BIAS 0x0D1D
#define GL_DEPTH_SCALE 0x0D1E
#define GL_DEPTH_BIAS 0x0D1F
#define GL_MAX_EVAL_ORDER 0x0D30
#define GL_MAX_LIGHTS 0x0D31
#define GL_MAX_CLIP_PLANES 0x0D32
#define GL_MAX_PIXEL_MAP_TABLE 0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH 0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH 0x0D36
#define GL_MAX_NAME_STACK_DEPTH 0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH 0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH 0x0D39
#define GL_INDEX_BITS 0x0D51
#define GL_RED_BITS 0x0D52
#define GL_GREEN_BITS 0x0D53
#define GL_BLUE_BITS 0x0D54
#define GL_ALPHA_BITS 0x0D55
#define GL_DEPTH_BITS 0x0D56
#define GL_STENCIL_BITS 0x0D57
#define GL_ACCUM_RED_BITS 0x0D58
#define GL_ACCUM_GREEN_BITS 0x0D59
#define GL_ACCUM_BLUE_BITS 0x0D5A
#define GL_ACCUM_ALPHA_BITS 0x0D5B
#define GL_NAME_STACK_DEPTH 0x0D70
#define GL_AUTO_NORMAL 0x0D80
#define GL_MAP1_COLOR_4 0x0D90
#define GL_MAP1_INDEX 0x0D91
#define GL_MAP1_NORMAL 0x0D92
#define GL_MAP1_TEXTURE_COORD_1 0x0D93
#define GL_MAP1_TEXTURE_COORD_2 0x0D94
#define GL_MAP1_TEXTURE_COORD_3 0x0D95
#define GL_MAP1_TEXTURE_COORD_4 0x0D96
#define GL_MAP1_VERTEX_3 0x0D97
#define GL_MAP1_VERTEX_4 0x0D98
#define GL_MAP2_COLOR_4 0x0DB0
#define GL_MAP2_INDEX 0x0DB1
#define GL_MAP2_NORMAL 0x0DB2
#define GL_MAP2_TEXTURE_COORD_1 0x0DB3
#define GL_MAP2_TEXTURE_COORD_2 0x0DB4
#define GL_MAP2_TEXTURE_COORD_3 0x0DB5
#define GL_MAP2_TEXTURE_COORD_4 0x0DB6
#define GL_MAP2_VERTEX_3 0x0DB7
#define GL_MAP2_VERTEX_4 0x0DB8
#define GL_MAP1_GRID_DOMAIN 0x0DD0
#define GL_MAP1_GRID_SEGMENTS 0x0DD1
#define GL_MAP2_GRID_DOMAIN 0x0DD2
#define GL_MAP2_GRID_SEGMENTS 0x0DD3
#define GL_TEXTURE_COMPONENTS 0x1003
#define GL_TEXTURE_BORDER 0x1005
#define GL_AMBIENT 0x1200
#define GL_DIFFUSE 0x1201
#define GL_SPECULAR 0x1202
#define GL_POSITION 0x1203
#define GL_SPOT_DIRECTION 0x1204
#define GL_SPOT_EXPONENT 0x1205
#define GL_SPOT_CUTOFF 0x1206
#define GL_CONSTANT_ATTENUATION 0x1207
#define GL_LINEAR_ATTENUATION 0x1208
#define GL_QUADRATIC_ATTENUATION 0x1209
#define GL_COMPILE 0x1300
#define GL_COMPILE_AND_EXECUTE 0x1301
#define GL_2_BYTES 0x1407
#define GL_3_BYTES 0x1408
#define GL_4_BYTES 0x1409
#define GL_EMISSION 0x1600
#define GL_SHININESS 0x1601
#define GL_AMBIENT_AND_DIFFUSE 0x1602
#define GL_COLOR_INDEXES 0x1603
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_COLOR_INDEX 0x1900
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_BITMAP 0x1A00
#define GL_RENDER 0x1C00
#define GL_FEEDBACK 0x1C01
#define GL_SELECT 0x1C02
#define GL_FLAT 0x1D00
#define GL_SMOOTH 0x1D01
#define GL_S 0x2000
#define GL_T 0x2001
#define GL_R 0x2002
#define GL_Q 0x2003
#define GL_MODULATE 0x2100
#define GL_DECAL 0x2101
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_TEXTURE_ENV_COLOR 0x2201
#define GL_TEXTURE_ENV 0x2300
#define GL_EYE_LINEAR 0x2400
#define GL_OBJECT_LINEAR 0x2401
#define GL_SPHERE_MAP 0x2402
#define GL_TEXTURE_GEN_MODE 0x2500
#define GL_OBJECT_PLANE 0x2501
#define GL_EYE_PLANE 0x2502
#define GL_CLAMP 0x2900
#define GL_CLIP_PLANE0 0x3000
#define GL_CLIP_PLANE1 0x3001
#define GL_CLIP_PLANE2 0x3002
#define GL_CLIP_PLANE3 0x3003
#define GL_CLIP_PLANE4 0x3004
#define GL_CLIP_PLANE5 0x3005
#define GL_LIGHT0 0x4000
#define GL_LIGHT1 0x4001
#define GL_LIGHT2 0x4002
#define GL_LIGHT3 0x4003
#define GL_LIGHT4 0x4004
#define GL_LIGHT5 0x4005
#define GL_LIGHT6 0x4006
#define GL_LIGHT7 0x4007
#define GL_COLOR_LOGIC_OP 0x0BF2
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_TEXTURE_BINDING_1D 0x8068
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_DOUBLE 0x140A
#define GL_PROXY_TEXTURE_1D 0x8063
#define GL_PROXY_TEXTURE_2D 0x8064
#define GL_R3_G3_B2 0x2A10
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB8 0x8051
#define GL_RGB10 0x8052
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGBA2 0x8055
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGBA8 0x8058
#define GL_RGB10_A2 0x8059
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_CLIENT_PIXEL_STORE_BIT 0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS 0xFFFFFFFF
#define GL_VERTEX_ARRAY_POINTER 0x808E
#define GL_NORMAL_ARRAY_POINTER 0x808F
#define GL_COLOR_ARRAY_POINTER 0x8090
#define GL_INDEX_ARRAY_POINTER 0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER 0x8093
#define GL_FEEDBACK_BUFFER_POINTER 0x0DF0
#define GL_SELECTION_BUFFER_POINTER 0x0DF3
#define GL_CLIENT_ATTRIB_STACK_DEPTH 0x0BB1
#define GL_INDEX_LOGIC_OP 0x0BF1
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH 0x0D3B
#define GL_FEEDBACK_BUFFER_SIZE 0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE 0x0DF2
#define GL_SELECTION_BUFFER_SIZE 0x0DF4
#define GL_VERTEX_ARRAY 0x8074
#define GL_NORMAL_ARRAY 0x8075
#define GL_COLOR_ARRAY 0x8076
#define GL_INDEX_ARRAY 0x8077
#define GL_TEXTURE_COORD_ARRAY 0x8078
#define GL_EDGE_FLAG_ARRAY 0x8079
#define GL_VERTEX_ARRAY_SIZE 0x807A
#define GL_VERTEX_ARRAY_TYPE 0x807B
#define GL_VERTEX_ARRAY_STRIDE 0x807C
#define GL_NORMAL_ARRAY_TYPE 0x807E
#define GL_NORMAL_ARRAY_STRIDE 0x807F
#define GL_COLOR_ARRAY_SIZE 0x8081
#define GL_COLOR_ARRAY_TYPE 0x8082
#define GL_COLOR_ARRAY_STRIDE 0x8083
#define GL_INDEX_ARRAY_TYPE 0x8085
#define GL_INDEX_ARRAY_STRIDE 0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE 0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE 0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE 0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE 0x808C
#define GL_TEXTURE_LUMINANCE_SIZE 0x8060
#define GL_TEXTURE_INTENSITY_SIZE 0x8061
#define GL_TEXTURE_PRIORITY 0x8066
#define GL_TEXTURE_RESIDENT 0x8067
#define GL_ALPHA4 0x803B
#define GL_ALPHA8 0x803C
#define GL_ALPHA12 0x803D
#define GL_ALPHA16 0x803E
#define GL_LUMINANCE4 0x803F
#define GL_LUMINANCE8 0x8040
#define GL_LUMINANCE12 0x8041
#define GL_LUMINANCE16 0x8042
#define GL_LUMINANCE4_ALPHA4 0x8043
#define GL_LUMINANCE6_ALPHA2 0x8044
#define GL_LUMINANCE8_ALPHA8 0x8045
#define GL_LUMINANCE12_ALPHA4 0x8046
#define GL_LUMINANCE12_ALPHA12 0x8047
#define GL_LUMINANCE16_ALPHA16 0x8048
#define GL_INTENSITY 0x8049
#define GL_INTENSITY4 0x804A
#define GL_INTENSITY8 0x804B
#define GL_INTENSITY12 0x804C
#define GL_INTENSITY16 0x804D
#define GL_V2F 0x2A20
#define GL_V3F 0x2A21
#define GL_C4UB_V2F 0x2A22
#define GL_C4UB_V3F 0x2A23
#define GL_C3F_V3F 0x2A24
#define GL_N3F_V3F 0x2A25
#define GL_C4F_N3F_V3F 0x2A26
#define GL_T2F_V3F 0x2A27
#define GL_T4F_V4F 0x2A28
#define GL_T2F_C4UB_V3F 0x2A29
#define GL_T2F_C3F_V3F 0x2A2A
#define GL_T2F_N3F_V3F 0x2A2B
#define GL_T2F_C4F_N3F_V3F 0x2A2C
#define GL_T4F_C4F_N3F_V4F 0x2A2D
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_TEXTURE_3D 0x806F
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_RESCALE_NORMAL 0x803A
#define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
#define GL_SINGLE_COLOR 0x81F9
#define GL_SEPARATE_SPECULAR_COLOR 0x81FA
#define GL_ALIASED_POINT_SIZE_RANGE 0x846D
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_MULTISAMPLE 0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE 0x809F
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLES 0x80A9
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP 0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_COMPRESSED_RGB 0x84ED
#define GL_COMPRESSED_RGBA 0x84EE
#define GL_TEXTURE_COMPRESSION_HINT 0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
#define GL_TEXTURE_COMPRESSED 0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLIENT_ACTIVE_TEXTURE 0x84E1
#define GL_MAX_TEXTURE_UNITS 0x84E2
#define GL_TRANSPOSE_MODELVIEW_MATRIX 0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX 0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX 0x84E6
#define GL_MULTISAMPLE_BIT 0x20000000
#define GL_NORMAL_MAP 0x8511
#define GL_REFLECTION_MAP 0x8512
#define GL_COMPRESSED_ALPHA 0x84E9
#define GL_COMPRESSED_LUMINANCE 0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA 0x84EB
#define GL_COMPRESSED_INTENSITY 0x84EC
#define GL_COMBINE 0x8570
#define GL_COMBINE_RGB 0x8571
#define GL_COMBINE_ALPHA 0x8572
#define GL_SOURCE0_RGB 0x8580
#define GL_SOURCE1_RGB 0x8581
#define GL_SOURCE2_RGB 0x8582
#define GL_SOURCE0_ALPHA 0x8588
#define GL_SOURCE1_ALPHA 0x8589
#define GL_SOURCE2_ALPHA 0x858A
#define GL_OPERAND0_RGB 0x8590
#define GL_OPERAND1_RGB 0x8591
#define GL_OPERAND2_RGB 0x8592
#define GL_OPERAND0_ALPHA 0x8598
#define GL_OPERAND1_ALPHA 0x8599
#define GL_OPERAND2_ALPHA 0x859A
#define GL_RGB_SCALE 0x8573
#define GL_ADD_SIGNED 0x8574
#define GL_INTERPOLATE 0x8575
#define GL_SUBTRACT 0x84E7
#define GL_CONSTANT 0x8576
#define GL_PRIMARY_COLOR 0x8577
#define GL_PREVIOUS 0x8578
#define GL_DOT3_RGB 0x86AE
#define GL_DOT3_RGBA 0x86AF
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_MIRRORED_REPEAT 0x8370
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_POINT_SIZE_MIN 0x8126
#define GL_POINT_SIZE_MAX 0x8127
#define GL_POINT_DISTANCE_ATTENUATION 0x8129
#define GL_GENERATE_MIPMAP 0x8191
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_FOG_COORDINATE_SOURCE 0x8450
#define GL_FOG_COORDINATE 0x8451
#define GL_FRAGMENT_DEPTH 0x8452
#define GL_CURRENT_FOG_COORDINATE 0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE 0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE 0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER 0x8456
#define GL_FOG_COORDINATE_ARRAY 0x8457
#define GL_COLOR_SUM 0x8458
#define GL_CURRENT_SECONDARY_COLOR 0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE 0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE 0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE 0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER 0x845D
#define GL_SECONDARY_COLOR_ARRAY 0x845E
#define GL_TEXTURE_FILTER_CONTROL 0x8500
#define GL_DEPTH_TEXTURE_MODE 0x884B
#define GL_COMPARE_R_TO_TEXTURE 0x884E
#define GL_BLEND_COLOR 0x8005
#define GL_BLEND_EQUATION 0x8009
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_FUNC_SUBTRACT 0x800A
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_CURRENT_QUERY 0x8865
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_BUFFER_ACCESS 0x88BB
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_SAMPLES_PASSED 0x8914
#define GL_SRC1_ALPHA 0x8589
#define GL_VERTEX_ARRAY_BUFFER_BINDING 0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING 0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING 0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING 0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING 0x889E
#define GL_FOG_COORD_SRC 0x8450
#define GL_FOG_COORD 0x8451
#define GL_CURRENT_FOG_COORD 0x8453
#define GL_FOG_COORD_ARRAY_TYPE 0x8454
#define GL_FOG_COORD_ARRAY_STRIDE 0x8455
#define GL_FOG_COORD_ARRAY_POINTER 0x8456
#define GL_FOG_COORD_ARRAY 0x8457
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING 0x889D
#define GL_SRC0_RGB 0x8580
#define GL_SRC1_RGB 0x8581
#define GL_SRC2_RGB 0x8582
#define GL_SRC0_ALPHA 0x8588
#define GL_SRC2_ALPHA 0x858A
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_DRAW_BUFFER14 0x8833
#define GL_DRAW_BUFFER15 0x8834
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_MAX_VARYING_FLOATS 0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE 0x8B4F
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_3D 0x8B5F
#define GL_SAMPLER_CUBE 0x8B60
#define GL_SAMPLER_1D_SHADOW 0x8B61
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_DELETE_STATUS 0x8B80
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#define GL_LOWER_LEFT 0x8CA1
#define GL_UPPER_LEFT 0x8CA2
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_VERTEX_PROGRAM_TWO_SIDE 0x8643
#define GL_POINT_SPRITE 0x8861
#define GL_COORD_REPLACE 0x8862
#define GL_MAX_TEXTURE_COORDS 0x8871
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB_ALPHA 0x8C42
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_COMPRESSED_SRGB 0x8C48
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
#define GL_CURRENT_RASTER_SECONDARY_COLOR 0x845F
#define GL_SLUMINANCE_ALPHA 0x8C44
#define GL_SLUMINANCE8_ALPHA8 0x8C45
#define GL_SLUMINANCE 0x8C46
#define GL_SLUMINANCE8 0x8C47
#define GL_COMPRESSED_SLUMINANCE 0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA 0x8C4B
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#define GL_CLIP_DISTANCE0 0x3000
#define GL_CLIP_DISTANCE1 0x3001
#define GL_CLIP_DISTANCE2 0x3002
#define GL_CLIP_DISTANCE3 0x3003
#define GL_CLIP_DISTANCE4 0x3004
#define GL_CLIP_DISTANCE5 0x3005
#define GL_CLIP_DISTANCE6 0x3006
#define GL_CLIP_DISTANCE7 0x3007
#define GL_MAX_CLIP_DISTANCES 0x0D32
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_NUM_EXTENSIONS 0x821D
#define GL_CONTEXT_FLAGS 0x821E
#define GL_COMPRESSED_RED 0x8225
#define GL_COMPRESSED_RG 0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815
#define GL_RGBA16F 0x881A
#define GL_RGB16F 0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
#define GL_CLAMP_READ_COLOR 0x891C
#define GL_FIXED_ONLY 0x891D
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#define GL_TEXTURE_1D_ARRAY 0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY 0x8C19
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY 0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY 0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_RGB9_E5 0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
#define GL_TEXTURE_SHARED_SIZE 0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED 0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS 0x8C8C
#define GL_SEPARATE_ATTRIBS 0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RED_INTEGER 0x8D94
#define GL_GREEN_INTEGER 0x8D95
#define GL_BLUE_INTEGER 0x8D96
#define GL_RGB_INTEGER 0x8D98
#define GL_RGBA_INTEGER 0x8D99
#define GL_BGR_INTEGER 0x8D9A
#define GL_BGRA_INTEGER 0x8D9B
#define GL_SAMPLER_1D_ARRAY 0x8DC0
#define GL_SAMPLER_2D_ARRAY 0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW 0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
#define GL_SAMPLER_CUBE_SHADOW 0x8DC5
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_INT_SAMPLER_1D 0x8DC9
#define GL_INT_SAMPLER_2D 0x8DCA
#define GL_INT_SAMPLER_3D 0x8DCB
#define GL_INT_SAMPLER_CUBE 0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY 0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D 0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
#define GL_QUERY_WAIT 0x8E13
#define GL_QUERY_NO_WAIT 0x8E14
#define GL_QUERY_BY_REGION_WAIT 0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT 0x8E16
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#define GL_BUFFER_MAP_LENGTH 0x9120
#define GL_BUFFER_MAP_OFFSET 0x9121
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT 0x8218
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_DEPTH_STENCIL 0x84F9
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_TEXTURE_STENCIL_SIZE 0x88F1
#define GL_TEXTURE_RED_TYPE 0x8C10
#define GL_TEXTURE_GREEN_TYPE 0x8C11
#define GL_TEXTURE_BLUE_TYPE 0x8C12
#define GL_TEXTURE_ALPHA_TYPE 0x8C13
#define GL_TEXTURE_DEPTH_TYPE 0x8C16
#define GL_UNSIGNED_NORMALIZED 0x8C17
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_RENDERBUFFER_SAMPLES 0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_STENCIL_INDEX1 0x8D46
#define GL_STENCIL_INDEX4 0x8D47
#define GL_STENCIL_INDEX8 0x8D48
#define GL_STENCIL_INDEX16 0x8D49
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES 0x8D57
#define GL_INDEX 0x8222
#define GL_TEXTURE_LUMINANCE_TYPE 0x8C14
#define GL_TEXTURE_INTENSITY_TYPE 0x8C15
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_HALF_FLOAT 0x140B
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_CLAMP_VERTEX_COLOR 0x891A
#define GL_CLAMP_FRAGMENT_COLOR 0x891B
#define GL_ALPHA_INTEGER 0x8D97
#define GL_SAMPLER_2D_RECT 0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
#define GL_SAMPLER_BUFFER 0x8DC2
#define GL_INT_SAMPLER_2D_RECT 0x8DCD
#define GL_INT_SAMPLER_BUFFER 0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
#define GL_TEXTURE_BINDING_BUFFER 0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGB8_SNORM 0x8F96
#define GL_RGBA8_SNORM 0x8F97
#define GL_R16_SNORM 0x8F98
#define GL_RG16_SNORM 0x8F99
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGBA16_SNORM 0x8F9B
#define GL_SIGNED_NORMALIZED 0x8F9C
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS 0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS 0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS 0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_UNIFORM_TYPE 0x8A37
#define GL_UNIFORM_SIZE 0x8A38
#define GL_UNIFORM_NAME_LENGTH 0x8A39
#define GL_UNIFORM_BLOCK_INDEX 0x8A3A
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE 0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE 0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR 0x8A3E
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH 0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX 0xFFFFFFFF
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY 0x000A
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_GEOMETRY_VERTICES_OUT 0x8916
#define GL_GEOMETRY_INPUT_TYPE 0x8917
#define GL_GEOMETRY_OUTPUT_TYPE 0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_DEPTH_CLAMP 0x864F
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION 0x8E4D
#define GL_LAST_VERTEX_CONVENTION 0x8E4E
#define GL_PROVOKING_VERTEX 0x8E4F
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#define GL_MAX_SERVER_WAIT_TIMEOUT 0x9111
#define GL_OBJECT_TYPE 0x9112
#define GL_SYNC_CONDITION 0x9113
#define GL_SYNC_STATUS 0x9114
#define GL_SYNC_FLAGS 0x9115
#define GL_SYNC_FENCE 0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_UNSIGNALED 0x9118
#define GL_SIGNALED 0x9119
#define GL_ALREADY_SIGNALED 0x911A
#define GL_TIMEOUT_EXPIRED 0x911B
#define GL_CONDITION_SATISFIED 0x911C
#define GL_WAIT_FAILED 0x911D
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFF
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#define GL_SAMPLE_POSITION 0x8E50
#define GL_SAMPLE_MASK 0x8E51
#define GL_SAMPLE_MASK_VALUE 0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE 0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES 0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE 0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
#define GL_MAX_INTEGER_SAMPLES 0x9110
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
#define GL_SRC1_COLOR 0x88F9
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS 0x88FC
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_SAMPLER_BINDING 0x8919
#define GL_RGB10_A2UI 0x906F
#define GL_TEXTURE_SWIZZLE_R 0x8E42
#define GL_TEXTURE_SWIZZLE_G 0x8E43
#define GL_TEXTURE_SWIZZLE_B 0x8E44
#define GL_TEXTURE_SWIZZLE_A 0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA 0x8E46
#define GL_TIME_ELAPSED 0x88BF
#define GL_TIMESTAMP 0x8E28
#define GL_INT_2_10_10_10_REV 0x8D9F
#ifndef GL_VERSION_1_0
#define GL_VERSION_1_0 1
GLAPI int GLAD_GL_VERSION_1_0;
typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
GLAPI PFNGLCULLFACEPROC glad_glCullFace;
#define glCullFace glad_glCullFace
typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum mode);
GLAPI PFNGLFRONTFACEPROC glad_glFrontFace;
#define glFrontFace glad_glFrontFace
typedef void (APIENTRYP PFNGLHINTPROC)(GLenum target, GLenum mode);
GLAPI PFNGLHINTPROC glad_glHint;
#define glHint glad_glHint
typedef void (APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat width);
GLAPI PFNGLLINEWIDTHPROC glad_glLineWidth;
#define glLineWidth glad_glLineWidth
typedef void (APIENTRYP PFNGLPOINTSIZEPROC)(GLfloat size);
GLAPI PFNGLPOINTSIZEPROC glad_glPointSize;
#define glPointSize glad_glPointSize
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
GLAPI PFNGLPOLYGONMODEPROC glad_glPolygonMode;
#define glPolygonMode glad_glPolygonMode
typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI PFNGLSCISSORPROC glad_glScissor;
#define glScissor glad_glScissor
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
GLAPI PFNGLTEXPARAMETERFPROC glad_glTexParameterf;
#define glTexParameterf glad_glTexParameterf
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
GLAPI PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv;
#define glTexParameterfv glad_glTexParameterfv
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
GLAPI PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
#define glTexParameteri glad_glTexParameteri
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum target, GLenum pname, const GLint *params);
GLAPI PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv;
#define glTexParameteriv glad_glTexParameteriv
typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXIMAGE1DPROC glad_glTexImage1D;
#define glTexImage1D glad_glTexImage1D
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
#define glTexImage2D glad_glTexImage2D
typedef void (APIENTRYP PFNGLDRAWBUFFERPROC)(GLenum buf);
GLAPI PFNGLDRAWBUFFERPROC glad_glDrawBuffer;
#define glDrawBuffer glad_glDrawBuffer
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
GLAPI PFNGLCLEARPROC glad_glClear;
#define glClear glad_glClear
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI PFNGLCLEARCOLORPROC glad_glClearColor;
#define glClearColor glad_glClearColor
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC)(GLint s);
GLAPI PFNGLCLEARSTENCILPROC glad_glClearStencil;
#define glClearStencil glad_glClearStencil
typedef void (APIENTRYP PFNGLCLEARDEPTHPROC)(GLdouble depth);
GLAPI PFNGLCLEARDEPTHPROC glad_glClearDepth;
#define glClearDepth glad_glClearDepth
typedef void (APIENTRYP PFNGLSTENCILMASKPROC)(GLuint mask);
GLAPI PFNGLSTENCILMASKPROC glad_glStencilMask;
#define glStencilMask glad_glStencilMask
typedef void (APIENTRYP PFNGLCOLORMASKPROC)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GLAPI PFNGLCOLORMASKPROC glad_glColorMask;
#define glColorMask glad_glColorMask
typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean flag);
GLAPI PFNGLDEPTHMASKPROC glad_glDepthMask;
#define glDepthMask glad_glDepthMask
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
GLAPI PFNGLDISABLEPROC glad_glDisable;
#define glDisable glad_glDisable
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
GLAPI PFNGLENABLEPROC glad_glEnable;
#define glEnable glad_glEnable
typedef void (APIENTRYP PFNGLFINISHPROC)(void);
GLAPI PFNGLFINISHPROC glad_glFinish;
#define glFinish glad_glFinish
typedef void (APIENTRYP PFNGLFLUSHPROC)(void);
GLAPI PFNGLFLUSHPROC glad_glFlush;
#define glFlush glad_glFlush
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
GLAPI PFNGLBLENDFUNCPROC glad_glBlendFunc;
#define glBlendFunc glad_glBlendFunc
typedef void (APIENTRYP PFNGLLOGICOPPROC)(GLenum opcode);
GLAPI PFNGLLOGICOPPROC glad_glLogicOp;
#define glLogicOp glad_glLogicOp
typedef void (APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
GLAPI PFNGLSTENCILFUNCPROC glad_glStencilFunc;
#define glStencilFunc glad_glStencilFunc
typedef void (APIENTRYP PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);
GLAPI PFNGLSTENCILOPPROC glad_glStencilOp;
#define glStencilOp glad_glStencilOp
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
GLAPI PFNGLDEPTHFUNCPROC glad_glDepthFunc;
#define glDepthFunc glad_glDepthFunc
typedef void (APIENTRYP PFNGLPIXELSTOREFPROC)(GLenum pname, GLfloat param);
GLAPI PFNGLPIXELSTOREFPROC glad_glPixelStoref;
#define glPixelStoref glad_glPixelStoref
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
GLAPI PFNGLPIXELSTOREIPROC glad_glPixelStorei;
#define glPixelStorei glad_glPixelStorei
typedef void (APIENTRYP PFNGLREADBUFFERPROC)(GLenum src);
GLAPI PFNGLREADBUFFERPROC glad_glReadBuffer;
#define glReadBuffer glad_glReadBuffer
typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
GLAPI PFNGLREADPIXELSPROC glad_glReadPixels;
#define glReadPixels glad_glReadPixels
typedef void (APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum pname, GLboolean *data);
GLAPI PFNGLGETBOOLEANVPROC glad_glGetBooleanv;
#define glGetBooleanv glad_glGetBooleanv
typedef void (APIENTRYP PFNGLGETDOUBLEVPROC)(GLenum pname, GLdouble *data);
GLAPI PFNGLGETDOUBLEVPROC glad_glGetDoublev;
#define glGetDoublev glad_glGetDoublev
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)(void);
GLAPI PFNGLGETERRORPROC glad_glGetError;
#define glGetError glad_glGetError
typedef void (APIENTRYP PFNGLGETFLOATVPROC)(GLenum pname, GLfloat *data);
GLAPI PFNGLGETFLOATVPROC glad_glGetFloatv;
#define glGetFloatv glad_glGetFloatv
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *data);
GLAPI PFNGLGETINTEGERVPROC glad_glGetIntegerv;
#define glGetIntegerv glad_glGetIntegerv
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
GLAPI PFNGLGETSTRINGPROC glad_glGetString;
#define glGetString glad_glGetString
typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC)(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
GLAPI PFNGLGETTEXIMAGEPROC glad_glGetTexImage;
#define glGetTexImage glad_glGetTexImage
typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC)(GLenum target, GLenum pname, GLfloat *params);
GLAPI PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv;
#define glGetTexParameterfv glad_glGetTexParameterfv
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
GLAPI PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv;
#define glGetTexParameteriv glad_glGetTexParameteriv
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC)(GLenum target, GLint level, GLenum pname, GLfloat *params);
GLAPI PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv;
#define glGetTexLevelParameterfv glad_glGetTexLevelParameterfv
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target, GLint level, GLenum pname, GLint *params);
GLAPI PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv;
#define glGetTexLevelParameteriv glad_glGetTexLevelParameteriv
typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC)(GLenum cap);
GLAPI PFNGLISENABLEDPROC glad_glIsEnabled;
#define glIsEnabled glad_glIsEnabled
typedef void (APIENTRYP PFNGLDEPTHRANGEPROC)(GLdouble n, GLdouble f);
GLAPI PFNGLDEPTHRANGEPROC glad_glDepthRange;
#define glDepthRange glad_glDepthRange
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI PFNGLVIEWPORTPROC glad_glViewport;
#define glViewport glad_glViewport
typedef void (APIENTRYP PFNGLNEWLISTPROC)(GLuint list, GLenum mode);
GLAPI PFNGLNEWLISTPROC glad_glNewList;
#define glNewList glad_glNewList
typedef void (APIENTRYP PFNGLENDLISTPROC)(void);
GLAPI PFNGLENDLISTPROC glad_glEndList;
#define glEndList glad_glEndList
typedef void (APIENTRYP PFNGLCALLLISTPROC)(GLuint list);
GLAPI PFNGLCALLLISTPROC glad_glCallList;
#define glCallList glad_glCallList
typedef void (APIENTRYP PFNGLCALLLISTSPROC)(GLsizei n, GLenum type, const void *lists);
GLAPI PFNGLCALLLISTSPROC glad_glCallLists;
#define glCallLists glad_glCallLists
typedef void (APIENTRYP PFNGLDELETELISTSPROC)(GLuint list, GLsizei range);
GLAPI PFNGLDELETELISTSPROC glad_glDeleteLists;
#define glDeleteLists glad_glDeleteLists
typedef GLuint (APIENTRYP PFNGLGENLISTSPROC)(GLsizei range);
GLAPI PFNGLGENLISTSPROC glad_glGenLists;
#define glGenLists glad_glGenLists
typedef void (APIENTRYP PFNGLLISTBASEPROC)(GLuint base);
GLAPI PFNGLLISTBASEPROC glad_glListBase;
#define glListBase glad_glListBase
typedef void (APIENTRYP PFNGLBEGINPROC)(GLenum mode);
GLAPI PFNGLBEGINPROC glad_glBegin;
#define glBegin glad_glBegin
typedef void (APIENTRYP PFNGLBITMAPPROC)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
GLAPI PFNGLBITMAPPROC glad_glBitmap;
#define glBitmap glad_glBitmap
typedef void (APIENTRYP PFNGLCOLOR3BPROC)(GLbyte red, GLbyte green, GLbyte blue);
GLAPI PFNGLCOLOR3BPROC glad_glColor3b;
#define glColor3b glad_glColor3b
typedef void (APIENTRYP PFNGLCOLOR3BVPROC)(const GLbyte *v);
GLAPI PFNGLCOLOR3BVPROC glad_glColor3bv;
#define glColor3bv glad_glColor3bv
typedef void (APIENTRYP PFNGLCOLOR3DPROC)(GLdouble red, GLdouble green, GLdouble blue);
GLAPI PFNGLCOLOR3DPROC glad_glColor3d;
#define glColor3d glad_glColor3d
typedef void (APIENTRYP PFNGLCOLOR3DVPROC)(const GLdouble *v);
GLAPI PFNGLCOLOR3DVPROC glad_glColor3dv;
#define glColor3dv glad_glColor3dv
typedef void (APIENTRYP PFNGLCOLOR3FPROC)(GLfloat red, GLfloat green, GLfloat blue);
GLAPI PFNGLCOLOR3FPROC glad_glColor3f;
#define glColor3f glad_glColor3f
typedef void (APIENTRYP PFNGLCOLOR3FVPROC)(const GLfloat *v);
GLAPI PFNGLCOLOR3FVPROC glad_glColor3fv;
#define glColor3fv glad_glColor3fv
typedef void (APIENTRYP PFNGLCOLOR3IPROC)(GLint red, GLint green, GLint blue);
GLAPI PFNGLCOLOR3IPROC glad_glColor3i;
#define glColor3i glad_glColor3i
typedef void (APIENTRYP PFNGLCOLOR3IVPROC)(const GLint *v);
GLAPI PFNGLCOLOR3IVPROC glad_glColor3iv;
#define glColor3iv glad_glColor3iv
typedef void (APIENTRYP PFNGLCOLOR3SPROC)(GLshort red, GLshort green, GLshort blue);
GLAPI PFNGLCOLOR3SPROC glad_glColor3s;
#define glColor3s glad_glColor3s
typedef void (APIENTRYP PFNGLCOLOR3SVPROC)(const GLshort *v);
GLAPI PFNGLCOLOR3SVPROC glad_glColor3sv;
#define glColor3sv glad_glColor3sv
typedef void (APIENTRYP PFNGLCOLOR3UBPROC)(GLubyte red, GLubyte green, GLubyte blue);
GLAPI PFNGLCOLOR3UBPROC glad_glColor3ub;
#define glColor3ub glad_glColor3ub
typedef void (APIENTRYP PFNGLCOLOR3UBVPROC)(const GLubyte *v);
GLAPI PFNGLCOLOR3UBVPROC glad_glColor3ubv;
#define glColor3ubv glad_glColor3ubv
typedef void (APIENTRYP PFNGLCOLOR3UIPROC)(GLuint red, GLuint green, GLuint blue);
GLAPI PFNGLCOLOR3UIPROC glad_glColor3ui;
#define glColor3ui glad_glColor3ui
typedef void (APIENTRYP PFNGLCOLOR3UIVPROC)(const GLuint *v);
GLAPI PFNGLCOLOR3UIVPROC glad_glColor3uiv;
#define glColor3uiv glad_glColor3uiv
typedef void (APIENTRYP PFNGLCOLOR3USPROC)(GLushort red, GLushort green, GLushort blue);
GLAPI PFNGLCOLOR3USPROC glad_glColor3us;
#define glColor3us glad_glColor3us
typedef void (APIENTRYP PFNGLCOLOR3USVPROC)(const GLushort *v);
GLAPI PFNGLCOLOR3USVPROC glad_glColor3usv;
#define glColor3usv glad_glColor3usv
typedef void (APIENTRYP PFNGLCOLOR4BPROC)(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
GLAPI PFNGLCOLOR4BPROC glad_glColor4b;
#define glColor4b glad_glColor4b
typedef void (APIENTRYP PFNGLCOLOR4BVPROC)(const GLbyte *v);
GLAPI PFNGLCOLOR4BVPROC glad_glColor4bv;
#define glColor4bv glad_glColor4bv
typedef void (APIENTRYP PFNGLCOLOR4DPROC)(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
GLAPI PFNGLCOLOR4DPROC glad_glColor4d;
#define glColor4d glad_glColor4d
typedef void (APIENTRYP PFNGLCOLOR4DVPROC)(const GLdouble *v);
GLAPI PFNGLCOLOR4DVPROC glad_glColor4dv;
#define glColor4dv glad_glColor4dv
typedef void (APIENTRYP PFNGLCOLOR4FPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI PFNGLCOLOR4FPROC glad_glColor4f;
#define glColor4f glad_glColor4f
typedef void (APIENTRYP PFNGLCOLOR4FVPROC)(const GLfloat *v);
GLAPI PFNGLCOLOR4FVPROC glad_glColor4fv;
#define glColor4fv glad_glColor4fv
typedef void (APIENTRYP PFNGLCOLOR4IPROC)(GLint red, GLint green, GLint blue, GLint alpha);
GLAPI PFNGLCOLOR4IPROC glad_glColor4i;
#define glColor4i glad_glColor4i
typedef void (APIENTRYP PFNGLCOLOR4IVPROC)(const GLint *v);
GLAPI PFNGLCOLOR4IVPROC glad_glColor4iv;
#define glColor4iv glad_glColor4iv
typedef void (APIENTRYP PFNGLCOLOR4SPROC)(GLshort red, GLshort green, GLshort blue, GLshort alpha);
GLAPI PFNGLCOLOR4SPROC glad_glColor4s;
#define glColor4s glad_glColor4s
typedef void (APIENTRYP PFNGLCOLOR4SVPROC)(const GLshort *v);
GLAPI PFNGLCOLOR4SVPROC glad_glColor4sv;
#define glColor4sv glad_glColor4sv
typedef void (APIENTRYP PFNGLCOLOR4UBPROC)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
GLAPI PFNGLCOLOR4UBPROC glad_glColor4ub;
#define glColor4ub glad_glColor4ub
typedef void (APIENTRYP PFNGLCOLOR4UBVPROC)(const GLubyte *v);
GLAPI PFNGLCOLOR4UBVPROC glad_glColor4ubv;
#define glColor4ubv glad_glColor4ubv
typedef void (APIENTRYP PFNGLCOLOR4UIPROC)(GLuint red, GLuint green, GLuint blue, GLuint alpha);
GLAPI PFNGLCOLOR4UIPROC glad_glColor4ui;
#define glColor4ui glad_glColor4ui
typedef void (APIENTRYP PFNGLCOLOR4UIVPROC)(const GLuint *v);
GLAPI PFNGLCOLOR4UIVPROC glad_glColor4uiv;
#define glColor4uiv glad_glColor4uiv
typedef void (APIENTRYP PFNGLCOLOR4USPROC)(GLushort red, GLushort green, GLushort blue, GLushort alpha);
GLAPI PFNGLCOLOR4USPROC glad_glColor4us;
#define glColor4us glad_glColor4us
typedef void (APIENTRYP PFNGLCOLOR4USVPROC)(const GLushort *v);
GLAPI PFNGLCOLOR4USVPROC glad_glColor4usv;
#define glColor4usv glad_glColor4usv
typedef void (APIENTRYP PFNGLEDGEFLAGPROC)(GLboolean flag);
GLAPI PFNGLEDGEFLAGPROC glad_glEdgeFlag;
#define glEdgeFlag glad_glEdgeFlag
typedef void (APIENTRYP PFNGLEDGEFLAGVPROC)(const GLboolean *flag);
GLAPI PFNGLEDGEFLAGVPROC glad_glEdgeFlagv;
#define glEdgeFlagv glad_glEdgeFlagv
typedef void (APIENTRYP PFNGLENDPROC)(void);
GLAPI PFNGLENDPROC glad_glEnd;
#define glEnd glad_glEnd
typedef void (APIENTRYP PFNGLINDEXDPROC)(GLdouble c);
GLAPI PFNGLINDEXDPROC glad_glIndexd;
#define glIndexd glad_glIndexd
typedef void (APIENTRYP PFNGLINDEXDVPROC)(const GLdouble *c);
GLAPI PFNGLINDEXDVPROC glad_glIndexdv;
#define glIndexdv glad_glIndexdv
typedef void (APIENTRYP PFNGLINDEXFPROC)(GLfloat c);
GLAPI PFNGLINDEXFPROC glad_glIndexf;
#define glIndexf glad_glIndexf
typedef void (APIENTRYP PFNGLINDEXFVPROC)(const GLfloat *c);
GLAPI PFNGLINDEXFVPROC glad_glIndexfv;
#define glIndexfv glad_glIndexfv
typedef void (APIENTRYP PFNGLINDEXIPROC)(GLint c);
GLAPI PFNGLINDEXIPROC glad_glIndexi;
#define glIndexi glad_glIndexi
typedef void (APIENTRYP PFNGLINDEXIVPROC)(const GLint *c);
GLAPI PFNGLINDEXIVPROC glad_glIndexiv;
#define glIndexiv glad_glIndexiv
typedef void (APIENTRYP PFNGLINDEXSPROC)(GLshort c);
GLAPI PFNGLINDEXSPROC glad_glIndexs;
#define glIndexs glad_glIndexs
typedef void (APIENTRYP PFNGLINDEXSVPROC)(const GLshort *c);
GLAPI PFNGLINDEXSVPROC glad_glIndexsv;
#define glIndexsv glad_glIndexsv
typedef void (APIENTRYP PFNGLNORMAL3BPROC)(GLbyte nx, GLbyte ny, GLbyte nz);
GLAPI PFNGLNORMAL3BPROC glad_glNormal3b;
#define glNormal3b glad_glNormal3b
typedef void (APIENTRYP PFNGLNORMAL3BVPROC)(const GLbyte *v);
GLAPI PFNGLNORMAL3BVPROC glad_glNormal3bv;
#define glNormal3bv glad_glNormal3bv
typedef void (APIENTRYP PFNGLNORMAL3DPROC)(GLdouble nx, GLdouble ny, GLdouble nz);
GLAPI PFNGLNORMAL3DPROC glad_glNormal3d;
#define glNormal3d glad_glNormal3d
typedef void (APIENTRYP PFNGLNORMAL3DVPROC)(const GLdouble *v);
GLAPI PFNGLNORMAL3DVPROC glad_glNormal3dv;
#define glNormal3dv glad_glNormal3dv
typedef void (APIENTRYP PFNGLNORMAL3FPROC)(GLfloat nx, GLfloat ny, GLfloat nz);
GLAPI PFNGLNORMAL3FPROC glad_glNormal3f;
#define glNormal3f glad_glNormal3f
typedef void (APIENTRYP PFNGLNORMAL3FVPROC)(const GLfloat *v);
GLAPI PFNGLNORMAL3FVPROC glad_glNormal3fv;
#define glNormal3fv glad_glNormal3fv
typedef void (APIENTRYP PFNGLNORMAL3IPROC)(GLint nx, GLint ny, GLint nz);
GLAPI PFNGLNORMAL3IPROC glad_glNormal3i;
#define glNormal3i glad_glNormal3i
typedef void (APIENTRYP PFNGLNORMAL3IVPROC)(const GLint *v);
GLAPI PFNGLNORMAL3IVPROC glad_glNormal3iv;
#define glNormal3iv glad_glNormal3iv
typedef void (APIENTRYP PFNGLNORMAL3SPROC)(GLshort nx, GLshort ny, GLshort nz);
GLAPI PFNGLNORMAL3SPROC glad_glNormal3s;
#define glNormal3s glad_glNormal3s
typedef void (APIENTRYP PFNGLNORMAL3SVPROC)(const GLshort *v);
GLAPI PFNGLNORMAL3SVPROC glad_glNormal3sv;
#define glNormal3sv glad_glNormal3sv
typedef void (APIENTRYP PFNGLRASTERPOS2DPROC)(GLdouble x, GLdouble y);
GLAPI PFNGLRASTERPOS2DPROC glad_glRasterPos2d;
#define glRasterPos2d glad_glRasterPos2d
typedef void (APIENTRYP PFNGLRASTERPOS2DVPROC)(const GLdouble *v);
GLAPI PFNGLRASTERPOS2DVPROC glad_glRasterPos2dv;
#define glRasterPos2dv glad_glRasterPos2dv
typedef void (APIENTRYP PFNGLRASTERPOS2FPROC)(GLfloat x, GLfloat y);
GLAPI PFNGLRASTERPOS2FPROC glad_glRasterPos2f;
#define glRasterPos2f glad_glRasterPos2f
typedef void (APIENTRYP PFNGLRASTERPOS2FVPROC)(const GLfloat *v);
GLAPI PFNGLRASTERPOS2FVPROC glad_glRasterPos2fv;
#define glRasterPos2fv glad_glRasterPos2fv
typedef void (APIENTRYP PFNGLRASTERPOS2IPROC)(GLint x, GLint y);
GLAPI PFNGLRASTERPOS2IPROC glad_glRasterPos2i;
#define glRasterPos2i glad_glRasterPos2i
typedef void (APIENTRYP PFNGLRASTERPOS2IVPROC)(const GLint *v);
GLAPI PFNGLRASTERPOS2IVPROC glad_glRasterPos2iv;
#define glRasterPos2iv glad_glRasterPos2iv
typedef void (APIENTRYP PFNGLRASTERPOS2SPROC)(GLshort x, GLshort y);
GLAPI PFNGLRASTERPOS2SPROC glad_glRasterPos2s;
#define glRasterPos2s glad_glRasterPos2s
typedef void (APIENTRYP PFNGLRASTERPOS2SVPROC)(const GLshort *v);
GLAPI PFNGLRASTERPOS2SVPROC glad_glRasterPos2sv;
#define glRasterPos2sv glad_glRasterPos2sv
typedef void (APIENTRYP PFNGLRASTERPOS3DPROC)(GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLRASTERPOS3DPROC glad_glRasterPos3d;
#define glRasterPos3d glad_glRasterPos3d
typedef void (APIENTRYP PFNGLRASTERPOS3DVPROC)(const GLdouble *v);
GLAPI PFNGLRASTERPOS3DVPROC glad_glRasterPos3dv;
#define glRasterPos3dv glad_glRasterPos3dv
typedef void (APIENTRYP PFNGLRASTERPOS3FPROC)(GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLRASTERPOS3FPROC glad_glRasterPos3f;
#define glRasterPos3f glad_glRasterPos3f
typedef void (APIENTRYP PFNGLRASTERPOS3FVPROC)(const GLfloat *v);
GLAPI PFNGLRASTERPOS3FVPROC glad_glRasterPos3fv;
#define glRasterPos3fv glad_glRasterPos3fv
typedef void (APIENTRYP PFNGLRASTERPOS3IPROC)(GLint x, GLint y, GLint z);
GLAPI PFNGLRASTERPOS3IPROC glad_glRasterPos3i;
#define glRasterPos3i glad_glRasterPos3i
typedef void (APIENTRYP PFNGLRASTERPOS3IVPROC)(const GLint *v);
GLAPI PFNGLRASTERPOS3IVPROC glad_glRasterPos3iv;
#define glRasterPos3iv glad_glRasterPos3iv
typedef void (APIENTRYP PFNGLRASTERPOS3SPROC)(GLshort x, GLshort y, GLshort z);
GLAPI PFNGLRASTERPOS3SPROC glad_glRasterPos3s;
#define glRasterPos3s glad_glRasterPos3s
typedef void (APIENTRYP PFNGLRASTERPOS3SVPROC)(const GLshort *v);
GLAPI PFNGLRASTERPOS3SVPROC glad_glRasterPos3sv;
#define glRasterPos3sv glad_glRasterPos3sv
typedef void (APIENTRYP PFNGLRASTERPOS4DPROC)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI PFNGLRASTERPOS4DPROC glad_glRasterPos4d;
#define glRasterPos4d glad_glRasterPos4d
typedef void (APIENTRYP PFNGLRASTERPOS4DVPROC)(const GLdouble *v);
GLAPI PFNGLRASTERPOS4DVPROC glad_glRasterPos4dv;
#define glRasterPos4dv glad_glRasterPos4dv
typedef void (APIENTRYP PFNGLRASTERPOS4FPROC)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI PFNGLRASTERPOS4FPROC glad_glRasterPos4f;
#define glRasterPos4f glad_glRasterPos4f
typedef void (APIENTRYP PFNGLRASTERPOS4FVPROC)(const GLfloat *v);
GLAPI PFNGLRASTERPOS4FVPROC glad_glRasterPos4fv;
#define glRasterPos4fv glad_glRasterPos4fv
typedef void (APIENTRYP PFNGLRASTERPOS4IPROC)(GLint x, GLint y, GLint z, GLint w);
GLAPI PFNGLRASTERPOS4IPROC glad_glRasterPos4i;
#define glRasterPos4i glad_glRasterPos4i
typedef void (APIENTRYP PFNGLRASTERPOS4IVPROC)(const GLint *v);
GLAPI PFNGLRASTERPOS4IVPROC glad_glRasterPos4iv;
#define glRasterPos4iv glad_glRasterPos4iv
typedef void (APIENTRYP PFNGLRASTERPOS4SPROC)(GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI PFNGLRASTERPOS4SPROC glad_glRasterPos4s;
#define glRasterPos4s glad_glRasterPos4s
typedef void (APIENTRYP PFNGLRASTERPOS4SVPROC)(const GLshort *v);
GLAPI PFNGLRASTERPOS4SVPROC glad_glRasterPos4sv;
#define glRasterPos4sv glad_glRasterPos4sv
typedef void (APIENTRYP PFNGLRECTDPROC)(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
GLAPI PFNGLRECTDPROC glad_glRectd;
#define glRectd glad_glRectd
typedef void (APIENTRYP PFNGLRECTDVPROC)(const GLdouble *v1, const GLdouble *v2);
GLAPI PFNGLRECTDVPROC glad_glRectdv;
#define glRectdv glad_glRectdv
typedef void (APIENTRYP PFNGLRECTFPROC)(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLAPI PFNGLRECTFPROC glad_glRectf;
#define glRectf glad_glRectf
typedef void (APIENTRYP PFNGLRECTFVPROC)(const GLfloat *v1, const GLfloat *v2);
GLAPI PFNGLRECTFVPROC glad_glRectfv;
#define glRectfv glad_glRectfv
typedef void (APIENTRYP PFNGLRECTIPROC)(GLint x1, GLint y1, GLint x2, GLint y2);
GLAPI PFNGLRECTIPROC glad_glRecti;
#define glRecti glad_glRecti
typedef void (APIENTRYP PFNGLRECTIVPROC)(const GLint *v1, const GLint *v2);
GLAPI PFNGLRECTIVPROC glad_glRectiv;
#define glRectiv glad_glRectiv
typedef void (APIENTRYP PFNGLRECTSPROC)(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
GLAPI PFNGLRECTSPROC glad_glRects;
#define glRects glad_glRects
typedef void (APIENTRYP PFNGLRECTSVPROC)(const GLshort *v1, const GLshort *v2);
GLAPI PFNGLRECTSVPROC glad_glRectsv;
#define glRectsv glad_glRectsv
typedef void (APIENTRYP PFNGLTEXCOORD1DPROC)(GLdouble s);
GLAPI PFNGLTEXCOORD1DPROC glad_glTexCoord1d;
#define glTexCoord1d glad_glTexCoord1d
typedef void (APIENTRYP PFNGLTEXCOORD1DVPROC)(const GLdouble *v);
GLAPI PFNGLTEXCOORD1DVPROC glad_glTexCoord1dv;
#define glTexCoord1dv glad_glTexCoord1dv
typedef void (APIENTRYP PFNGLTEXCOORD1FPROC)(GLfloat s);
GLAPI PFNGLTEXCOORD1FPROC glad_glTexCoord1f;
#define glTexCoord1f glad_glTexCoord1f
typedef void (APIENTRYP PFNGLTEXCOORD1FVPROC)(const GLfloat *v);
GLAPI PFNGLTEXCOORD1FVPROC glad_glTexCoord1fv;
#define glTexCoord1fv glad_glTexCoord1fv
typedef void (APIENTRYP PFNGLTEXCOORD1IPROC)(GLint s);
GLAPI PFNGLTEXCOORD1IPROC glad_glTexCoord1i;
#define glTexCoord1i glad_glTexCoord1i
typedef void (APIENTRYP PFNGLTEXCOORD1IVPROC)(const GLint *v);
GLAPI PFNGLTEXCOORD1IVPROC glad_glTexCoord1iv;
#define glTexCoord1iv glad_glTexCoord1iv
typedef void (APIENTRYP PFNGLTEXCOORD1SPROC)(GLshort s);
GLAPI PFNGLTEXCOORD1SPROC glad_glTexCoord1s;
#define glTexCoord1s glad_glTexCoord1s
typedef void (APIENTRYP PFNGLTEXCOORD1SVPROC)(const GLshort *v);
GLAPI PFNGLTEXCOORD1SVPROC glad_glTexCoord1sv;
#define glTexCoord1sv glad_glTexCoord1sv
typedef void (APIENTRYP PFNGLTEXCOORD2DPROC)(GLdouble s, GLdouble t);
GLAPI PFNGLTEXCOORD2DPROC glad_glTexCoord2d;
#define glTexCoord2d glad_glTexCoord2d
typedef void (APIENTRYP PFNGLTEXCOORD2DVPROC)(const GLdouble *v);
GLAPI PFNGLTEXCOORD2DVPROC glad_glTexCoord2dv;
#define glTexCoord2dv glad_glTexCoord2dv
typedef void (APIENTRYP PFNGLTEXCOORD2FPROC)(GLfloat s, GLfloat t);
GLAPI PFNGLTEXCOORD2FPROC glad_glTexCoord2f;
#define glTexCoord2f glad_glTexCoord2f
typedef void (APIENTRYP PFNGLTEXCOORD2FVPROC)(const GLfloat *v);
GLAPI PFNGLTEXCOORD2FVPROC glad_glTexCoord2fv;
#define glTexCoord2fv glad_glTexCoord2fv
typedef void (APIENTRYP PFNGLTEXCOORD2IPROC)(GLint s, GLint t);
GLAPI PFNGLTEXCOORD2IPROC glad_glTexCoord2i;
#define glTexCoord2i glad_glTexCoord2i
typedef void (APIENTRYP PFNGLTEXCOORD2IVPROC)(const GLint *v);
GLAPI PFNGLTEXCOORD2IVPROC glad_glTexCoord2iv;
#define glTexCoord2iv glad_glTexCoord2iv
typedef void (APIENTRYP PFNGLTEXCOORD2SPROC)(GLshort s, GLshort t);
GLAPI PFNGLTEXCOORD2SPROC glad_glTexCoord2s;
#define glTexCoord2s glad_glTexCoord2s
typedef void (APIENTRYP PFNGLTEXCOORD2SVPROC)(const GLshort *v);
GLAPI PFNGLTEXCOORD2SVPROC glad_glTexCoord2sv;
#define glTexCoord2sv glad_glTexCoord2sv
typedef void (APIENTRYP PFNGLTEXCOORD3DPROC)(GLdouble s, GLdouble t, GLdouble r);
GLAPI PFNGLTEXCOORD3DPROC glad_glTexCoord3d;
#define glTexCoord3d glad_glTexCoord3d
typedef void (APIENTRYP PFNGLTEXCOORD3DVPROC)(const GLdouble *v);
GLAPI PFNGLTEXCOORD3DVPROC glad_glTexCoord3dv;
#define glTexCoord3dv glad_glTexCoord3dv
typedef void (APIENTRYP PFNGLTEXCOORD3FPROC)(GLfloat s, GLfloat t, GLfloat r);
GLAPI PFNGLTEXCOORD3FPROC glad_glTexCoord3f;
#define glTexCoord3f glad_glTexCoord3f
typedef void (APIENTRYP PFNGLTEXCOORD3FVPROC)(const GLfloat *v);
GLAPI PFNGLTEXCOORD3FVPROC glad_glTexCoord3fv;
#define glTexCoord3fv glad_glTexCoord3fv
typedef void (APIENTRYP PFNGLTEXCOORD3IPROC)(GLint s, GLint t, GLint r);
GLAPI PFNGLTEXCOORD3IPROC glad_glTexCoord3i;
#define glTexCoord3i glad_glTexCoord3i
typedef void (APIENTRYP PFNGLTEXCOORD3IVPROC)(const GLint *v);
GLAPI PFNGLTEXCOORD3IVPROC glad_glTexCoord3iv;
#define glTexCoord3iv glad_glTexCoord3iv
typedef void (APIENTRYP PFNGLTEXCOORD3SPROC)(GLshort s, GLshort t, GLshort r);
GLAPI PFNGLTEXCOORD3SPROC glad_glTexCoord3s;
#define glTexCoord3s glad_glTexCoord3s
typedef void (APIENTRYP PFNGLTEXCOORD3SVPROC)(const GLshort *v);
GLAPI PFNGLTEXCOORD3SVPROC glad_glTexCoord3sv;
#define glTexCoord3sv glad_glTexCoord3sv
typedef void (APIENTRYP PFNGLTEXCOORD4DPROC)(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLAPI PFNGLTEXCOORD4DPROC glad_glTexCoord4d;
#define glTexCoord4d glad_glTexCoord4d
typedef void (APIENTRYP PFNGLTEXCOORD4DVPROC)(const GLdouble *v);
GLAPI PFNGLTEXCOORD4DVPROC glad_glTexCoord4dv;
#define glTexCoord4dv glad_glTexCoord4dv
typedef void (APIENTRYP PFNGLTEXCOORD4FPROC)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLAPI PFNGLTEXCOORD4FPROC glad_glTexCoord4f;
#define glTexCoord4f glad_glTexCoord4f
typedef void (APIENTRYP PFNGLTEXCOORD4FVPROC)(const GLfloat *v);
GLAPI PFNGLTEXCOORD4FVPROC glad_glTexCoord4fv;
#define glTexCoord4fv glad_glTexCoord4fv
typedef void (APIENTRYP PFNGLTEXCOORD4IPROC)(GLint s, GLint t, GLint r, GLint q);
GLAPI PFNGLTEXCOORD4IPROC glad_glTexCoord4i;
#define glTexCoord4i glad_glTexCoord4i
typedef void (APIENTRYP PFNGLTEXCOORD4IVPROC)(const GLint *v);
GLAPI PFNGLTEXCOORD4IVPROC glad_glTexCoord4iv;
#define glTexCoord4iv glad_glTexCoord4iv
typedef void (APIENTRYP PFNGLTEXCOORD4SPROC)(GLshort s, GLshort t, GLshort r, GLshort q);
GLAPI PFNGLTEXCOORD4SPROC glad_glTexCoord4s;
#define glTexCoord4s glad_glTexCoord4s
typedef void (APIENTRYP PFNGLTEXCOORD4SVPROC)(const GLshort *v);
GLAPI PFNGLTEXCOORD4SVPROC glad_glTexCoord4sv;
#define glTexCoord4sv glad_glTexCoord4sv
typedef void (APIENTRYP PFNGLVERTEX2DPROC)(GLdouble x, GLdouble y);
GLAPI PFNGLVERTEX2DPROC glad_glVertex2d;
#define glVertex2d glad_glVertex2d
typedef void (APIENTRYP PFNGLVERTEX2DVPROC)(const GLdouble *v);
GLAPI PFNGLVERTEX2DVPROC glad_glVertex2dv;
#define glVertex2dv glad_glVertex2dv
typedef void (APIENTRYP PFNGLVERTEX2FPROC)(GLfloat x, GLfloat y);
GLAPI PFNGLVERTEX2FPROC glad_glVertex2f;
#define glVertex2f glad_glVertex2f
typedef void (APIENTRYP PFNGLVERTEX2FVPROC)(const GLfloat *v);
GLAPI PFNGLVERTEX2FVPROC glad_glVertex2fv;
#define glVertex2fv glad_glVertex2fv
typedef void (APIENTRYP PFNGLVERTEX2IPROC)(GLint x, GLint y);
GLAPI PFNGLVERTEX2IPROC glad_glVertex2i;
#define glVertex2i glad_glVertex2i
typedef void (APIENTRYP PFNGLVERTEX2IVPROC)(const GLint *v);
GLAPI PFNGLVERTEX2IVPROC glad_glVertex2iv;
#define glVertex2iv glad_glVertex2iv
typedef void (APIENTRYP PFNGLVERTEX2SPROC)(GLshort x, GLshort y);
GLAPI PFNGLVERTEX2SPROC glad_glVertex2s;
#define glVertex2s glad_glVertex2s
typedef void (APIENTRYP PFNGLVERTEX2SVPROC)(const GLshort *v);
GLAPI PFNGLVERTEX2SVPROC glad_glVertex2sv;
#define glVertex2sv glad_glVertex2sv
typedef void (APIENTRYP PFNGLVERTEX3DPROC)(GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLVERTEX3DPROC glad_glVertex3d;
#define glVertex3d glad_glVertex3d
typedef void (APIENTRYP PFNGLVERTEX3DVPROC)(const GLdouble *v);
GLAPI PFNGLVERTEX3DVPROC glad_glVertex3dv;
#define glVertex3dv glad_glVertex3dv
typedef void (APIENTRYP PFNGLVERTEX3FPROC)(GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLVERTEX3FPROC glad_glVertex3f;
#define glVertex3f glad_glVertex3f
typedef void (APIENTRYP PFNGLVERTEX3FVPROC)(const GLfloat *v);
GLAPI PFNGLVERTEX3FVPROC glad_glVertex3fv;
#define glVertex3fv glad_glVertex3fv
typedef void (APIENTRYP PFNGLVERTEX3IPROC)(GLint x, GLint y, GLint z);
GLAPI PFNGLVERTEX3IPROC glad_glVertex3i;
#define glVertex3i glad_glVertex3i
typedef void (APIENTRYP PFNGLVERTEX3IVPROC)(const GLint *v);
GLAPI PFNGLVERTEX3IVPROC glad_glVertex3iv;
#define glVertex3iv glad_glVertex3iv
typedef void (APIENTRYP PFNGLVERTEX3SPROC)(GLshort x, GLshort y, GLshort z);
GLAPI PFNGLVERTEX3SPROC glad_glVertex3s;
#define glVertex3s glad_glVertex3s
typedef void (APIENTRYP PFNGLVERTEX3SVPROC)(const GLshort *v);
GLAPI PFNGLVERTEX3SVPROC glad_glVertex3sv;
#define glVertex3sv glad_glVertex3sv
typedef void (APIENTRYP PFNGLVERTEX4DPROC)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI PFNGLVERTEX4DPROC glad_glVertex4d;
#define glVertex4d glad_glVertex4d
typedef void (APIENTRYP PFNGLVERTEX4DVPROC)(const GLdouble *v);
GLAPI PFNGLVERTEX4DVPROC glad_glVertex4dv;
#define glVertex4dv glad_glVertex4dv
typedef void (APIENTRYP PFNGLVERTEX4FPROC)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI PFNGLVERTEX4FPROC glad_glVertex4f;
#define glVertex4f glad_glVertex4f
typedef void (APIENTRYP PFNGLVERTEX4FVPROC)(const GLfloat *v);
GLAPI PFNGLVERTEX4FVPROC glad_glVertex4fv;
#define glVertex4fv glad_glVertex4fv
typedef void (APIENTRYP PFNGLVERTEX4IPROC)(GLint x, GLint y, GLint z, GLint w);
GLAPI PFNGLVERTEX4IPROC glad_glVertex4i;
#define glVertex4i glad_glVertex4i
typedef void (APIENTRYP PFNGLVERTEX4IVPROC)(const GLint *v);
GLAPI PFNGLVERTEX4IVPROC glad_glVertex4iv;
#define glVertex4iv glad_glVertex4iv
typedef void (APIENTRYP PFNGLVERTEX4SPROC)(GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI PFNGLVERTEX4SPROC glad_glVertex4s;
#define glVertex4s glad_glVertex4s
typedef void (APIENTRYP PFNGLVERTEX4SVPROC)(const GLshort *v);
GLAPI PFNGLVERTEX4SVPROC glad_glVertex4sv;
#define glVertex4sv glad_glVertex4sv
typedef void (APIENTRYP PFNGLCLIPPLANEPROC)(GLenum plane, const GLdouble *equation);
GLAPI PFNGLCLIPPLANEPROC glad_glClipPlane;
#define glClipPlane glad_glClipPlane
typedef void (APIENTRYP PFNGLCOLORMATERIALPROC)(GLenum face, GLenum mode);
GLAPI PFNGLCOLORMATERIALPROC glad_glColorMaterial;
#define glColorMaterial glad_glColorMaterial
typedef void (APIENTRYP PFNGLFOGFPROC)(GLenum pname, GLfloat param);
GLAPI PFNGLFOGFPROC glad_glFogf;
#define glFogf glad_glFogf
typedef void (APIENTRYP PFNGLFOGFVPROC)(GLenum pname, const GLfloat *params);
GLAPI PFNGLFOGFVPROC glad_glFogfv;
#define glFogfv glad_glFogfv
typedef void (APIENTRYP PFNGLFOGIPROC)(GLenum pname, GLint param);
GLAPI PFNGLFOGIPROC glad_glFogi;
#define glFogi glad_glFogi
typedef void (APIENTRYP PFNGLFOGIVPROC)(GLenum pname, const GLint *params);
GLAPI PFNGLFOGIVPROC glad_glFogiv;
#define glFogiv glad_glFogiv
typedef void (APIENTRYP PFNGLLIGHTFPROC)(GLenum light, GLenum pname, GLfloat param);
GLAPI PFNGLLIGHTFPROC glad_glLightf;
#define glLightf glad_glLightf
typedef void (APIENTRYP PFNGLLIGHTFVPROC)(GLenum light, GLenum pname, const GLfloat *params);
GLAPI PFNGLLIGHTFVPROC glad_glLightfv;
#define glLightfv glad_glLightfv
typedef void (APIENTRYP PFNGLLIGHTIPROC)(GLenum light, GLenum pname, GLint param);
GLAPI PFNGLLIGHTIPROC glad_glLighti;
#define glLighti glad_glLighti
typedef void (APIENTRYP PFNGLLIGHTIVPROC)(GLenum light, GLenum pname, const GLint *params);
GLAPI PFNGLLIGHTIVPROC glad_glLightiv;
#define glLightiv glad_glLightiv
typedef void (APIENTRYP PFNGLLIGHTMODELFPROC)(GLenum pname, GLfloat param);
GLAPI PFNGLLIGHTMODELFPROC glad_glLightModelf;
#define glLightModelf glad_glLightModelf
typedef void (APIENTRYP PFNGLLIGHTMODELFVPROC)(GLenum pname, const GLfloat *params);
GLAPI PFNGLLIGHTMODELFVPROC glad_glLightModelfv;
#define glLightModelfv glad_glLightModelfv
typedef void (APIENTRYP PFNGLLIGHTMODELIPROC)(GLenum pname, GLint param);
GLAPI PFNGLLIGHTMODELIPROC glad_glLightModeli;
#define glLightModeli glad_glLightModeli
typedef void (APIENTRYP PFNGLLIGHTMODELIVPROC)(GLenum pname, const GLint *params);
GLAPI PFNGLLIGHTMODELIVPROC glad_glLightModeliv;
#define glLightModeliv glad_glLightModeliv
typedef void (APIENTRYP PFNGLLINESTIPPLEPROC)(GLint factor, GLushort pattern);
GLAPI PFNGLLINESTIPPLEPROC glad_glLineStipple;
#define glLineStipple glad_glLineStipple
typedef void (APIENTRYP PFNGLMATERIALFPROC)(GLenum face, GLenum pname, GLfloat param);
GLAPI PFNGLMATERIALFPROC glad_glMaterialf;
#define glMaterialf glad_glMaterialf
typedef void (APIENTRYP PFNGLMATERIALFVPROC)(GLenum face, GLenum pname, const GLfloat *params);
GLAPI PFNGLMATERIALFVPROC glad_glMaterialfv;
#define glMaterialfv glad_glMaterialfv
typedef void (APIENTRYP PFNGLMATERIALIPROC)(GLenum face, GLenum pname, GLint param);
GLAPI PFNGLMATERIALIPROC glad_glMateriali;
#define glMateriali glad_glMateriali
typedef void (APIENTRYP PFNGLMATERIALIVPROC)(GLenum face, GLenum pname, const GLint *params);
GLAPI PFNGLMATERIALIVPROC glad_glMaterialiv;
#define glMaterialiv glad_glMaterialiv
typedef void (APIENTRYP PFNGLPOLYGONSTIPPLEPROC)(const GLubyte *mask);
GLAPI PFNGLPOLYGONSTIPPLEPROC glad_glPolygonStipple;
#define glPolygonStipple glad_glPolygonStipple
typedef void (APIENTRYP PFNGLSHADEMODELPROC)(GLenum mode);
GLAPI PFNGLSHADEMODELPROC glad_glShadeModel;
#define glShadeModel glad_glShadeModel
typedef void (APIENTRYP PFNGLTEXENVFPROC)(GLenum target, GLenum pname, GLfloat param);
GLAPI PFNGLTEXENVFPROC glad_glTexEnvf;
#define glTexEnvf glad_glTexEnvf
typedef void (APIENTRYP PFNGLTEXENVFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
GLAPI PFNGLTEXENVFVPROC glad_glTexEnvfv;
#define glTexEnvfv glad_glTexEnvfv
typedef void (APIENTRYP PFNGLTEXENVIPROC)(GLenum target, GLenum pname, GLint param);
GLAPI PFNGLTEXENVIPROC glad_glTexEnvi;
#define glTexEnvi glad_glTexEnvi
typedef void (APIENTRYP PFNGLTEXENVIVPROC)(GLenum target, GLenum pname, const GLint *params);
GLAPI PFNGLTEXENVIVPROC glad_glTexEnviv;
#define glTexEnviv glad_glTexEnviv
typedef void (APIENTRYP PFNGLTEXGENDPROC)(GLenum coord, GLenum pname, GLdouble param);
GLAPI PFNGLTEXGENDPROC glad_glTexGend;
#define glTexGend glad_glTexGend
typedef void (APIENTRYP PFNGLTEXGENDVPROC)(GLenum coord, GLenum pname, const GLdouble *params);
GLAPI PFNGLTEXGENDVPROC glad_glTexGendv;
#define glTexGendv glad_glTexGendv
typedef void (APIENTRYP PFNGLTEXGENFPROC)(GLenum coord, GLenum pname, GLfloat param);
GLAPI PFNGLTEXGENFPROC glad_glTexGenf;
#define glTexGenf glad_glTexGenf
typedef void (APIENTRYP PFNGLTEXGENFVPROC)(GLenum coord, GLenum pname, const GLfloat *params);
GLAPI PFNGLTEXGENFVPROC glad_glTexGenfv;
#define glTexGenfv glad_glTexGenfv
typedef void (APIENTRYP PFNGLTEXGENIPROC)(GLenum coord, GLenum pname, GLint param);
GLAPI PFNGLTEXGENIPROC glad_glTexGeni;
#define glTexGeni glad_glTexGeni
typedef void (APIENTRYP PFNGLTEXGENIVPROC)(GLenum coord, GLenum pname, const GLint *params);
GLAPI PFNGLTEXGENIVPROC glad_glTexGeniv;
#define glTexGeniv glad_glTexGeniv
typedef void (APIENTRYP PFNGLFEEDBACKBUFFERPROC)(GLsizei size, GLenum type, GLfloat *buffer);
GLAPI PFNGLFEEDBACKBUFFERPROC glad_glFeedbackBuffer;
#define glFeedbackBuffer glad_glFeedbackBuffer
typedef void (APIENTRYP PFNGLSELECTBUFFERPROC)(GLsizei size, GLuint *buffer);
GLAPI PFNGLSELECTBUFFERPROC glad_glSelectBuffer;
#define glSelectBuffer glad_glSelectBuffer
typedef GLint (APIENTRYP PFNGLRENDERMODEPROC)(GLenum mode);
GLAPI PFNGLRENDERMODEPROC glad_glRenderMode;
#define glRenderMode glad_glRenderMode
typedef void (APIENTRYP PFNGLINITNAMESPROC)(void);
GLAPI PFNGLINITNAMESPROC glad_glInitNames;
#define glInitNames glad_glInitNames
typedef void (APIENTRYP PFNGLLOADNAMEPROC)(GLuint name);
GLAPI PFNGLLOADNAMEPROC glad_glLoadName;
#define glLoadName glad_glLoadName
typedef void (APIENTRYP PFNGLPASSTHROUGHPROC)(GLfloat token);
GLAPI PFNGLPASSTHROUGHPROC glad_glPassThrough;
#define glPassThrough glad_glPassThrough
typedef void (APIENTRYP PFNGLPOPNAMEPROC)(void);
GLAPI PFNGLPOPNAMEPROC glad_glPopName;
#define glPopName glad_glPopName
typedef void (APIENTRYP PFNGLPUSHNAMEPROC)(GLuint name);
GLAPI PFNGLPUSHNAMEPROC glad_glPushName;
#define glPushName glad_glPushName
typedef void (APIENTRYP PFNGLCLEARACCUMPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI PFNGLCLEARACCUMPROC glad_glClearAccum;
#define glClearAccum glad_glClearAccum
typedef void (APIENTRYP PFNGLCLEARINDEXPROC)(GLfloat c);
GLAPI PFNGLCLEARINDEXPROC glad_glClearIndex;
#define glClearIndex glad_glClearIndex
typedef void (APIENTRYP PFNGLINDEXMASKPROC)(GLuint mask);
GLAPI PFNGLINDEXMASKPROC glad_glIndexMask;
#define glIndexMask glad_glIndexMask
typedef void (APIENTRYP PFNGLACCUMPROC)(GLenum op, GLfloat value);
GLAPI PFNGLACCUMPROC glad_glAccum;
#define glAccum glad_glAccum
typedef void (APIENTRYP PFNGLPOPATTRIBPROC)(void);
GLAPI PFNGLPOPATTRIBPROC glad_glPopAttrib;
#define glPopAttrib glad_glPopAttrib
typedef void (APIENTRYP PFNGLPUSHATTRIBPROC)(GLbitfield mask);
GLAPI PFNGLPUSHATTRIBPROC glad_glPushAttrib;
#define glPushAttrib glad_glPushAttrib
typedef void (APIENTRYP PFNGLMAP1DPROC)(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
GLAPI PFNGLMAP1DPROC glad_glMap1d;
#define glMap1d glad_glMap1d
typedef void (APIENTRYP PFNGLMAP1FPROC)(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
GLAPI PFNGLMAP1FPROC glad_glMap1f;
#define glMap1f glad_glMap1f
typedef void (APIENTRYP PFNGLMAP2DPROC)(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
GLAPI PFNGLMAP2DPROC glad_glMap2d;
#define glMap2d glad_glMap2d
typedef void (APIENTRYP PFNGLMAP2FPROC)(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
GLAPI PFNGLMAP2FPROC glad_glMap2f;
#define glMap2f glad_glMap2f
typedef void (APIENTRYP PFNGLMAPGRID1DPROC)(GLint un, GLdouble u1, GLdouble u2);
GLAPI PFNGLMAPGRID1DPROC glad_glMapGrid1d;
#define glMapGrid1d glad_glMapGrid1d
typedef void (APIENTRYP PFNGLMAPGRID1FPROC)(GLint un, GLfloat u1, GLfloat u2);
GLAPI PFNGLMAPGRID1FPROC glad_glMapGrid1f;
#define glMapGrid1f glad_glMapGrid1f
typedef void (APIENTRYP PFNGLMAPGRID2DPROC)(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
GLAPI PFNGLMAPGRID2DPROC glad_glMapGrid2d;
#define glMapGrid2d glad_glMapGrid2d
typedef void (APIENTRYP PFNGLMAPGRID2FPROC)(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
GLAPI PFNGLMAPGRID2FPROC glad_glMapGrid2f;
#define glMapGrid2f glad_glMapGrid2f
typedef void (APIENTRYP PFNGLEVALCOORD1DPROC)(GLdouble u);
GLAPI PFNGLEVALCOORD1DPROC glad_glEvalCoord1d;
#define glEvalCoord1d glad_glEvalCoord1d
typedef void (APIENTRYP PFNGLEVALCOORD1DVPROC)(const GLdouble *u);
GLAPI PFNGLEVALCOORD1DVPROC glad_glEvalCoord1dv;
#define glEvalCoord1dv glad_glEvalCoord1dv
typedef void (APIENTRYP PFNGLEVALCOORD1FPROC)(GLfloat u);
GLAPI PFNGLEVALCOORD1FPROC glad_glEvalCoord1f;
#define glEvalCoord1f glad_glEvalCoord1f
typedef void (APIENTRYP PFNGLEVALCOORD1FVPROC)(const GLfloat *u);
GLAPI PFNGLEVALCOORD1FVPROC glad_glEvalCoord1fv;
#define glEvalCoord1fv glad_glEvalCoord1fv
typedef void (APIENTRYP PFNGLEVALCOORD2DPROC)(GLdouble u, GLdouble v);
GLAPI PFNGLEVALCOORD2DPROC glad_glEvalCoord2d;
#define glEvalCoord2d glad_glEvalCoord2d
typedef void (APIENTRYP PFNGLEVALCOORD2DVPROC)(const GLdouble *u);
GLAPI PFNGLEVALCOORD2DVPROC glad_glEvalCoord2dv;
#define glEvalCoord2dv glad_glEvalCoord2dv
typedef void (APIENTRYP PFNGLEVALCOORD2FPROC)(GLfloat u, GLfloat v);
GLAPI PFNGLEVALCOORD2FPROC glad_glEvalCoord2f;
#define glEvalCoord2f glad_glEvalCoord2f
typedef void (APIENTRYP PFNGLEVALCOORD2FVPROC)(const GLfloat *u);
GLAPI PFNGLEVALCOORD2FVPROC glad_glEvalCoord2fv;
#define glEvalCoord2fv glad_glEvalCoord2fv
typedef void (APIENTRYP PFNGLEVALMESH1PROC)(GLenum mode, GLint i1, GLint i2);
GLAPI PFNGLEVALMESH1PROC glad_glEvalMesh1;
#define glEvalMesh1 glad_glEvalMesh1
typedef void (APIENTRYP PFNGLEVALPOINT1PROC)(GLint i);
GLAPI PFNGLEVALPOINT1PROC glad_glEvalPoint1;
#define glEvalPoint1 glad_glEvalPoint1
typedef void (APIENTRYP PFNGLEVALMESH2PROC)(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
GLAPI PFNGLEVALMESH2PROC glad_glEvalMesh2;
#define glEvalMesh2 glad_glEvalMesh2
typedef void (APIENTRYP PFNGLEVALPOINT2PROC)(GLint i, GLint j);
GLAPI PFNGLEVALPOINT2PROC glad_glEvalPoint2;
#define glEvalPoint2 glad_glEvalPoint2
typedef void (APIENTRYP PFNGLALPHAFUNCPROC)(GLenum func, GLfloat ref);
GLAPI PFNGLALPHAFUNCPROC glad_glAlphaFunc;
#define glAlphaFunc glad_glAlphaFunc
typedef void (APIENTRYP PFNGLPIXELZOOMPROC)(GLfloat xfactor, GLfloat yfactor);
GLAPI PFNGLPIXELZOOMPROC glad_glPixelZoom;
#define glPixelZoom glad_glPixelZoom
typedef void (APIENTRYP PFNGLPIXELTRANSFERFPROC)(GLenum pname, GLfloat param);
GLAPI PFNGLPIXELTRANSFERFPROC glad_glPixelTransferf;
#define glPixelTransferf glad_glPixelTransferf
typedef void (APIENTRYP PFNGLPIXELTRANSFERIPROC)(GLenum pname, GLint param);
GLAPI PFNGLPIXELTRANSFERIPROC glad_glPixelTransferi;
#define glPixelTransferi glad_glPixelTransferi
typedef void (APIENTRYP PFNGLPIXELMAPFVPROC)(GLenum map, GLsizei mapsize, const GLfloat *values);
GLAPI PFNGLPIXELMAPFVPROC glad_glPixelMapfv;
#define glPixelMapfv glad_glPixelMapfv
typedef void (APIENTRYP PFNGLPIXELMAPUIVPROC)(GLenum map, GLsizei mapsize, const GLuint *values);
GLAPI PFNGLPIXELMAPUIVPROC glad_glPixelMapuiv;
#define glPixelMapuiv glad_glPixelMapuiv
typedef void (APIENTRYP PFNGLPIXELMAPUSVPROC)(GLenum map, GLsizei mapsize, const GLushort *values);
GLAPI PFNGLPIXELMAPUSVPROC glad_glPixelMapusv;
#define glPixelMapusv glad_glPixelMapusv
typedef void (APIENTRYP PFNGLCOPYPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
GLAPI PFNGLCOPYPIXELSPROC glad_glCopyPixels;
#define glCopyPixels glad_glCopyPixels
typedef void (APIENTRYP PFNGLDRAWPIXELSPROC)(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLDRAWPIXELSPROC glad_glDrawPixels;
#define glDrawPixels glad_glDrawPixels
typedef void (APIENTRYP PFNGLGETCLIPPLANEPROC)(GLenum plane, GLdouble *equation);
GLAPI PFNGLGETCLIPPLANEPROC glad_glGetClipPlane;
#define glGetClipPlane glad_glGetClipPlane
typedef void (APIENTRYP PFNGLGETLIGHTFVPROC)(GLenum light, GLenum pname, GLfloat *params);
GLAPI PFNGLGETLIGHTFVPROC glad_glGetLightfv;
#define glGetLightfv glad_glGetLightfv
typedef void (APIENTRYP PFNGLGETLIGHTIVPROC)(GLenum light, GLenum pname, GLint *params);
GLAPI PFNGLGETLIGHTIVPROC glad_glGetLightiv;
#define glGetLightiv glad_glGetLightiv
typedef void (APIENTRYP PFNGLGETMAPDVPROC)(GLenum target, GLenum query, GLdouble *v);
GLAPI PFNGLGETMAPDVPROC glad_glGetMapdv;
#define glGetMapdv glad_glGetMapdv
typedef void (APIENTRYP PFNGLGETMAPFVPROC)(GLenum target, GLenum query, GLfloat *v);
GLAPI PFNGLGETMAPFVPROC glad_glGetMapfv;
#define glGetMapfv glad_glGetMapfv
typedef void (APIENTRYP PFNGLGETMAPIVPROC)(GLenum target, GLenum query, GLint *v);
GLAPI PFNGLGETMAPIVPROC glad_glGetMapiv;
#define glGetMapiv glad_glGetMapiv
typedef void (APIENTRYP PFNGLGETMATERIALFVPROC)(GLenum face, GLenum pname, GLfloat *params);
GLAPI PFNGLGETMATERIALFVPROC glad_glGetMaterialfv;
#define glGetMaterialfv glad_glGetMaterialfv
typedef void (APIENTRYP PFNGLGETMATERIALIVPROC)(GLenum face, GLenum pname, GLint *params);
GLAPI PFNGLGETMATERIALIVPROC glad_glGetMaterialiv;
#define glGetMaterialiv glad_glGetMaterialiv
typedef void (APIENTRYP PFNGLGETPIXELMAPFVPROC)(GLenum map, GLfloat *values);
GLAPI PFNGLGETPIXELMAPFVPROC glad_glGetPixelMapfv;
#define glGetPixelMapfv glad_glGetPixelMapfv
typedef void (APIENTRYP PFNGLGETPIXELMAPUIVPROC)(GLenum map, GLuint *values);
GLAPI PFNGLGETPIXELMAPUIVPROC glad_glGetPixelMapuiv;
#define glGetPixelMapuiv glad_glGetPixelMapuiv
typedef void (APIENTRYP PFNGLGETPIXELMAPUSVPROC)(GLenum map, GLushort *values);
GLAPI PFNGLGETPIXELMAPUSVPROC glad_glGetPixelMapusv;
#define glGetPixelMapusv glad_glGetPixelMapusv
typedef void (APIENTRYP PFNGLGETPOLYGONSTIPPLEPROC)(GLubyte *mask);
GLAPI PFNGLGETPOLYGONSTIPPLEPROC glad_glGetPolygonStipple;
#define glGetPolygonStipple glad_glGetPolygonStipple
typedef void (APIENTRYP PFNGLGETTEXENVFVPROC)(GLenum target, GLenum pname, GLfloat *params);
GLAPI PFNGLGETTEXENVFVPROC glad_glGetTexEnvfv;
#define glGetTexEnvfv glad_glGetTexEnvfv
typedef void (APIENTRYP PFNGLGETTEXENVIVPROC)(GLenum target, GLenum pname, GLint *params);
GLAPI PFNGLGETTEXENVIVPROC glad_glGetTexEnviv;
#define glGetTexEnviv glad_glGetTexEnviv
typedef void (APIENTRYP PFNGLGETTEXGENDVPROC)(GLenum coord, GLenum pname, GLdouble *params);
GLAPI PFNGLGETTEXGENDVPROC glad_glGetTexGendv;
#define glGetTexGendv glad_glGetTexGendv
typedef void (APIENTRYP PFNGLGETTEXGENFVPROC)(GLenum coord, GLenum pname, GLfloat *params);
GLAPI PFNGLGETTEXGENFVPROC glad_glGetTexGenfv;
#define glGetTexGenfv glad_glGetTexGenfv
typedef void (APIENTRYP PFNGLGETTEXGENIVPROC)(GLenum coord, GLenum pname, GLint *params);
GLAPI PFNGLGETTEXGENIVPROC glad_glGetTexGeniv;
#define glGetTexGeniv glad_glGetTexGeniv
typedef GLboolean (APIENTRYP PFNGLISLISTPROC)(GLuint list);
GLAPI PFNGLISLISTPROC glad_glIsList;
#define glIsList glad_glIsList
typedef void (APIENTRYP PFNGLFRUSTUMPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
GLAPI PFNGLFRUSTUMPROC glad_glFrustum;
#define glFrustum glad_glFrustum
typedef void (APIENTRYP PFNGLLOADIDENTITYPROC)(void);
GLAPI PFNGLLOADIDENTITYPROC glad_glLoadIdentity;
#define glLoadIdentity glad_glLoadIdentity
typedef void (APIENTRYP PFNGLLOADMATRIXFPROC)(const GLfloat *m);
GLAPI PFNGLLOADMATRIXFPROC glad_glLoadMatrixf;
#define glLoadMatrixf glad_glLoadMatrixf
typedef void (APIENTRYP PFNGLLOADMATRIXDPROC)(const GLdouble *m);
GLAPI PFNGLLOADMATRIXDPROC glad_glLoadMatrixd;
#define glLoadMatrixd glad_glLoadMatrixd
typedef void (APIENTRYP PFNGLMATRIXMODEPROC)(GLenum mode);
GLAPI PFNGLMATRIXMODEPROC glad_glMatrixMode;
#define glMatrixMode glad_glMatrixMode
typedef void (APIENTRYP PFNGLMULTMATRIXFPROC)(const GLfloat *m);
GLAPI PFNGLMULTMATRIXFPROC glad_glMultMatrixf;
#define glMultMatrixf glad_glMultMatrixf
typedef void (APIENTRYP PFNGLMULTMATRIXDPROC)(const GLdouble *m);
GLAPI PFNGLMULTMATRIXDPROC glad_glMultMatrixd;
#define glMultMatrixd glad_glMultMatrixd
typedef void (APIENTRYP PFNGLORTHOPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
GLAPI PFNGLORTHOPROC glad_glOrtho;
#define glOrtho glad_glOrtho
typedef void (APIENTRYP PFNGLPOPMATRIXPROC)(void);
GLAPI PFNGLPOPMATRIXPROC glad_glPopMatrix;
#define glPopMatrix glad_glPopMatrix
typedef void (APIENTRYP PFNGLPUSHMATRIXPROC)(void);
GLAPI PFNGLPUSHMATRIXPROC glad_glPushMatrix;
#define glPushMatrix glad_glPushMatrix
typedef void (APIENTRYP PFNGLROTATEDPROC)(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLROTATEDPROC glad_glRotated;
#define glRotated glad_glRotated
typedef void (APIENTRYP PFNGLROTATEFPROC)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLROTATEFPROC glad_glRotatef;
#define glRotatef glad_glRotatef
typedef void (APIENTRYP PFNGLSCALEDPROC)(GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLSCALEDPROC glad_glScaled;
#define glScaled glad_glScaled
typedef void (APIENTRYP PFNGLSCALEFPROC)(GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLSCALEFPROC glad_glScalef;
#define glScalef glad_glScalef
typedef void (APIENTRYP PFNGLTRANSLATEDPROC)(GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLTRANSLATEDPROC glad_glTranslated;
#define glTranslated glad_glTranslated
typedef void (APIENTRYP PFNGLTRANSLATEFPROC)(GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLTRANSLATEFPROC glad_glTranslatef;
#define glTranslatef glad_glTranslatef
#endif
#ifndef GL_VERSION_1_1
#define GL_VERSION_1_1 1
GLAPI int GLAD_GL_VERSION_1_1;
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
GLAPI PFNGLDRAWARRAYSPROC glad_glDrawArrays;
#define glDrawArrays glad_glDrawArrays
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
GLAPI PFNGLDRAWELEMENTSPROC glad_glDrawElements;
#define glDrawElements glad_glDrawElements
typedef void (APIENTRYP PFNGLGETPOINTERVPROC)(GLenum pname, void **params);
GLAPI PFNGLGETPOINTERVPROC glad_glGetPointerv;
#define glGetPointerv glad_glGetPointerv
typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC)(GLfloat factor, GLfloat units);
GLAPI PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset;
#define glPolygonOffset glad_glPolygonOffset
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
GLAPI PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D;
#define glCopyTexImage1D glad_glCopyTexImage1D
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLAPI PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D;
#define glCopyTexImage2D glad_glCopyTexImage2D
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
GLAPI PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D;
#define glCopyTexSubImage1D glad_glCopyTexSubImage1D
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D;
#define glCopyTexSubImage2D glad_glCopyTexSubImage2D
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D;
#define glTexSubImage1D glad_glTexSubImage1D
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;
#define glTexSubImage2D glad_glTexSubImage2D
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
GLAPI PFNGLBINDTEXTUREPROC glad_glBindTexture;
#define glBindTexture glad_glBindTexture
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
GLAPI PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
#define glDeleteTextures glad_glDeleteTextures
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
GLAPI PFNGLGENTEXTURESPROC glad_glGenTextures;
#define glGenTextures glad_glGenTextures
typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC)(GLuint texture);
GLAPI PFNGLISTEXTUREPROC glad_glIsTexture;
#define glIsTexture glad_glIsTexture
typedef void (APIENTRYP PFNGLARRAYELEMENTPROC)(GLint i);
GLAPI PFNGLARRAYELEMENTPROC glad_glArrayElement;
#define glArrayElement glad_glArrayElement
typedef void (APIENTRYP PFNGLCOLORPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLCOLORPOINTERPROC glad_glColorPointer;
#define glColorPointer glad_glColorPointer
typedef void (APIENTRYP PFNGLDISABLECLIENTSTATEPROC)(GLenum array);
GLAPI PFNGLDISABLECLIENTSTATEPROC glad_glDisableClientState;
#define glDisableClientState glad_glDisableClientState
typedef void (APIENTRYP PFNGLEDGEFLAGPOINTERPROC)(GLsizei stride, const void *pointer);
GLAPI PFNGLEDGEFLAGPOINTERPROC glad_glEdgeFlagPointer;
#define glEdgeFlagPointer glad_glEdgeFlagPointer
typedef void (APIENTRYP PFNGLENABLECLIENTSTATEPROC)(GLenum array);
GLAPI PFNGLENABLECLIENTSTATEPROC glad_glEnableClientState;
#define glEnableClientState glad_glEnableClientState
typedef void (APIENTRYP PFNGLINDEXPOINTERPROC)(GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLINDEXPOINTERPROC glad_glIndexPointer;
#define glIndexPointer glad_glIndexPointer
typedef void (APIENTRYP PFNGLINTERLEAVEDARRAYSPROC)(GLenum format, GLsizei stride, const void *pointer);
GLAPI PFNGLINTERLEAVEDARRAYSPROC glad_glInterleavedArrays;
#define glInterleavedArrays glad_glInterleavedArrays
typedef void (APIENTRYP PFNGLNORMALPOINTERPROC)(GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLNORMALPOINTERPROC glad_glNormalPointer;
#define glNormalPointer glad_glNormalPointer
typedef void (APIENTRYP PFNGLTEXCOORDPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLTEXCOORDPOINTERPROC glad_glTexCoordPointer;
#define glTexCoordPointer glad_glTexCoordPointer
typedef void (APIENTRYP PFNGLVERTEXPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLVERTEXPOINTERPROC glad_glVertexPointer;
#define glVertexPointer glad_glVertexPointer
typedef GLboolean (APIENTRYP PFNGLARETEXTURESRESIDENTPROC)(GLsizei n, const GLuint *textures, GLboolean *residences);
GLAPI PFNGLARETEXTURESRESIDENTPROC glad_glAreTexturesResident;
#define glAreTexturesResident glad_glAreTexturesResident
typedef void (APIENTRYP PFNGLPRIORITIZETEXTURESPROC)(GLsizei n, const GLuint *textures, const GLfloat *priorities);
GLAPI PFNGLPRIORITIZETEXTURESPROC glad_glPrioritizeTextures;
#define glPrioritizeTextures glad_glPrioritizeTextures
typedef void (APIENTRYP PFNGLINDEXUBPROC)(GLubyte c);
GLAPI PFNGLINDEXUBPROC glad_glIndexub;
#define glIndexub glad_glIndexub
typedef void (APIENTRYP PFNGLINDEXUBVPROC)(const GLubyte *c);
GLAPI PFNGLINDEXUBVPROC glad_glIndexubv;
#define glIndexubv glad_glIndexubv
typedef void (APIENTRYP PFNGLPOPCLIENTATTRIBPROC)(void);
GLAPI PFNGLPOPCLIENTATTRIBPROC glad_glPopClientAttrib;
#define glPopClientAttrib glad_glPopClientAttrib
typedef void (APIENTRYP PFNGLPUSHCLIENTATTRIBPROC)(GLbitfield mask);
GLAPI PFNGLPUSHCLIENTATTRIBPROC glad_glPushClientAttrib;
#define glPushClientAttrib glad_glPushClientAttrib
#endif
#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
GLAPI int GLAD_GL_VERSION_1_2;
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
GLAPI PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements;
#define glDrawRangeElements glad_glDrawRangeElements
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXIMAGE3DPROC glad_glTexImage3D;
#define glTexImage3D glad_glTexImage3D
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D;
#define glTexSubImage3D glad_glTexSubImage3D
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D;
#define glCopyTexSubImage3D glad_glCopyTexSubImage3D
#endif
#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3 1
GLAPI int GLAD_GL_VERSION_1_3;
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
GLAPI PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
#define glActiveTexture glad_glActiveTexture
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC)(GLfloat value, GLboolean invert);
GLAPI PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage;
#define glSampleCoverage glad_glSampleCoverage
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
GLAPI PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D;
#define glCompressedTexImage3D glad_glCompressedTexImage3D
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D;
#define glCompressedTexImage2D glad_glCompressedTexImage2D
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
GLAPI PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D;
#define glCompressedTexImage1D glad_glCompressedTexImage1D
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D;
#define glCompressedTexSubImage3D glad_glCompressedTexSubImage3D
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D;
#define glCompressedTexSubImage2D glad_glCompressedTexSubImage2D
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D;
#define glCompressedTexSubImage1D glad_glCompressedTexSubImage1D
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC)(GLenum target, GLint level, void *img);
GLAPI PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage;
#define glGetCompressedTexImage glad_glGetCompressedTexImage
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREPROC)(GLenum texture);
GLAPI PFNGLCLIENTACTIVETEXTUREPROC glad_glClientActiveTexture;
#define glClientActiveTexture glad_glClientActiveTexture
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DPROC)(GLenum target, GLdouble s);
GLAPI PFNGLMULTITEXCOORD1DPROC glad_glMultiTexCoord1d;
#define glMultiTexCoord1d glad_glMultiTexCoord1d
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DVPROC)(GLenum target, const GLdouble *v);
GLAPI PFNGLMULTITEXCOORD1DVPROC glad_glMultiTexCoord1dv;
#define glMultiTexCoord1dv glad_glMultiTexCoord1dv
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FPROC)(GLenum target, GLfloat s);
GLAPI PFNGLMULTITEXCOORD1FPROC glad_glMultiTexCoord1f;
#define glMultiTexCoord1f glad_glMultiTexCoord1f
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVPROC)(GLenum target, const GLfloat *v);
GLAPI PFNGLMULTITEXCOORD1FVPROC glad_glMultiTexCoord1fv;
#define glMultiTexCoord1fv glad_glMultiTexCoord1fv
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IPROC)(GLenum target, GLint s);
GLAPI PFNGLMULTITEXCOORD1IPROC glad_glMultiTexCoord1i;
#define glMultiTexCoord1i glad_glMultiTexCoord1i
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IVPROC)(GLenum target, const GLint *v);
GLAPI PFNGLMULTITEXCOORD1IVPROC glad_glMultiTexCoord1iv;
#define glMultiTexCoord1iv glad_glMultiTexCoord1iv
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SPROC)(GLenum target, GLshort s);
GLAPI PFNGLMULTITEXCOORD1SPROC glad_glMultiTexCoord1s;
#define glMultiTexCoord1s glad_glMultiTexCoord1s
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SVPROC)(GLenum target, const GLshort *v);
GLAPI PFNGLMULTITEXCOORD1SVPROC glad_glMultiTexCoord1sv;
#define glMultiTexCoord1sv glad_glMultiTexCoord1sv
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DPROC)(GLenum target, GLdouble s, GLdouble t);
GLAPI PFNGLMULTITEXCOORD2DPROC glad_glMultiTexCoord2d;
#define glMultiTexCoord2d glad_glMultiTexCoord2d
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DVPROC)(GLenum target, const GLdouble *v);
GLAPI PFNGLMULTITEXCOORD2DVPROC glad_glMultiTexCoord2dv;
#define glMultiTexCoord2dv glad_glMultiTexCoord2dv
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC)(GLenum target, GLfloat s, GLfloat t);
GLAPI PFNGLMULTITEXCOORD2FPROC glad_glMultiTexCoord2f;
#define glMultiTexCoord2f glad_glMultiTexCoord2f
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVPROC)(GLenum target, const GLfloat *v);
GLAPI PFNGLMULTITEXCOORD2FVPROC glad_glMultiTexCoord2fv;
#define glMultiTexCoord2fv glad_glMultiTexCoord2fv
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IPROC)(GLenum target, GLint s, GLint t);
GLAPI PFNGLMULTITEXCOORD2IPROC glad_glMultiTexCoord2i;
#define glMultiTexCoord2i glad_glMultiTexCoord2i
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IVPROC)(GLenum target, const GLint *v);
GLAPI PFNGLMULTITEXCOORD2IVPROC glad_glMultiTexCoord2iv;
#define glMultiTexCoord2iv glad_glMultiTexCoord2iv
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SPROC)(GLenum target, GLshort s, GLshort t);
GLAPI PFNGLMULTITEXCOORD2SPROC glad_glMultiTexCoord2s;
#define glMultiTexCoord2s glad_glMultiTexCoord2s
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SVPROC)(GLenum target, const GLshort *v);
GLAPI PFNGLMULTITEXCOORD2SVPROC glad_glMultiTexCoord2sv;
#define glMultiTexCoord2sv glad_glMultiTexCoord2sv
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r);
GLAPI PFNGLMULTITEXCOORD3DPROC glad_glMultiTexCoord3d;
#define glMultiTexCoord3d glad_glMultiTexCoord3d
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DVPROC)(GLenum target, const GLdouble *v);
GLAPI PFNGLMULTITEXCOORD3DVPROC glad_glMultiTexCoord3dv;
#define glMultiTexCoord3dv glad_glMultiTexCoord3dv
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r);
GLAPI PFNGLMULTITEXCOORD3FPROC glad_glMultiTexCoord3f;
#define glMultiTexCoord3f glad_glMultiTexCoord3f
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVPROC)(GLenum target, const GLfloat *v);
GLAPI PFNGLMULTITEXCOORD3FVPROC glad_glMultiTexCoord3fv;
#define glMultiTexCoord3fv glad_glMultiTexCoord3fv
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IPROC)(GLenum target, GLint s, GLint t, GLint r);
GLAPI PFNGLMULTITEXCOORD3IPROC glad_glMultiTexCoord3i;
#define glMultiTexCoord3i glad_glMultiTexCoord3i
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IVPROC)(GLenum target, const GLint *v);
GLAPI PFNGLMULTITEXCOORD3IVPROC glad_glMultiTexCoord3iv;
#define glMultiTexCoord3iv glad_glMultiTexCoord3iv
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SPROC)(GLenum target, GLshort s, GLshort t, GLshort r);
GLAPI PFNGLMULTITEXCOORD3SPROC glad_glMultiTexCoord3s;
#define glMultiTexCoord3s glad_glMultiTexCoord3s
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SVPROC)(GLenum target, const GLshort *v);
GLAPI PFNGLMULTITEXCOORD3SVPROC glad_glMultiTexCoord3sv;
#define glMultiTexCoord3sv glad_glMultiTexCoord3sv
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLAPI PFNGLMULTITEXCOORD4DPROC glad_glMultiTexCoord4d;
#define glMultiTexCoord4d glad_glMultiTexCoord4d
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DVPROC)(GLenum target, const GLdouble *v);
GLAPI PFNGLMULTITEXCOORD4DVPROC glad_glMultiTexCoord4dv;
#define glMultiTexCoord4dv glad_glMultiTexCoord4dv
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLAPI PFNGLMULTITEXCOORD4FPROC glad_glMultiTexCoord4f;
#define glMultiTexCoord4f glad_glMultiTexCoord4f
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVPROC)(GLenum target, const GLfloat *v);
GLAPI PFNGLMULTITEXCOORD4FVPROC glad_glMultiTexCoord4fv;
#define glMultiTexCoord4fv glad_glMultiTexCoord4fv
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IPROC)(GLenum target, GLint s, GLint t, GLint r, GLint q);
GLAPI PFNGLMULTITEXCOORD4IPROC glad_glMultiTexCoord4i;
#define glMultiTexCoord4i glad_glMultiTexCoord4i
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IVPROC)(GLenum target, const GLint *v);
GLAPI PFNGLMULTITEXCOORD4IVPROC glad_glMultiTexCoord4iv;
#define glMultiTexCoord4iv glad_glMultiTexCoord4iv
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SPROC)(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
GLAPI PFNGLMULTITEXCOORD4SPROC glad_glMultiTexCoord4s;
#define glMultiTexCoord4s glad_glMultiTexCoord4s
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SVPROC)(GLenum target, const GLshort *v);
GLAPI PFNGLMULTITEXCOORD4SVPROC glad_glMultiTexCoord4sv;
#define glMultiTexCoord4sv glad_glMultiTexCoord4sv
typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXFPROC)(const GLfloat *m);
GLAPI PFNGLLOADTRANSPOSEMATRIXFPROC glad_glLoadTransposeMatrixf;
#define glLoadTransposeMatrixf glad_glLoadTransposeMatrixf
typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXDPROC)(const GLdouble *m);
GLAPI PFNGLLOADTRANSPOSEMATRIXDPROC glad_glLoadTransposeMatrixd;
#define glLoadTransposeMatrixd glad_glLoadTransposeMatrixd
typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXFPROC)(const GLfloat *m);
GLAPI PFNGLMULTTRANSPOSEMATRIXFPROC glad_glMultTransposeMatrixf;
#define glMultTransposeMatrixf glad_glMultTransposeMatrixf
typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXDPROC)(const GLdouble *m);
GLAPI PFNGLMULTTRANSPOSEMATRIXDPROC glad_glMultTransposeMatrixd;
#define glMultTransposeMatrixd glad_glMultTransposeMatrixd
#endif
#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
GLAPI int GLAD_GL_VERSION_1_4;
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
GLAPI PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate;
#define glBlendFuncSeparate glad_glBlendFuncSeparate
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
GLAPI PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays;
#define glMultiDrawArrays glad_glMultiDrawArrays
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
GLAPI PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements;
#define glMultiDrawElements glad_glMultiDrawElements
typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC)(GLenum pname, GLfloat param);
GLAPI PFNGLPOINTPARAMETERFPROC glad_glPointParameterf;
#define glPointParameterf glad_glPointParameterf
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC)(GLenum pname, const GLfloat *params);
GLAPI PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv;
#define glPointParameterfv glad_glPointParameterfv
typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC)(GLenum pname, GLint param);
GLAPI PFNGLPOINTPARAMETERIPROC glad_glPointParameteri;
#define glPointParameteri glad_glPointParameteri
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC)(GLenum pname, const GLint *params);
GLAPI PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv;
#define glPointParameteriv glad_glPointParameteriv
typedef void (APIENTRYP PFNGLFOGCOORDFPROC)(GLfloat coord);
GLAPI PFNGLFOGCOORDFPROC glad_glFogCoordf;
#define glFogCoordf glad_glFogCoordf
typedef void (APIENTRYP PFNGLFOGCOORDFVPROC)(const GLfloat *coord);
GLAPI PFNGLFOGCOORDFVPROC glad_glFogCoordfv;
#define glFogCoordfv glad_glFogCoordfv
typedef void (APIENTRYP PFNGLFOGCOORDDPROC)(GLdouble coord);
GLAPI PFNGLFOGCOORDDPROC glad_glFogCoordd;
#define glFogCoordd glad_glFogCoordd
typedef void (APIENTRYP PFNGLFOGCOORDDVPROC)(const GLdouble *coord);
GLAPI PFNGLFOGCOORDDVPROC glad_glFogCoorddv;
#define glFogCoorddv glad_glFogCoorddv
typedef void (APIENTRYP PFNGLFOGCOORDPOINTERPROC)(GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLFOGCOORDPOINTERPROC glad_glFogCoordPointer;
#define glFogCoordPointer glad_glFogCoordPointer
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BPROC)(GLbyte red, GLbyte green, GLbyte blue);
GLAPI PFNGLSECONDARYCOLOR3BPROC glad_glSecondaryColor3b;
#define glSecondaryColor3b glad_glSecondaryColor3b
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BVPROC)(const GLbyte *v);
GLAPI PFNGLSECONDARYCOLOR3BVPROC glad_glSecondaryColor3bv;
#define glSecondaryColor3bv glad_glSecondaryColor3bv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DPROC)(GLdouble red, GLdouble green, GLdouble blue);
GLAPI PFNGLSECONDARYCOLOR3DPROC glad_glSecondaryColor3d;
#define glSecondaryColor3d glad_glSecondaryColor3d
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DVPROC)(const GLdouble *v);
GLAPI PFNGLSECONDARYCOLOR3DVPROC glad_glSecondaryColor3dv;
#define glSecondaryColor3dv glad_glSecondaryColor3dv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FPROC)(GLfloat red, GLfloat green, GLfloat blue);
GLAPI PFNGLSECONDARYCOLOR3FPROC glad_glSecondaryColor3f;
#define glSecondaryColor3f glad_glSecondaryColor3f
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FVPROC)(const GLfloat *v);
GLAPI PFNGLSECONDARYCOLOR3FVPROC glad_glSecondaryColor3fv;
#define glSecondaryColor3fv glad_glSecondaryColor3fv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IPROC)(GLint red, GLint green, GLint blue);
GLAPI PFNGLSECONDARYCOLOR3IPROC glad_glSecondaryColor3i;
#define glSecondaryColor3i glad_glSecondaryColor3i
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IVPROC)(const GLint *v);
GLAPI PFNGLSECONDARYCOLOR3IVPROC glad_glSecondaryColor3iv;
#define glSecondaryColor3iv glad_glSecondaryColor3iv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SPROC)(GLshort red, GLshort green, GLshort blue);
GLAPI PFNGLSECONDARYCOLOR3SPROC glad_glSecondaryColor3s;
#define glSecondaryColor3s glad_glSecondaryColor3s
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SVPROC)(const GLshort *v);
GLAPI PFNGLSECONDARYCOLOR3SVPROC glad_glSecondaryColor3sv;
#define glSecondaryColor3sv glad_glSecondaryColor3sv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBPROC)(GLubyte red, GLubyte green, GLubyte blue);
GLAPI PFNGLSECONDARYCOLOR3UBPROC glad_glSecondaryColor3ub;
#define glSecondaryColor3ub glad_glSecondaryColor3ub
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBVPROC)(const GLubyte *v);
GLAPI PFNGLSECONDARYCOLOR3UBVPROC glad_glSecondaryColor3ubv;
#define glSecondaryColor3ubv glad_glSecondaryColor3ubv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIPROC)(GLuint red, GLuint green, GLuint blue);
GLAPI PFNGLSECONDARYCOLOR3UIPROC glad_glSecondaryColor3ui;
#define glSecondaryColor3ui glad_glSecondaryColor3ui
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIVPROC)(const GLuint *v);
GLAPI PFNGLSECONDARYCOLOR3UIVPROC glad_glSecondaryColor3uiv;
#define glSecondaryColor3uiv glad_glSecondaryColor3uiv
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USPROC)(GLushort red, GLushort green, GLushort blue);
GLAPI PFNGLSECONDARYCOLOR3USPROC glad_glSecondaryColor3us;
#define glSecondaryColor3us glad_glSecondaryColor3us
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USVPROC)(const GLushort *v);
GLAPI PFNGLSECONDARYCOLOR3USVPROC glad_glSecondaryColor3usv;
#define glSecondaryColor3usv glad_glSecondaryColor3usv
typedef void (APIENTRYP PFNGLSECONDARYCOLORPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLSECONDARYCOLORPOINTERPROC glad_glSecondaryColorPointer;
#define glSecondaryColorPointer glad_glSecondaryColorPointer
typedef void (APIENTRYP PFNGLWINDOWPOS2DPROC)(GLdouble x, GLdouble y);
GLAPI PFNGLWINDOWPOS2DPROC glad_glWindowPos2d;
#define glWindowPos2d glad_glWindowPos2d
typedef void (APIENTRYP PFNGLWINDOWPOS2DVPROC)(const GLdouble *v);
GLAPI PFNGLWINDOWPOS2DVPROC glad_glWindowPos2dv;
#define glWindowPos2dv glad_glWindowPos2dv
typedef void (APIENTRYP PFNGLWINDOWPOS2FPROC)(GLfloat x, GLfloat y);
GLAPI PFNGLWINDOWPOS2FPROC glad_glWindowPos2f;
#define glWindowPos2f glad_glWindowPos2f
typedef void (APIENTRYP PFNGLWINDOWPOS2FVPROC)(const GLfloat *v);
GLAPI PFNGLWINDOWPOS2FVPROC glad_glWindowPos2fv;
#define glWindowPos2fv glad_glWindowPos2fv
typedef void (APIENTRYP PFNGLWINDOWPOS2IPROC)(GLint x, GLint y);
GLAPI PFNGLWINDOWPOS2IPROC glad_glWindowPos2i;
#define glWindowPos2i glad_glWindowPos2i
typedef void (APIENTRYP PFNGLWINDOWPOS2IVPROC)(const GLint *v);
GLAPI PFNGLWINDOWPOS2IVPROC glad_glWindowPos2iv;
#define glWindowPos2iv glad_glWindowPos2iv
typedef void (APIENTRYP PFNGLWINDOWPOS2SPROC)(GLshort x, GLshort y);
GLAPI PFNGLWINDOWPOS2SPROC glad_glWindowPos2s;
#define glWindowPos2s glad_glWindowPos2s
typedef void (APIENTRYP PFNGLWINDOWPOS2SVPROC)(const GLshort *v);
GLAPI PFNGLWINDOWPOS2SVPROC glad_glWindowPos2sv;
#define glWindowPos2sv glad_glWindowPos2sv
typedef void (APIENTRYP PFNGLWINDOWPOS3DPROC)(GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLWINDOWPOS3DPROC glad_glWindowPos3d;
#define glWindowPos3d glad_glWindowPos3d
typedef void (APIENTRYP PFNGLWINDOWPOS3DVPROC)(const GLdouble *v);
GLAPI PFNGLWINDOWPOS3DVPROC glad_glWindowPos3dv;
#define glWindowPos3dv glad_glWindowPos3dv
typedef void (APIENTRYP PFNGLWINDOWPOS3FPROC)(GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLWINDOWPOS3FPROC glad_glWindowPos3f;
#define glWindowPos3f glad_glWindowPos3f
typedef void (APIENTRYP PFNGLWINDOWPOS3FVPROC)(const GLfloat *v);
GLAPI PFNGLWINDOWPOS3FVPROC glad_glWindowPos3fv;
#define glWindowPos3fv glad_glWindowPos3fv
typedef void (APIENTRYP PFNGLWINDOWPOS3IPROC)(GLint x, GLint y, GLint z);
GLAPI PFNGLWINDOWPOS3IPROC glad_glWindowPos3i;
#define glWindowPos3i glad_glWindowPos3i
typedef void (APIENTRYP PFNGLWINDOWPOS3IVPROC)(const GLint *v);
GLAPI PFNGLWINDOWPOS3IVPROC glad_glWindowPos3iv;
#define glWindowPos3iv glad_glWindowPos3iv
typedef void (APIENTRYP PFNGLWINDOWPOS3SPROC)(GLshort x, GLshort y, GLshort z);
GLAPI PFNGLWINDOWPOS3SPROC glad_glWindowPos3s;
#define glWindowPos3s glad_glWindowPos3s
typedef void (APIENTRYP PFNGLWINDOWPOS3SVPROC)(const GLshort *v);
GLAPI PFNGLWINDOWPOS3SVPROC glad_glWindowPos3sv;
#define glWindowPos3sv glad_glWindowPos3sv
typedef void (APIENTRYP PFNGLBLENDCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI PFNGLBLENDCOLORPROC glad_glBlendColor;
#define glBlendColor glad_glBlendColor
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
GLAPI PFNGLBLENDEQUATIONPROC glad_glBlendEquation;
#define glBlendEquation glad_glBlendEquation
#endif
#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5 1
GLAPI int GLAD_GL_VERSION_1_5;
typedef void (APIENTRYP PFNGLGENQUERIESPROC)(GLsizei n, GLuint *ids);
GLAPI PFNGLGENQUERIESPROC glad_glGenQueries;
#define glGenQueries glad_glGenQueries
typedef void (APIENTRYP PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint *ids);
GLAPI PFNGLDELETEQUERIESPROC glad_glDeleteQueries;
#define glDeleteQueries glad_glDeleteQueries
typedef GLboolean (APIENTRYP PFNGLISQUERYPROC)(GLuint id);
GLAPI PFNGLISQUERYPROC glad_glIsQuery;
#define glIsQuery glad_glIsQuery
typedef void (APIENTRYP PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
GLAPI PFNGLBEGINQUERYPROC glad_glBeginQuery;
#define glBeginQuery glad_glBeginQuery
typedef void (APIENTRYP PFNGLENDQUERYPROC)(GLenum target);
GLAPI PFNGLENDQUERYPROC glad_glEndQuery;
#define glEndQuery glad_glEndQuery
typedef void (APIENTRYP PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint *params);
GLAPI PFNGLGETQUERYIVPROC glad_glGetQueryiv;
#define glGetQueryiv glad_glGetQueryiv
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint *params);
GLAPI PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv;
#define glGetQueryObjectiv glad_glGetQueryObjectiv
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint *params);
GLAPI PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv;
#define glGetQueryObjectuiv glad_glGetQueryObjectuiv
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
GLAPI PFNGLBINDBUFFERPROC glad_glBindBuffer;
#define glBindBuffer glad_glBindBuffer
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
GLAPI PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
#define glDeleteBuffers glad_glDeleteBuffers
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
GLAPI PFNGLGENBUFFERSPROC glad_glGenBuffers;
#define glGenBuffers glad_glGenBuffers
typedef GLboolean (APIENTRYP PFNGLISBUFFERPROC)(GLuint buffer);
GLAPI PFNGLISBUFFERPROC glad_glIsBuffer;
#define glIsBuffer glad_glIsBuffer
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
GLAPI PFNGLBUFFERDATAPROC glad_glBufferData;
#define glBufferData glad_glBufferData
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
GLAPI PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
#define glBufferSubData glad_glBufferSubData
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
GLAPI PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData;
#define glGetBufferSubData glad_glGetBufferSubData
typedef void * (APIENTRYP PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
GLAPI PFNGLMAPBUFFERPROC glad_glMapBuffer;
#define glMapBuffer glad_glMapBuffer
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC)(GLenum target);
GLAPI PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer;
#define glUnmapBuffer glad_glUnmapBuffer
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
GLAPI PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv;
#define glGetBufferParameteriv glad_glGetBufferParameteriv
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVPROC)(GLenum target, GLenum pname, void **params);
GLAPI PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv;
#define glGetBufferPointerv glad_glGetBufferPointerv
#endif
#ifndef GL_VERSION_2_0
#define GL_VERSION_2_0 1
GLAPI int GLAD_GL_VERSION_2_0;
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
GLAPI PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate;
#define glBlendEquationSeparate glad_glBlendEquationSeparate
typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum *bufs);
GLAPI PFNGLDRAWBUFFERSPROC glad_glDrawBuffers;
#define glDrawBuffers glad_glDrawBuffers
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
GLAPI PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate;
#define glStencilOpSeparate glad_glStencilOpSeparate
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC)(GLenum face, GLenum func, GLint ref, GLuint mask);
GLAPI PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate;
#define glStencilFuncSeparate glad_glStencilFuncSeparate
typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC)(GLenum face, GLuint mask);
GLAPI PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate;
#define glStencilMaskSeparate glad_glStencilMaskSeparate
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
GLAPI PFNGLATTACHSHADERPROC glad_glAttachShader;
#define glAttachShader glad_glAttachShader
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar *name);
GLAPI PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation;
#define glBindAttribLocation glad_glBindAttribLocation
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
GLAPI PFNGLCOMPILESHADERPROC glad_glCompileShader;
#define glCompileShader glad_glCompileShader
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
GLAPI PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
#define glCreateProgram glad_glCreateProgram
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
GLAPI PFNGLCREATESHADERPROC glad_glCreateShader;
#define glCreateShader glad_glCreateShader
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
GLAPI PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
#define glDeleteProgram glad_glDeleteProgram
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
GLAPI PFNGLDELETESHADERPROC glad_glDeleteShader;
#define glDeleteShader glad_glDeleteShader
typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
GLAPI PFNGLDETACHSHADERPROC glad_glDetachShader;
#define glDetachShader glad_glDetachShader
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
#define glDisableVertexAttribArray glad_glDisableVertexAttribArray
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
#define glEnableVertexAttribArray glad_glEnableVertexAttribArray
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
GLAPI PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib;
#define glGetActiveAttrib glad_glGetActiveAttrib
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
GLAPI PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform;
#define glGetActiveUniform glad_glGetActiveUniform
typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
GLAPI PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders;
#define glGetAttachedShaders glad_glGetAttachedShaders
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar *name);
GLAPI PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation;
#define glGetAttribLocation glad_glGetAttribLocation
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
GLAPI PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
#define glGetProgramiv glad_glGetProgramiv
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GLAPI PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
#define glGetProgramInfoLog glad_glGetProgramInfoLog
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
GLAPI PFNGLGETSHADERIVPROC glad_glGetShaderiv;
#define glGetShaderiv glad_glGetShaderiv
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GLAPI PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
#define glGetShaderInfoLog glad_glGetShaderInfoLog
typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
GLAPI PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource;
#define glGetShaderSource glad_glGetShaderSource
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
#define glGetUniformLocation glad_glGetUniformLocation
typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC)(GLuint program, GLint location, GLfloat *params);
GLAPI PFNGLGETUNIFORMFVPROC glad_glGetUniformfv;
#define glGetUniformfv glad_glGetUniformfv
typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC)(GLuint program, GLint location, GLint *params);
GLAPI PFNGLGETUNIFORMIVPROC glad_glGetUniformiv;
#define glGetUniformiv glad_glGetUniformiv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVPROC)(GLuint index, GLenum pname, GLdouble *params);
GLAPI PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv;
#define glGetVertexAttribdv glad_glGetVertexAttribdv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC)(GLuint index, GLenum pname, GLfloat *params);
GLAPI PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv;
#define glGetVertexAttribfv glad_glGetVertexAttribfv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(GLuint index, GLenum pname, GLint *params);
GLAPI PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv;
#define glGetVertexAttribiv glad_glGetVertexAttribiv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint index, GLenum pname, void **pointer);
GLAPI PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv;
#define glGetVertexAttribPointerv glad_glGetVertexAttribPointerv
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPROC)(GLuint program);
GLAPI PFNGLISPROGRAMPROC glad_glIsProgram;
#define glIsProgram glad_glIsProgram
typedef GLboolean (APIENTRYP PFNGLISSHADERPROC)(GLuint shader);
GLAPI PFNGLISSHADERPROC glad_glIsShader;
#define glIsShader glad_glIsShader
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
GLAPI PFNGLLINKPROGRAMPROC glad_glLinkProgram;
#define glLinkProgram glad_glLinkProgram
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
GLAPI PFNGLSHADERSOURCEPROC glad_glShaderSource;
#define glShaderSource glad_glShaderSource
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
GLAPI PFNGLUSEPROGRAMPROC glad_glUseProgram;
#define glUseProgram glad_glUseProgram
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
GLAPI PFNGLUNIFORM1FPROC glad_glUniform1f;
#define glUniform1f glad_glUniform1f
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
GLAPI PFNGLUNIFORM2FPROC glad_glUniform2f;
#define glUniform2f glad_glUniform2f
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLAPI PFNGLUNIFORM3FPROC glad_glUniform3f;
#define glUniform3f glad_glUniform3f
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLAPI PFNGLUNIFORM4FPROC glad_glUniform4f;
#define glUniform4f glad_glUniform4f
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
GLAPI PFNGLUNIFORM1IPROC glad_glUniform1i;
#define glUniform1i glad_glUniform1i
typedef void (APIENTRYP PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
GLAPI PFNGLUNIFORM2IPROC glad_glUniform2i;
#define glUniform2i glad_glUniform2i
typedef void (APIENTRYP PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
GLAPI PFNGLUNIFORM3IPROC glad_glUniform3i;
#define glUniform3i glad_glUniform3i
typedef void (APIENTRYP PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
GLAPI PFNGLUNIFORM4IPROC glad_glUniform4i;
#define glUniform4i glad_glUniform4i
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat *value);
GLAPI PFNGLUNIFORM1FVPROC glad_glUniform1fv;
#define glUniform1fv glad_glUniform1fv
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat *value);
GLAPI PFNGLUNIFORM2FVPROC glad_glUniform2fv;
#define glUniform2fv glad_glUniform2fv
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat *value);
GLAPI PFNGLUNIFORM3FVPROC glad_glUniform3fv;
#define glUniform3fv glad_glUniform3fv
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat *value);
GLAPI PFNGLUNIFORM4FVPROC glad_glUniform4fv;
#define glUniform4fv glad_glUniform4fv
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint *value);
GLAPI PFNGLUNIFORM1IVPROC glad_glUniform1iv;
#define glUniform1iv glad_glUniform1iv
typedef void (APIENTRYP PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint *value);
GLAPI PFNGLUNIFORM2IVPROC glad_glUniform2iv;
#define glUniform2iv glad_glUniform2iv
typedef void (APIENTRYP PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint *value);
GLAPI PFNGLUNIFORM3IVPROC glad_glUniform3iv;
#define glUniform3iv glad_glUniform3iv
typedef void (APIENTRYP PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint *value);
GLAPI PFNGLUNIFORM4IVPROC glad_glUniform4iv;
#define glUniform4iv glad_glUniform4iv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv;
#define glUniformMatrix2fv glad_glUniformMatrix2fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv;
#define glUniformMatrix3fv glad_glUniformMatrix3fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
#define glUniformMatrix4fv glad_glUniformMatrix4fv
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint program);
GLAPI PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram;
#define glValidateProgram glad_glValidateProgram
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DPROC)(GLuint index, GLdouble x);
GLAPI PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d;
#define glVertexAttrib1d glad_glVertexAttrib1d
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVPROC)(GLuint index, const GLdouble *v);
GLAPI PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv;
#define glVertexAttrib1dv glad_glVertexAttrib1dv
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC)(GLuint index, GLfloat x);
GLAPI PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f;
#define glVertexAttrib1f glad_glVertexAttrib1f
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC)(GLuint index, const GLfloat *v);
GLAPI PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv;
#define glVertexAttrib1fv glad_glVertexAttrib1fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SPROC)(GLuint index, GLshort x);
GLAPI PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s;
#define glVertexAttrib1s glad_glVertexAttrib1s
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVPROC)(GLuint index, const GLshort *v);
GLAPI PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv;
#define glVertexAttrib1sv glad_glVertexAttrib1sv
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DPROC)(GLuint index, GLdouble x, GLdouble y);
GLAPI PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d;
#define glVertexAttrib2d glad_glVertexAttrib2d
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVPROC)(GLuint index, const GLdouble *v);
GLAPI PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv;
#define glVertexAttrib2dv glad_glVertexAttrib2dv
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC)(GLuint index, GLfloat x, GLfloat y);
GLAPI PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f;
#define glVertexAttrib2f glad_glVertexAttrib2f
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC)(GLuint index, const GLfloat *v);
GLAPI PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv;
#define glVertexAttrib2fv glad_glVertexAttrib2fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SPROC)(GLuint index, GLshort x, GLshort y);
GLAPI PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s;
#define glVertexAttrib2s glad_glVertexAttrib2s
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVPROC)(GLuint index, const GLshort *v);
GLAPI PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv;
#define glVertexAttrib2sv glad_glVertexAttrib2sv
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
GLAPI PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d;
#define glVertexAttrib3d glad_glVertexAttrib3d
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVPROC)(GLuint index, const GLdouble *v);
GLAPI PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv;
#define glVertexAttrib3dv glad_glVertexAttrib3dv
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
GLAPI PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f;
#define glVertexAttrib3f glad_glVertexAttrib3f
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC)(GLuint index, const GLfloat *v);
GLAPI PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv;
#define glVertexAttrib3fv glad_glVertexAttrib3fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
GLAPI PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s;
#define glVertexAttrib3s glad_glVertexAttrib3s
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVPROC)(GLuint index, const GLshort *v);
GLAPI PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv;
#define glVertexAttrib3sv glad_glVertexAttrib3sv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVPROC)(GLuint index, const GLbyte *v);
GLAPI PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv;
#define glVertexAttrib4Nbv glad_glVertexAttrib4Nbv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVPROC)(GLuint index, const GLint *v);
GLAPI PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv;
#define glVertexAttrib4Niv glad_glVertexAttrib4Niv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVPROC)(GLuint index, const GLshort *v);
GLAPI PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv;
#define glVertexAttrib4Nsv glad_glVertexAttrib4Nsv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
GLAPI PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub;
#define glVertexAttrib4Nub glad_glVertexAttrib4Nub
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVPROC)(GLuint index, const GLubyte *v);
GLAPI PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv;
#define glVertexAttrib4Nubv glad_glVertexAttrib4Nubv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVPROC)(GLuint index, const GLuint *v);
GLAPI PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv;
#define glVertexAttrib4Nuiv glad_glVertexAttrib4Nuiv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVPROC)(GLuint index, const GLushort *v);
GLAPI PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv;
#define glVertexAttrib4Nusv glad_glVertexAttrib4Nusv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVPROC)(GLuint index, const GLbyte *v);
GLAPI PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv;
#define glVertexAttrib4bv glad_glVertexAttrib4bv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d;
#define glVertexAttrib4d glad_glVertexAttrib4d
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC)(GLuint index, const GLdouble *v);
GLAPI PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv;
#define glVertexAttrib4dv glad_glVertexAttrib4dv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f;
#define glVertexAttrib4f glad_glVertexAttrib4f
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC)(GLuint index, const GLfloat *v);
GLAPI PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv;
#define glVertexAttrib4fv glad_glVertexAttrib4fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVPROC)(GLuint index, const GLint *v);
GLAPI PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv;
#define glVertexAttrib4iv glad_glVertexAttrib4iv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s;
#define glVertexAttrib4s glad_glVertexAttrib4s
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVPROC)(GLuint index, const GLshort *v);
GLAPI PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv;
#define glVertexAttrib4sv glad_glVertexAttrib4sv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVPROC)(GLuint index, const GLubyte *v);
GLAPI PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv;
#define glVertexAttrib4ubv glad_glVertexAttrib4ubv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVPROC)(GLuint index, const GLuint *v);
GLAPI PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv;
#define glVertexAttrib4uiv glad_glVertexAttrib4uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVPROC)(GLuint index, const GLushort *v);
GLAPI PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv;
#define glVertexAttrib4usv glad_glVertexAttrib4usv
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
GLAPI PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
#define glVertexAttribPointer glad_glVertexAttribPointer
#endif
#ifndef GL_VERSION_2_1
#define GL_VERSION_2_1 1
GLAPI int GLAD_GL_VERSION_2_1;
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv;
#define glUniformMatrix2x3fv glad_glUniformMatrix2x3fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv;
#define glUniformMatrix3x2fv glad_glUniformMatrix3x2fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv;
#define glUniformMatrix2x4fv glad_glUniformMatrix2x4fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv;
#define glUniformMatrix4x2fv glad_glUniformMatrix4x2fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv;
#define glUniformMatrix3x4fv glad_glUniformMatrix3x4fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv;
#define glUniformMatrix4x3fv glad_glUniformMatrix4x3fv
#endif
#ifndef GL_VERSION_3_0
#define GL_VERSION_3_0 1
GLAPI int GLAD_GL_VERSION_3_0;
typedef void (APIENTRYP PFNGLCOLORMASKIPROC)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
GLAPI PFNGLCOLORMASKIPROC glad_glColorMaski;
#define glColorMaski glad_glColorMaski
typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC)(GLenum target, GLuint index, GLboolean *data);
GLAPI PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v;
#define glGetBooleani_v glad_glGetBooleani_v
typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC)(GLenum target, GLuint index, GLint *data);
GLAPI PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v;
#define glGetIntegeri_v glad_glGetIntegeri_v
typedef void (APIENTRYP PFNGLENABLEIPROC)(GLenum target, GLuint index);
GLAPI PFNGLENABLEIPROC glad_glEnablei;
#define glEnablei glad_glEnablei
typedef void (APIENTRYP PFNGLDISABLEIPROC)(GLenum target, GLuint index);
GLAPI PFNGLDISABLEIPROC glad_glDisablei;
#define glDisablei glad_glDisablei
typedef GLboolean (APIENTRYP PFNGLISENABLEDIPROC)(GLenum target, GLuint index);
GLAPI PFNGLISENABLEDIPROC glad_glIsEnabledi;
#define glIsEnabledi glad_glIsEnabledi
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC)(GLenum primitiveMode);
GLAPI PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback;
#define glBeginTransformFeedback glad_glBeginTransformFeedback
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC)(void);
GLAPI PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback;
#define glEndTransformFeedback glad_glEndTransformFeedback
typedef void (APIENTRYP PFNGLBINDBUFFERRANGEPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
GLAPI PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange;
#define glBindBufferRange glad_glBindBufferRange
typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
GLAPI PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase;
#define glBindBufferBase glad_glBindBufferBase
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
GLAPI PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings;
#define glTransformFeedbackVaryings glad_glTransformFeedbackVaryings
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
GLAPI PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying;
#define glGetTransformFeedbackVarying glad_glGetTransformFeedbackVarying
typedef void (APIENTRYP PFNGLCLAMPCOLORPROC)(GLenum target, GLenum clamp);
GLAPI PFNGLCLAMPCOLORPROC glad_glClampColor;
#define glClampColor glad_glClampColor
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERPROC)(GLuint id, GLenum mode);
GLAPI PFNGLBEGINCONDITIONALRENDERPROC glad_glBeginConditionalRender;
#define glBeginConditionalRender glad_glBeginConditionalRender
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERPROC)(void);
GLAPI PFNGLENDCONDITIONALRENDERPROC glad_glEndConditionalRender;
#define glEndConditionalRender glad_glEndConditionalRender
typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
GLAPI PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer;
#define glVertexAttribIPointer glad_glVertexAttribIPointer
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC)(GLuint index, GLenum pname, GLint *params);
GLAPI PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv;
#define glGetVertexAttribIiv glad_glGetVertexAttribIiv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC)(GLuint index, GLenum pname, GLuint *params);
GLAPI PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv;
#define glGetVertexAttribIuiv glad_glGetVertexAttribIuiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IPROC)(GLuint index, GLint x);
GLAPI PFNGLVERTEXATTRIBI1IPROC glad_glVertexAttribI1i;
#define glVertexAttribI1i glad_glVertexAttribI1i
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IPROC)(GLuint index, GLint x, GLint y);
GLAPI PFNGLVERTEXATTRIBI2IPROC glad_glVertexAttribI2i;
#define glVertexAttribI2i glad_glVertexAttribI2i
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IPROC)(GLuint index, GLint x, GLint y, GLint z);
GLAPI PFNGLVERTEXATTRIBI3IPROC glad_glVertexAttribI3i;
#define glVertexAttribI3i glad_glVertexAttribI3i
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IPROC)(GLuint index, GLint x, GLint y, GLint z, GLint w);
GLAPI PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i;
#define glVertexAttribI4i glad_glVertexAttribI4i
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIPROC)(GLuint index, GLuint x);
GLAPI PFNGLVERTEXATTRIBI1UIPROC glad_glVertexAttribI1ui;
#define glVertexAttribI1ui glad_glVertexAttribI1ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIPROC)(GLuint index, GLuint x, GLuint y);
GLAPI PFNGLVERTEXATTRIBI2UIPROC glad_glVertexAttribI2ui;
#define glVertexAttribI2ui glad_glVertexAttribI2ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z);
GLAPI PFNGLVERTEXATTRIBI3UIPROC glad_glVertexAttribI3ui;
#define glVertexAttribI3ui glad_glVertexAttribI3ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
GLAPI PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui;
#define glVertexAttribI4ui glad_glVertexAttribI4ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVPROC)(GLuint index, const GLint *v);
GLAPI PFNGLVERTEXATTRIBI1IVPROC glad_glVertexAttribI1iv;
#define glVertexAttribI1iv glad_glVertexAttribI1iv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVPROC)(GLuint index, const GLint *v);
GLAPI PFNGLVERTEXATTRIBI2IVPROC glad_glVertexAttribI2iv;
#define glVertexAttribI2iv glad_glVertexAttribI2iv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVPROC)(GLuint index, const GLint *v);
GLAPI PFNGLVERTEXATTRIBI3IVPROC glad_glVertexAttribI3iv;
#define glVertexAttribI3iv glad_glVertexAttribI3iv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVPROC)(GLuint index, const GLint *v);
GLAPI PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv;
#define glVertexAttribI4iv glad_glVertexAttribI4iv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVPROC)(GLuint index, const GLuint *v);
GLAPI PFNGLVERTEXATTRIBI1UIVPROC glad_glVertexAttribI1uiv;
#define glVertexAttribI1uiv glad_glVertexAttribI1uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVPROC)(GLuint index, const GLuint *v);
GLAPI PFNGLVERTEXATTRIBI2UIVPROC glad_glVertexAttribI2uiv;
#define glVertexAttribI2uiv glad_glVertexAttribI2uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVPROC)(GLuint index, const GLuint *v);
GLAPI PFNGLVERTEXATTRIBI3UIVPROC glad_glVertexAttribI3uiv;
#define glVertexAttribI3uiv glad_glVertexAttribI3uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC)(GLuint index, const GLuint *v);
GLAPI PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv;
#define glVertexAttribI4uiv glad_glVertexAttribI4uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVPROC)(GLuint index, const GLbyte *v);
GLAPI PFNGLVERTEXATTRIBI4BVPROC glad_glVertexAttribI4bv;
#define glVertexAttribI4bv glad_glVertexAttribI4bv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVPROC)(GLuint index, const GLshort *v);
GLAPI PFNGLVERTEXATTRIBI4SVPROC glad_glVertexAttribI4sv;
#define glVertexAttribI4sv glad_glVertexAttribI4sv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVPROC)(GLuint index, const GLubyte *v);
GLAPI PFNGLVERTEXATTRIBI4UBVPROC glad_glVertexAttribI4ubv;
#define glVertexAttribI4ubv glad_glVertexAttribI4ubv
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVPROC)(GLuint index, const GLushort *v);
GLAPI PFNGLVERTEXATTRIBI4USVPROC glad_glVertexAttribI4usv;
#define glVertexAttribI4usv glad_glVertexAttribI4usv
typedef void (APIENTRYP PFNGLGETUNIFORMUIVPROC)(GLuint program, GLint location, GLuint *params);
GLAPI PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv;
#define glGetUniformuiv glad_glGetUniformuiv
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONPROC)(GLuint program, GLuint color, const GLchar *name);
GLAPI PFNGLBINDFRAGDATALOCATIONPROC glad_glBindFragDataLocation;
#define glBindFragDataLocation glad_glBindFragDataLocation
typedef GLint (APIENTRYP PFNGLGETFRAGDATALOCATIONPROC)(GLuint program, const GLchar *name);
GLAPI PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation;
#define glGetFragDataLocation glad_glGetFragDataLocation
typedef void (APIENTRYP PFNGLUNIFORM1UIPROC)(GLint location, GLuint v0);
GLAPI PFNGLUNIFORM1UIPROC glad_glUniform1ui;
#define glUniform1ui glad_glUniform1ui
typedef void (APIENTRYP PFNGLUNIFORM2UIPROC)(GLint location, GLuint v0, GLuint v1);
GLAPI PFNGLUNIFORM2UIPROC glad_glUniform2ui;
#define glUniform2ui glad_glUniform2ui
typedef void (APIENTRYP PFNGLUNIFORM3UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2);
GLAPI PFNGLUNIFORM3UIPROC glad_glUniform3ui;
#define glUniform3ui glad_glUniform3ui
typedef void (APIENTRYP PFNGLUNIFORM4UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
GLAPI PFNGLUNIFORM4UIPROC glad_glUniform4ui;
#define glUniform4ui glad_glUniform4ui
typedef void (APIENTRYP PFNGLUNIFORM1UIVPROC)(GLint location, GLsizei count, const GLuint *value);
GLAPI PFNGLUNIFORM1UIVPROC glad_glUniform1uiv;
#define glUniform1uiv glad_glUniform1uiv
typedef void (APIENTRYP PFNGLUNIFORM2UIVPROC)(GLint location, GLsizei count, const GLuint *value);
GLAPI PFNGLUNIFORM2UIVPROC glad_glUniform2uiv;
#define glUniform2uiv glad_glUniform2uiv
typedef void (APIENTRYP PFNGLUNIFORM3UIVPROC)(GLint location, GLsizei count, const GLuint *value);
GLAPI PFNGLUNIFORM3UIVPROC glad_glUniform3uiv;
#define glUniform3uiv glad_glUniform3uiv
typedef void (APIENTRYP PFNGLUNIFORM4UIVPROC)(GLint location, GLsizei count, const GLuint *value);
GLAPI PFNGLUNIFORM4UIVPROC glad_glUniform4uiv;
#define glUniform4uiv glad_glUniform4uiv
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, const GLint *params);
GLAPI PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv;
#define glTexParameterIiv glad_glTexParameterIiv
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVPROC)(GLenum target, GLenum pname, const GLuint *params);
GLAPI PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv;
#define glTexParameterIuiv glad_glTexParameterIuiv
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, GLint *params);
GLAPI PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv;
#define glGetTexParameterIiv glad_glGetTexParameterIiv
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC)(GLenum target, GLenum pname, GLuint *params);
GLAPI PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv;
#define glGetTexParameterIuiv glad_glGetTexParameterIuiv
typedef void (APIENTRYP PFNGLCLEARBUFFERIVPROC)(GLenum buffer, GLint drawbuffer, const GLint *value);
GLAPI PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv;
#define glClearBufferiv glad_glClearBufferiv
typedef void (APIENTRYP PFNGLCLEARBUFFERUIVPROC)(GLenum buffer, GLint drawbuffer, const GLuint *value);
GLAPI PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv;
#define glClearBufferuiv glad_glClearBufferuiv
typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC)(GLenum buffer, GLint drawbuffer, const GLfloat *value);
GLAPI PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv;
#define glClearBufferfv glad_glClearBufferfv
typedef void (APIENTRYP PFNGLCLEARBUFFERFIPROC)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
GLAPI PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi;
#define glClearBufferfi glad_glClearBufferfi
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);
GLAPI PFNGLGETSTRINGIPROC glad_glGetStringi;
#define glGetStringi glad_glGetStringi
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFERPROC)(GLuint renderbuffer);
GLAPI PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer;
#define glIsRenderbuffer glad_glIsRenderbuffer
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
GLAPI PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
#define glBindRenderbuffer glad_glBindRenderbuffer
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint *renderbuffers);
GLAPI PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
#define glDeleteRenderbuffers glad_glDeleteRenderbuffers
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
GLAPI PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
#define glGenRenderbuffers glad_glGenRenderbuffers
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLAPI PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
#define glRenderbufferStorage glad_glRenderbufferStorage
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv;
#define glGetRenderbufferParameteriv glad_glGetRenderbufferParameteriv
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFERPROC)(GLuint framebuffer);
GLAPI PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer;
#define glIsFramebuffer glad_glIsFramebuffer
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
GLAPI PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
#define glBindFramebuffer glad_glBindFramebuffer
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *framebuffers);
GLAPI PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
#define glDeleteFramebuffers glad_glDeleteFramebuffers
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
GLAPI PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
#define glGenFramebuffers glad_glGenFramebuffers
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
#define glCheckFramebufferStatus glad_glCheckFramebufferStatus
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI PFNGLFRAMEBUFFERTEXTURE1DPROC glad_glFramebufferTexture1D;
#define glFramebufferTexture1D glad_glFramebufferTexture1D
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
#define glFramebufferTexture2D glad_glFramebufferTexture2D
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
GLAPI PFNGLFRAMEBUFFERTEXTURE3DPROC glad_glFramebufferTexture3D;
#define glFramebufferTexture3D glad_glFramebufferTexture3D
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
#define glFramebufferRenderbuffer glad_glFramebufferRenderbuffer
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv;
#define glGetFramebufferAttachmentParameteriv glad_glGetFramebufferAttachmentParameteriv
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);
GLAPI PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
#define glGenerateMipmap glad_glGenerateMipmap
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERPROC)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
GLAPI PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer;
#define glBlitFramebuffer glad_glBlitFramebuffer
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample;
#define glRenderbufferStorageMultisample glad_glRenderbufferStorageMultisample
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
GLAPI PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer;
#define glFramebufferTextureLayer glad_glFramebufferTextureLayer
typedef void * (APIENTRYP PFNGLMAPBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
GLAPI PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange;
#define glMapBufferRange glad_glMapBufferRange
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length);
GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange;
#define glFlushMappedBufferRange glad_glFlushMappedBufferRange
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
GLAPI PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
#define glBindVertexArray glad_glBindVertexArray
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
GLAPI PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
#define glDeleteVertexArrays glad_glDeleteVertexArrays
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
GLAPI PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
#define glGenVertexArrays glad_glGenVertexArrays
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYPROC)(GLuint array);
GLAPI PFNGLISVERTEXARRAYPROC glad_glIsVertexArray;
#define glIsVertexArray glad_glIsVertexArray
#endif
#ifndef GL_VERSION_3_1
#define GL_VERSION_3_1 1
GLAPI int GLAD_GL_VERSION_3_1;
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GLAPI PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced;
#define glDrawArraysInstanced glad_glDrawArraysInstanced
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
GLAPI PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced;
#define glDrawElementsInstanced glad_glDrawElementsInstanced
typedef void (APIENTRYP PFNGLTEXBUFFERPROC)(GLenum target, GLenum internalformat, GLuint buffer);
GLAPI PFNGLTEXBUFFERPROC glad_glTexBuffer;
#define glTexBuffer glad_glTexBuffer
typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXPROC)(GLuint index);
GLAPI PFNGLPRIMITIVERESTARTINDEXPROC glad_glPrimitiveRestartIndex;
#define glPrimitiveRestartIndex glad_glPrimitiveRestartIndex
typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
GLAPI PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData;
#define glCopyBufferSubData glad_glCopyBufferSubData
typedef void (APIENTRYP PFNGLGETUNIFORMINDICESPROC)(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
GLAPI PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices;
#define glGetUniformIndices glad_glGetUniformIndices
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC)(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
GLAPI PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv;
#define glGetActiveUniformsiv glad_glGetActiveUniformsiv
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
GLAPI PFNGLGETACTIVEUNIFORMNAMEPROC glad_glGetActiveUniformName;
#define glGetActiveUniformName glad_glGetActiveUniformName
typedef GLuint (APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint program, const GLchar *uniformBlockName);
GLAPI PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex;
#define glGetUniformBlockIndex glad_glGetUniformBlockIndex
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
GLAPI PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv;
#define glGetActiveUniformBlockiv glad_glGetActiveUniformBlockiv
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
GLAPI PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName;
#define glGetActiveUniformBlockName glad_glGetActiveUniformBlockName
typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
GLAPI PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding;
#define glUniformBlockBinding glad_glUniformBlockBinding
#endif
#ifndef GL_VERSION_3_2
#define GL_VERSION_3_2 1
GLAPI int GLAD_GL_VERSION_3_2;
typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
GLAPI PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex;
#define glDrawElementsBaseVertex glad_glDrawElementsBaseVertex
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
GLAPI PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex;
#define glDrawRangeElementsBaseVertex glad_glDrawRangeElementsBaseVertex
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex;
#define glDrawElementsInstancedBaseVertex glad_glDrawElementsInstancedBaseVertex
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
GLAPI PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glad_glMultiDrawElementsBaseVertex;
#define glMultiDrawElementsBaseVertex glad_glMultiDrawElementsBaseVertex
typedef void (APIENTRYP PFNGLPROVOKINGVERTEXPROC)(GLenum mode);
GLAPI PFNGLPROVOKINGVERTEXPROC glad_glProvokingVertex;
#define glProvokingVertex glad_glProvokingVertex
typedef GLsync (APIENTRYP PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
GLAPI PFNGLFENCESYNCPROC glad_glFenceSync;
#define glFenceSync glad_glFenceSync
typedef GLboolean (APIENTRYP PFNGLISSYNCPROC)(GLsync sync);
GLAPI PFNGLISSYNCPROC glad_glIsSync;
#define glIsSync glad_glIsSync
typedef void (APIENTRYP PFNGLDELETESYNCPROC)(GLsync sync);
GLAPI PFNGLDELETESYNCPROC glad_glDeleteSync;
#define glDeleteSync glad_glDeleteSync
typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
GLAPI PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync;
#define glClientWaitSync glad_glClientWaitSync
typedef void (APIENTRYP PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
GLAPI PFNGLWAITSYNCPROC glad_glWaitSync;
#define glWaitSync glad_glWaitSync
typedef void (APIENTRYP PFNGLGETINTEGER64VPROC)(GLenum pname, GLint64 *data);
GLAPI PFNGLGETINTEGER64VPROC glad_glGetInteger64v;
#define glGetInteger64v glad_glGetInteger64v
typedef void (APIENTRYP PFNGLGETSYNCIVPROC)(GLsync sync, GLenum pname, GLsizei count, GLsizei *length, GLint *values);
GLAPI PFNGLGETSYNCIVPROC glad_glGetSynciv;
#define glGetSynciv glad_glGetSynciv
typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC)(GLenum target, GLuint index, GLint64 *data);
GLAPI PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v;
#define glGetInteger64i_v glad_glGetInteger64i_v
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC)(GLenum target, GLenum pname, GLint64 *params);
GLAPI PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v;
#define glGetBufferParameteri64v glad_glGetBufferParameteri64v
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
GLAPI PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture;
#define glFramebufferTexture glad_glFramebufferTexture
typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
GLAPI PFNGLTEXIMAGE2DMULTISAMPLEPROC glad_glTexImage2DMultisample;
#define glTexImage2DMultisample glad_glTexImage2DMultisample
typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
GLAPI PFNGLTEXIMAGE3DMULTISAMPLEPROC glad_glTexImage3DMultisample;
#define glTexImage3DMultisample glad_glTexImage3DMultisample
typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC)(GLenum pname, GLuint index, GLfloat *val);
GLAPI PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv;
#define glGetMultisamplefv glad_glGetMultisamplefv
typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC)(GLuint maskNumber, GLbitfield mask);
GLAPI PFNGLSAMPLEMASKIPROC glad_glSampleMaski;
#define glSampleMaski glad_glSampleMaski
#endif
#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3 1
GLAPI int GLAD_GL_VERSION_3_3;
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)(GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
GLAPI PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glad_glBindFragDataLocationIndexed;
#define glBindFragDataLocationIndexed glad_glBindFragDataLocationIndexed
typedef GLint (APIENTRYP PFNGLGETFRAGDATAINDEXPROC)(GLuint program, const GLchar *name);
GLAPI PFNGLGETFRAGDATAINDEXPROC glad_glGetFragDataIndex;
#define glGetFragDataIndex glad_glGetFragDataIndex
typedef void (APIENTRYP PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint *samplers);
GLAPI PFNGLGENSAMPLERSPROC glad_glGenSamplers;
#define glGenSamplers glad_glGenSamplers
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint *samplers);
GLAPI PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers;
#define glDeleteSamplers glad_glDeleteSamplers
typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC)(GLuint sampler);
GLAPI PFNGLISSAMPLERPROC glad_glIsSampler;
#define glIsSampler glad_glIsSampler
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
GLAPI PFNGLBINDSAMPLERPROC glad_glBindSampler;
#define glBindSampler glad_glBindSampler
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
GLAPI PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri;
#define glSamplerParameteri glad_glSamplerParameteri
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, const GLint *param);
GLAPI PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv;
#define glSamplerParameteriv glad_glSamplerParameteriv
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC)(GLuint sampler, GLenum pname, GLfloat param);
GLAPI PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf;
#define glSamplerParameterf glad_glSamplerParameterf
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, const GLfloat *param);
GLAPI PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv;
#define glSamplerParameterfv glad_glSamplerParameterfv
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, const GLint *param);
GLAPI PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv;
#define glSamplerParameterIiv glad_glSamplerParameterIiv
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, const GLuint *param);
GLAPI PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv;
#define glSamplerParameterIuiv glad_glSamplerParameterIuiv
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, GLint *params);
GLAPI PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv;
#define glGetSamplerParameteriv glad_glGetSamplerParameteriv
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, GLint *params);
GLAPI PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv;
#define glGetSamplerParameterIiv glad_glGetSamplerParameterIiv
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, GLfloat *params);
GLAPI PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv;
#define glGetSamplerParameterfv glad_glGetSamplerParameterfv
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, GLuint *params);
GLAPI PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv;
#define glGetSamplerParameterIuiv glad_glGetSamplerParameterIuiv
typedef void (APIENTRYP PFNGLQUERYCOUNTERPROC)(GLuint id, GLenum target);
GLAPI PFNGLQUERYCOUNTERPROC glad_glQueryCounter;
#define glQueryCounter glad_glQueryCounter
typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VPROC)(GLuint id, GLenum pname, GLint64 *params);
GLAPI PFNGLGETQUERYOBJECTI64VPROC glad_glGetQueryObjecti64v;
#define glGetQueryObjecti64v glad_glGetQueryObjecti64v
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC)(GLuint id, GLenum pname, GLuint64 *params);
GLAPI PFNGLGETQUERYOBJECTUI64VPROC glad_glGetQueryObjectui64v;
#define glGetQueryObjectui64v glad_glGetQueryObjectui64v
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);
GLAPI PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor;
#define glVertexAttribDivisor glad_glVertexAttribDivisor
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI PFNGLVERTEXATTRIBP1UIPROC glad_glVertexAttribP1ui;
#define glVertexAttribP1ui glad_glVertexAttribP1ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI PFNGLVERTEXATTRIBP1UIVPROC glad_glVertexAttribP1uiv;
#define glVertexAttribP1uiv glad_glVertexAttribP1uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI PFNGLVERTEXATTRIBP2UIPROC glad_glVertexAttribP2ui;
#define glVertexAttribP2ui glad_glVertexAttribP2ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI PFNGLVERTEXATTRIBP2UIVPROC glad_glVertexAttribP2uiv;
#define glVertexAttribP2uiv glad_glVertexAttribP2uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI PFNGLVERTEXATTRIBP3UIPROC glad_glVertexAttribP3ui;
#define glVertexAttribP3ui glad_glVertexAttribP3ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI PFNGLVERTEXATTRIBP3UIVPROC glad_glVertexAttribP3uiv;
#define glVertexAttribP3uiv glad_glVertexAttribP3uiv
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI PFNGLVERTEXATTRIBP4UIPROC glad_glVertexAttribP4ui;
#define glVertexAttribP4ui glad_glVertexAttribP4ui
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI PFNGLVERTEXATTRIBP4UIVPROC glad_glVertexAttribP4uiv;
#define glVertexAttribP4uiv glad_glVertexAttribP4uiv
typedef void (APIENTRYP PFNGLVERTEXP2UIPROC)(GLenum type, GLuint value);
GLAPI PFNGLVERTEXP2UIPROC glad_glVertexP2ui;
#define glVertexP2ui glad_glVertexP2ui
typedef void (APIENTRYP PFNGLVERTEXP2UIVPROC)(GLenum type, const GLuint *value);
GLAPI PFNGLVERTEXP2UIVPROC glad_glVertexP2uiv;
#define glVertexP2uiv glad_glVertexP2uiv
typedef void (APIENTRYP PFNGLVERTEXP3UIPROC)(GLenum type, GLuint value);
GLAPI PFNGLVERTEXP3UIPROC glad_glVertexP3ui;
#define glVertexP3ui glad_glVertexP3ui
typedef void (APIENTRYP PFNGLVERTEXP3UIVPROC)(GLenum type, const GLuint *value);
GLAPI PFNGLVERTEXP3UIVPROC glad_glVertexP3uiv;
#define glVertexP3uiv glad_glVertexP3uiv
typedef void (APIENTRYP PFNGLVERTEXP4UIPROC)(GLenum type, GLuint value);
GLAPI PFNGLVERTEXP4UIPROC glad_glVertexP4ui;
#define glVertexP4ui glad_glVertexP4ui
typedef void (APIENTRYP PFNGLVERTEXP4UIVPROC)(GLenum type, const GLuint *value);
GLAPI PFNGLVERTEXP4UIVPROC glad_glVertexP4uiv;
#define glVertexP4uiv glad_glVertexP4uiv
typedef void (APIENTRYP PFNGLTEXCOORDP1UIPROC)(GLenum type, GLuint coords);
GLAPI PFNGLTEXCOORDP1UIPROC glad_glTexCoordP1ui;
#define glTexCoordP1ui glad_glTexCoordP1ui
typedef void (APIENTRYP PFNGLTEXCOORDP1UIVPROC)(GLenum type, const GLuint *coords);
GLAPI PFNGLTEXCOORDP1UIVPROC glad_glTexCoordP1uiv;
#define glTexCoordP1uiv glad_glTexCoordP1uiv
typedef void (APIENTRYP PFNGLTEXCOORDP2UIPROC)(GLenum type, GLuint coords);
GLAPI PFNGLTEXCOORDP2UIPROC glad_glTexCoordP2ui;
#define glTexCoordP2ui glad_glTexCoordP2ui
typedef void (APIENTRYP PFNGLTEXCOORDP2UIVPROC)(GLenum type, const GLuint *coords);
GLAPI PFNGLTEXCOORDP2UIVPROC glad_glTexCoordP2uiv;
#define glTexCoordP2uiv glad_glTexCoordP2uiv
typedef void (APIENTRYP PFNGLTEXCOORDP3UIPROC)(GLenum type, GLuint coords);
GLAPI PFNGLTEXCOORDP3UIPROC glad_glTexCoordP3ui;
#define glTexCoordP3ui glad_glTexCoordP3ui
typedef void (APIENTRYP PFNGLTEXCOORDP3UIVPROC)(GLenum type, const GLuint *coords);
GLAPI PFNGLTEXCOORDP3UIVPROC glad_glTexCoordP3uiv;
#define glTexCoordP3uiv glad_glTexCoordP3uiv
typedef void (APIENTRYP PFNGLTEXCOORDP4UIPROC)(GLenum type, GLuint coords);
GLAPI PFNGLTEXCOORDP4UIPROC glad_glTexCoordP4ui;
#define glTexCoordP4ui glad_glTexCoordP4ui
typedef void (APIENTRYP PFNGLTEXCOORDP4UIVPROC)(GLenum type, const GLuint *coords);
GLAPI PFNGLTEXCOORDP4UIVPROC glad_glTexCoordP4uiv;
#define glTexCoordP4uiv glad_glTexCoordP4uiv
typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIPROC)(GLenum texture, GLenum type, GLuint coords);
GLAPI PFNGLMULTITEXCOORDP1UIPROC glad_glMultiTexCoordP1ui;
#define glMultiTexCoordP1ui glad_glMultiTexCoordP1ui
typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
GLAPI PFNGLMULTITEXCOORDP1UIVPROC glad_glMultiTexCoordP1uiv;
#define glMultiTexCoordP1uiv glad_glMultiTexCoordP1uiv
typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIPROC)(GLenum texture, GLenum type, GLuint coords);
GLAPI PFNGLMULTITEXCOORDP2UIPROC glad_glMultiTexCoordP2ui;
#define glMultiTexCoordP2ui glad_glMultiTexCoordP2ui
typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
GLAPI PFNGLMULTITEXCOORDP2UIVPROC glad_glMultiTexCoordP2uiv;
#define glMultiTexCoordP2uiv glad_glMultiTexCoordP2uiv
typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIPROC)(GLenum texture, GLenum type, GLuint coords);
GLAPI PFNGLMULTITEXCOORDP3UIPROC glad_glMultiTexCoordP3ui;
#define glMultiTexCoordP3ui glad_glMultiTexCoordP3ui
typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
GLAPI PFNGLMULTITEXCOORDP3UIVPROC glad_glMultiTexCoordP3uiv;
#define glMultiTexCoordP3uiv glad_glMultiTexCoordP3uiv
typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIPROC)(GLenum texture, GLenum type, GLuint coords);
GLAPI PFNGLMULTITEXCOORDP4UIPROC glad_glMultiTexCoordP4ui;
#define glMultiTexCoordP4ui glad_glMultiTexCoordP4ui
typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
GLAPI PFNGLMULTITEXCOORDP4UIVPROC glad_glMultiTexCoordP4uiv;
#define glMultiTexCoordP4uiv glad_glMultiTexCoordP4uiv
typedef void (APIENTRYP PFNGLNORMALP3UIPROC)(GLenum type, GLuint coords);
GLAPI PFNGLNORMALP3UIPROC glad_glNormalP3ui;
#define glNormalP3ui glad_glNormalP3ui
typedef void (APIENTRYP PFNGLNORMALP3UIVPROC)(GLenum type, const GLuint *coords);
GLAPI PFNGLNORMALP3UIVPROC glad_glNormalP3uiv;
#define glNormalP3uiv glad_glNormalP3uiv
typedef void (APIENTRYP PFNGLCOLORP3UIPROC)(GLenum type, GLuint color);
GLAPI PFNGLCOLORP3UIPROC glad_glColorP3ui;
#define glColorP3ui glad_glColorP3ui
typedef void (APIENTRYP PFNGLCOLORP3UIVPROC)(GLenum type, const GLuint *color);
GLAPI PFNGLCOLORP3UIVPROC glad_glColorP3uiv;
#define glColorP3uiv glad_glColorP3uiv
typedef void (APIENTRYP PFNGLCOLORP4UIPROC)(GLenum type, GLuint color);
GLAPI PFNGLCOLORP4UIPROC glad_glColorP4ui;
#define glColorP4ui glad_glColorP4ui
typedef void (APIENTRYP PFNGLCOLORP4UIVPROC)(GLenum type, const GLuint *color);
GLAPI PFNGLCOLORP4UIVPROC glad_glColorP4uiv;
#define glColorP4uiv glad_glColorP4uiv
typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIPROC)(GLenum type, GLuint color);
GLAPI PFNGLSECONDARYCOLORP3UIPROC glad_glSecondaryColorP3ui;
#define glSecondaryColorP3ui glad_glSecondaryColorP3ui
typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIVPROC)(GLenum type, const GLuint *color);
GLAPI PFNGLSECONDARYCOLORP3UIVPROC glad_glSecondaryColorP3uiv;
#define glSecondaryColorP3uiv glad_glSecondaryColorP3uiv
#endif

#ifdef __cplusplus
}
#endif

#endif

#pragma endregion

#pragma region implementation
#ifdef RAYFORK_IMPLEMENTATION
/*

    OpenGL loader generated by glad 0.1.33 on Tue Jun  2 18:14:13 2020.

    Language/Generator: C/C++
    Specification: gl
    APIs: gl=3.3
    Profile: compatibility
    Extensions:

    Loader: True
    Local files: False
    Omit khrplatform: False
    Reproducible: False

    Commandline:
        --profile="compatibility" --api="gl=3.3" --generator="c" --spec="gl" --extensions=""
    Online:
        https://glad.dav1d.de/#profile=compatibility&language=c&specification=gl&loader=on&api=gl%3D3.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <glad/glad.h>

static void* get_proc(const char *namez);

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef _WINDOWS_
#undef APIENTRY
#endif
#include <windows.h>
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNWGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;

#ifdef _MSC_VER
#ifdef __has_include
  #if __has_include(<winapifamily.h>)
    #define HAVE_WINAPIFAMILY 1
  #endif
#elif _MSC_VER >= 1700 && !_USING_V110_SDK71_
  #define HAVE_WINAPIFAMILY 1
#endif
#endif

#ifdef HAVE_WINAPIFAMILY
  #include <winapifamily.h>
  #if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    #define IS_UWP 1
  #endif
#endif

static
int open_gl(void) {
#ifndef IS_UWP
    libGL = LoadLibraryW(L"opengl32.dll");
    if(libGL != NULL) {
        void (* tmp)(void);
        tmp = (void(*)(void)) GetProcAddress(libGL, "wglGetProcAddress");
        gladGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE) tmp;
        return gladGetProcAddressPtr != NULL;
    }
#endif

    return 0;
}

static
void close_gl(void) {
    if(libGL != NULL) {
        FreeLibrary((HMODULE) libGL);
        libGL = NULL;
    }
}
#else
#include <dlfcn.h>
static void* libGL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
typedef void* (APIENTRYP PFNGLXGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNGLXGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;
#endif

static
int open_gl(void) {
#ifdef __APPLE__
    static const char *NAMES[] = {
        "../Frameworks/OpenGL.framework/OpenGL",
        "/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
    };
#else
    static const char *NAMES[] = {"libGL.so.1", "libGL.so"};
#endif

    unsigned int index = 0;
    for(index = 0; index < (sizeof(NAMES) / sizeof(NAMES[0])); index++) {
        libGL = dlopen(NAMES[index], RTLD_NOW | RTLD_GLOBAL);

        if(libGL != NULL) {
#if defined(__APPLE__) || defined(__HAIKU__)
            return 1;
#else
            gladGetProcAddressPtr = (PFNGLXGETPROCADDRESSPROC_PRIVATE)dlsym(libGL,
                "glXGetProcAddressARB");
            return gladGetProcAddressPtr != NULL;
#endif
        }
    }

    return 0;
}

static
void close_gl(void) {
    if(libGL != NULL) {
        dlclose(libGL);
        libGL = NULL;
    }
}
#endif

static
void* get_proc(const char *namez) {
    void* result = NULL;
    if(libGL == NULL) return NULL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
    if(gladGetProcAddressPtr != NULL) {
        result = gladGetProcAddressPtr(namez);
    }
#endif
    if(result == NULL) {
#if defined(_WIN32) || defined(__CYGWIN__)
        result = (void*)GetProcAddress((HMODULE) libGL, namez);
#else
        result = dlsym(libGL, namez);
#endif
    }

    return result;
}

int gladLoadGL(void) {
    int status = 0;

    if(open_gl()) {
        status = gladLoadGLLoader(&get_proc);
        close_gl();
    }

    return status;
}

struct gladGLversionStruct GLVersion = { 0, 0 };

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_IS_SOME_NEW_VERSION 1
#endif

static int max_loaded_major;
static int max_loaded_minor;

static const char *exts = NULL;
static int num_exts_i = 0;
static char **exts_i = NULL;

static int get_exts(void) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        exts = (const char *)glGetString(GL_EXTENSIONS);
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        unsigned int index;

        num_exts_i = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (char **)malloc((size_t)num_exts_i * (sizeof *exts_i));
        }

        if (exts_i == NULL) {
            return 0;
        }

        for(index = 0; index < (unsigned)num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = (char*)malloc((len+1) * sizeof(char));
            if(local_str != NULL) {
                memcpy(local_str, gl_str_tmp, (len+1) * sizeof(char));
            }
            exts_i[index] = local_str;
        }
    }
#endif
    return 1;
}

static void free_exts(void) {
    if (exts_i != NULL) {
        int index;
        for(index = 0; index < num_exts_i; index++) {
            free((char *)exts_i[index]);
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}

static int has_ext(const char *ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }

        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }

            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int index;
        if(exts_i == NULL) return 0;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];

            if(exts_i[index] != NULL && strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
#endif

    return 0;
}
int GLAD_GL_VERSION_1_0 = 0;
int GLAD_GL_VERSION_1_1 = 0;
int GLAD_GL_VERSION_1_2 = 0;
int GLAD_GL_VERSION_1_3 = 0;
int GLAD_GL_VERSION_1_4 = 0;
int GLAD_GL_VERSION_1_5 = 0;
int GLAD_GL_VERSION_2_0 = 0;
int GLAD_GL_VERSION_2_1 = 0;
int GLAD_GL_VERSION_3_0 = 0;
int GLAD_GL_VERSION_3_1 = 0;
int GLAD_GL_VERSION_3_2 = 0;
int GLAD_GL_VERSION_3_3 = 0;
PFNGLACCUMPROC glad_glAccum = NULL;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
PFNGLALPHAFUNCPROC glad_glAlphaFunc = NULL;
PFNGLARETEXTURESRESIDENTPROC glad_glAreTexturesResident = NULL;
PFNGLARRAYELEMENTPROC glad_glArrayElement = NULL;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
PFNGLBEGINPROC glad_glBegin = NULL;
PFNGLBEGINCONDITIONALRENDERPROC glad_glBeginConditionalRender = NULL;
PFNGLBEGINQUERYPROC glad_glBeginQuery = NULL;
PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback = NULL;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase = NULL;
PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange = NULL;
PFNGLBINDFRAGDATALOCATIONPROC glad_glBindFragDataLocation = NULL;
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glad_glBindFragDataLocationIndexed = NULL;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer = NULL;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer = NULL;
PFNGLBINDSAMPLERPROC glad_glBindSampler = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
PFNGLBITMAPPROC glad_glBitmap = NULL;
PFNGLBLENDCOLORPROC glad_glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation = NULL;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate = NULL;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
PFNGLCALLLISTPROC glad_glCallList = NULL;
PFNGLCALLLISTSPROC glad_glCallLists = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = NULL;
PFNGLCLAMPCOLORPROC glad_glClampColor = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARACCUMPROC glad_glClearAccum = NULL;
PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi = NULL;
PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv = NULL;
PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv = NULL;
PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLCLEARDEPTHPROC glad_glClearDepth = NULL;
PFNGLCLEARINDEXPROC glad_glClearIndex = NULL;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
PFNGLCLIENTACTIVETEXTUREPROC glad_glClientActiveTexture = NULL;
PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync = NULL;
PFNGLCLIPPLANEPROC glad_glClipPlane = NULL;
PFNGLCOLOR3BPROC glad_glColor3b = NULL;
PFNGLCOLOR3BVPROC glad_glColor3bv = NULL;
PFNGLCOLOR3DPROC glad_glColor3d = NULL;
PFNGLCOLOR3DVPROC glad_glColor3dv = NULL;
PFNGLCOLOR3FPROC glad_glColor3f = NULL;
PFNGLCOLOR3FVPROC glad_glColor3fv = NULL;
PFNGLCOLOR3IPROC glad_glColor3i = NULL;
PFNGLCOLOR3IVPROC glad_glColor3iv = NULL;
PFNGLCOLOR3SPROC glad_glColor3s = NULL;
PFNGLCOLOR3SVPROC glad_glColor3sv = NULL;
PFNGLCOLOR3UBPROC glad_glColor3ub = NULL;
PFNGLCOLOR3UBVPROC glad_glColor3ubv = NULL;
PFNGLCOLOR3UIPROC glad_glColor3ui = NULL;
PFNGLCOLOR3UIVPROC glad_glColor3uiv = NULL;
PFNGLCOLOR3USPROC glad_glColor3us = NULL;
PFNGLCOLOR3USVPROC glad_glColor3usv = NULL;
PFNGLCOLOR4BPROC glad_glColor4b = NULL;
PFNGLCOLOR4BVPROC glad_glColor4bv = NULL;
PFNGLCOLOR4DPROC glad_glColor4d = NULL;
PFNGLCOLOR4DVPROC glad_glColor4dv = NULL;
PFNGLCOLOR4FPROC glad_glColor4f = NULL;
PFNGLCOLOR4FVPROC glad_glColor4fv = NULL;
PFNGLCOLOR4IPROC glad_glColor4i = NULL;
PFNGLCOLOR4IVPROC glad_glColor4iv = NULL;
PFNGLCOLOR4SPROC glad_glColor4s = NULL;
PFNGLCOLOR4SVPROC glad_glColor4sv = NULL;
PFNGLCOLOR4UBPROC glad_glColor4ub = NULL;
PFNGLCOLOR4UBVPROC glad_glColor4ubv = NULL;
PFNGLCOLOR4UIPROC glad_glColor4ui = NULL;
PFNGLCOLOR4UIVPROC glad_glColor4uiv = NULL;
PFNGLCOLOR4USPROC glad_glColor4us = NULL;
PFNGLCOLOR4USVPROC glad_glColor4usv = NULL;
PFNGLCOLORMASKPROC glad_glColorMask = NULL;
PFNGLCOLORMASKIPROC glad_glColorMaski = NULL;
PFNGLCOLORMATERIALPROC glad_glColorMaterial = NULL;
PFNGLCOLORP3UIPROC glad_glColorP3ui = NULL;
PFNGLCOLORP3UIVPROC glad_glColorP3uiv = NULL;
PFNGLCOLORP4UIPROC glad_glColorP4ui = NULL;
PFNGLCOLORP4UIVPROC glad_glColorP4uiv = NULL;
PFNGLCOLORPOINTERPROC glad_glColorPointer = NULL;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D = NULL;
PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData = NULL;
PFNGLCOPYPIXELSPROC glad_glCopyPixels = NULL;
PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D = NULL;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D = NULL;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D = NULL;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = NULL;
PFNGLDELETELISTSPROC glad_glDeleteLists = NULL;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries = NULL;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers = NULL;
PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers = NULL;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
PFNGLDELETESYNCPROC glad_glDeleteSync = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
PFNGLDEPTHMASKPROC glad_glDepthMask = NULL;
PFNGLDEPTHRANGEPROC glad_glDepthRange = NULL;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLDISABLECLIENTSTATEPROC glad_glDisableClientState = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
PFNGLDISABLEIPROC glad_glDisablei = NULL;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced = NULL;
PFNGLDRAWBUFFERPROC glad_glDrawBuffer = NULL;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex = NULL;
PFNGLDRAWPIXELSPROC glad_glDrawPixels = NULL;
PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements = NULL;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex = NULL;
PFNGLEDGEFLAGPROC glad_glEdgeFlag = NULL;
PFNGLEDGEFLAGPOINTERPROC glad_glEdgeFlagPointer = NULL;
PFNGLEDGEFLAGVPROC glad_glEdgeFlagv = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLENABLECLIENTSTATEPROC glad_glEnableClientState = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
PFNGLENABLEIPROC glad_glEnablei = NULL;
PFNGLENDPROC glad_glEnd = NULL;
PFNGLENDCONDITIONALRENDERPROC glad_glEndConditionalRender = NULL;
PFNGLENDLISTPROC glad_glEndList = NULL;
PFNGLENDQUERYPROC glad_glEndQuery = NULL;
PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback = NULL;
PFNGLEVALCOORD1DPROC glad_glEvalCoord1d = NULL;
PFNGLEVALCOORD1DVPROC glad_glEvalCoord1dv = NULL;
PFNGLEVALCOORD1FPROC glad_glEvalCoord1f = NULL;
PFNGLEVALCOORD1FVPROC glad_glEvalCoord1fv = NULL;
PFNGLEVALCOORD2DPROC glad_glEvalCoord2d = NULL;
PFNGLEVALCOORD2DVPROC glad_glEvalCoord2dv = NULL;
PFNGLEVALCOORD2FPROC glad_glEvalCoord2f = NULL;
PFNGLEVALCOORD2FVPROC glad_glEvalCoord2fv = NULL;
PFNGLEVALMESH1PROC glad_glEvalMesh1 = NULL;
PFNGLEVALMESH2PROC glad_glEvalMesh2 = NULL;
PFNGLEVALPOINT1PROC glad_glEvalPoint1 = NULL;
PFNGLEVALPOINT2PROC glad_glEvalPoint2 = NULL;
PFNGLFEEDBACKBUFFERPROC glad_glFeedbackBuffer = NULL;
PFNGLFENCESYNCPROC glad_glFenceSync = NULL;
PFNGLFINISHPROC glad_glFinish = NULL;
PFNGLFLUSHPROC glad_glFlush = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange = NULL;
PFNGLFOGCOORDPOINTERPROC glad_glFogCoordPointer = NULL;
PFNGLFOGCOORDDPROC glad_glFogCoordd = NULL;
PFNGLFOGCOORDDVPROC glad_glFogCoorddv = NULL;
PFNGLFOGCOORDFPROC glad_glFogCoordf = NULL;
PFNGLFOGCOORDFVPROC glad_glFogCoordfv = NULL;
PFNGLFOGFPROC glad_glFogf = NULL;
PFNGLFOGFVPROC glad_glFogfv = NULL;
PFNGLFOGIPROC glad_glFogi = NULL;
PFNGLFOGIVPROC glad_glFogiv = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC glad_glFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC glad_glFramebufferTexture3D = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer = NULL;
PFNGLFRONTFACEPROC glad_glFrontFace = NULL;
PFNGLFRUSTUMPROC glad_glFrustum = NULL;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers = NULL;
PFNGLGENLISTSPROC glad_glGenLists = NULL;
PFNGLGENQUERIESPROC glad_glGenQueries = NULL;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers = NULL;
PFNGLGENSAMPLERSPROC glad_glGenSamplers = NULL;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap = NULL;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv = NULL;
PFNGLGETACTIVEUNIFORMNAMEPROC glad_glGetActiveUniformName = NULL;
PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv = NULL;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation = NULL;
PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v = NULL;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv = NULL;
PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv = NULL;
PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData = NULL;
PFNGLGETCLIPPLANEPROC glad_glGetClipPlane = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage = NULL;
PFNGLGETDOUBLEVPROC glad_glGetDoublev = NULL;
PFNGLGETERRORPROC glad_glGetError = NULL;
PFNGLGETFLOATVPROC glad_glGetFloatv = NULL;
PFNGLGETFRAGDATAINDEXPROC glad_glGetFragDataIndex = NULL;
PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v = NULL;
PFNGLGETINTEGER64VPROC glad_glGetInteger64v = NULL;
PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLGETLIGHTFVPROC glad_glGetLightfv = NULL;
PFNGLGETLIGHTIVPROC glad_glGetLightiv = NULL;
PFNGLGETMAPDVPROC glad_glGetMapdv = NULL;
PFNGLGETMAPFVPROC glad_glGetMapfv = NULL;
PFNGLGETMAPIVPROC glad_glGetMapiv = NULL;
PFNGLGETMATERIALFVPROC glad_glGetMaterialfv = NULL;
PFNGLGETMATERIALIVPROC glad_glGetMaterialiv = NULL;
PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv = NULL;
PFNGLGETPIXELMAPFVPROC glad_glGetPixelMapfv = NULL;
PFNGLGETPIXELMAPUIVPROC glad_glGetPixelMapuiv = NULL;
PFNGLGETPIXELMAPUSVPROC glad_glGetPixelMapusv = NULL;
PFNGLGETPOINTERVPROC glad_glGetPointerv = NULL;
PFNGLGETPOLYGONSTIPPLEPROC glad_glGetPolygonStipple = NULL;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
PFNGLGETQUERYOBJECTI64VPROC glad_glGetQueryObjecti64v = NULL;
PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUI64VPROC glad_glGetQueryObjectui64v = NULL;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv = NULL;
PFNGLGETQUERYIVPROC glad_glGetQueryiv = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv = NULL;
PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv = NULL;
PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv = NULL;
PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv = NULL;
PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv = NULL;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource = NULL;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLGETSTRINGIPROC glad_glGetStringi = NULL;
PFNGLGETSYNCIVPROC glad_glGetSynciv = NULL;
PFNGLGETTEXENVFVPROC glad_glGetTexEnvfv = NULL;
PFNGLGETTEXENVIVPROC glad_glGetTexEnviv = NULL;
PFNGLGETTEXGENDVPROC glad_glGetTexGendv = NULL;
PFNGLGETTEXGENFVPROC glad_glGetTexGenfv = NULL;
PFNGLGETTEXGENIVPROC glad_glGetTexGeniv = NULL;
PFNGLGETTEXIMAGEPROC glad_glGetTexImage = NULL;
PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv = NULL;
PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv = NULL;
PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv = NULL;
PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv = NULL;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv = NULL;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying = NULL;
PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex = NULL;
PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices = NULL;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv = NULL;
PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv = NULL;
PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv = NULL;
PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv = NULL;
PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv = NULL;
PFNGLHINTPROC glad_glHint = NULL;
PFNGLINDEXMASKPROC glad_glIndexMask = NULL;
PFNGLINDEXPOINTERPROC glad_glIndexPointer = NULL;
PFNGLINDEXDPROC glad_glIndexd = NULL;
PFNGLINDEXDVPROC glad_glIndexdv = NULL;
PFNGLINDEXFPROC glad_glIndexf = NULL;
PFNGLINDEXFVPROC glad_glIndexfv = NULL;
PFNGLINDEXIPROC glad_glIndexi = NULL;
PFNGLINDEXIVPROC glad_glIndexiv = NULL;
PFNGLINDEXSPROC glad_glIndexs = NULL;
PFNGLINDEXSVPROC glad_glIndexsv = NULL;
PFNGLINDEXUBPROC glad_glIndexub = NULL;
PFNGLINDEXUBVPROC glad_glIndexubv = NULL;
PFNGLINITNAMESPROC glad_glInitNames = NULL;
PFNGLINTERLEAVEDARRAYSPROC glad_glInterleavedArrays = NULL;
PFNGLISBUFFERPROC glad_glIsBuffer = NULL;
PFNGLISENABLEDPROC glad_glIsEnabled = NULL;
PFNGLISENABLEDIPROC glad_glIsEnabledi = NULL;
PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer = NULL;
PFNGLISLISTPROC glad_glIsList = NULL;
PFNGLISPROGRAMPROC glad_glIsProgram = NULL;
PFNGLISQUERYPROC glad_glIsQuery = NULL;
PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer = NULL;
PFNGLISSAMPLERPROC glad_glIsSampler = NULL;
PFNGLISSHADERPROC glad_glIsShader = NULL;
PFNGLISSYNCPROC glad_glIsSync = NULL;
PFNGLISTEXTUREPROC glad_glIsTexture = NULL;
PFNGLISVERTEXARRAYPROC glad_glIsVertexArray = NULL;
PFNGLLIGHTMODELFPROC glad_glLightModelf = NULL;
PFNGLLIGHTMODELFVPROC glad_glLightModelfv = NULL;
PFNGLLIGHTMODELIPROC glad_glLightModeli = NULL;
PFNGLLIGHTMODELIVPROC glad_glLightModeliv = NULL;
PFNGLLIGHTFPROC glad_glLightf = NULL;
PFNGLLIGHTFVPROC glad_glLightfv = NULL;
PFNGLLIGHTIPROC glad_glLighti = NULL;
PFNGLLIGHTIVPROC glad_glLightiv = NULL;
PFNGLLINESTIPPLEPROC glad_glLineStipple = NULL;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
PFNGLLISTBASEPROC glad_glListBase = NULL;
PFNGLLOADIDENTITYPROC glad_glLoadIdentity = NULL;
PFNGLLOADMATRIXDPROC glad_glLoadMatrixd = NULL;
PFNGLLOADMATRIXFPROC glad_glLoadMatrixf = NULL;
PFNGLLOADNAMEPROC glad_glLoadName = NULL;
PFNGLLOADTRANSPOSEMATRIXDPROC glad_glLoadTransposeMatrixd = NULL;
PFNGLLOADTRANSPOSEMATRIXFPROC glad_glLoadTransposeMatrixf = NULL;
PFNGLLOGICOPPROC glad_glLogicOp = NULL;
PFNGLMAP1DPROC glad_glMap1d = NULL;
PFNGLMAP1FPROC glad_glMap1f = NULL;
PFNGLMAP2DPROC glad_glMap2d = NULL;
PFNGLMAP2FPROC glad_glMap2f = NULL;
PFNGLMAPBUFFERPROC glad_glMapBuffer = NULL;
PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange = NULL;
PFNGLMAPGRID1DPROC glad_glMapGrid1d = NULL;
PFNGLMAPGRID1FPROC glad_glMapGrid1f = NULL;
PFNGLMAPGRID2DPROC glad_glMapGrid2d = NULL;
PFNGLMAPGRID2FPROC glad_glMapGrid2f = NULL;
PFNGLMATERIALFPROC glad_glMaterialf = NULL;
PFNGLMATERIALFVPROC glad_glMaterialfv = NULL;
PFNGLMATERIALIPROC glad_glMateriali = NULL;
PFNGLMATERIALIVPROC glad_glMaterialiv = NULL;
PFNGLMATRIXMODEPROC glad_glMatrixMode = NULL;
PFNGLMULTMATRIXDPROC glad_glMultMatrixd = NULL;
PFNGLMULTMATRIXFPROC glad_glMultMatrixf = NULL;
PFNGLMULTTRANSPOSEMATRIXDPROC glad_glMultTransposeMatrixd = NULL;
PFNGLMULTTRANSPOSEMATRIXFPROC glad_glMultTransposeMatrixf = NULL;
PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements = NULL;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glad_glMultiDrawElementsBaseVertex = NULL;
PFNGLMULTITEXCOORD1DPROC glad_glMultiTexCoord1d = NULL;
PFNGLMULTITEXCOORD1DVPROC glad_glMultiTexCoord1dv = NULL;
PFNGLMULTITEXCOORD1FPROC glad_glMultiTexCoord1f = NULL;
PFNGLMULTITEXCOORD1FVPROC glad_glMultiTexCoord1fv = NULL;
PFNGLMULTITEXCOORD1IPROC glad_glMultiTexCoord1i = NULL;
PFNGLMULTITEXCOORD1IVPROC glad_glMultiTexCoord1iv = NULL;
PFNGLMULTITEXCOORD1SPROC glad_glMultiTexCoord1s = NULL;
PFNGLMULTITEXCOORD1SVPROC glad_glMultiTexCoord1sv = NULL;
PFNGLMULTITEXCOORD2DPROC glad_glMultiTexCoord2d = NULL;
PFNGLMULTITEXCOORD2DVPROC glad_glMultiTexCoord2dv = NULL;
PFNGLMULTITEXCOORD2FPROC glad_glMultiTexCoord2f = NULL;
PFNGLMULTITEXCOORD2FVPROC glad_glMultiTexCoord2fv = NULL;
PFNGLMULTITEXCOORD2IPROC glad_glMultiTexCoord2i = NULL;
PFNGLMULTITEXCOORD2IVPROC glad_glMultiTexCoord2iv = NULL;
PFNGLMULTITEXCOORD2SPROC glad_glMultiTexCoord2s = NULL;
PFNGLMULTITEXCOORD2SVPROC glad_glMultiTexCoord2sv = NULL;
PFNGLMULTITEXCOORD3DPROC glad_glMultiTexCoord3d = NULL;
PFNGLMULTITEXCOORD3DVPROC glad_glMultiTexCoord3dv = NULL;
PFNGLMULTITEXCOORD3FPROC glad_glMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD3FVPROC glad_glMultiTexCoord3fv = NULL;
PFNGLMULTITEXCOORD3IPROC glad_glMultiTexCoord3i = NULL;
PFNGLMULTITEXCOORD3IVPROC glad_glMultiTexCoord3iv = NULL;
PFNGLMULTITEXCOORD3SPROC glad_glMultiTexCoord3s = NULL;
PFNGLMULTITEXCOORD3SVPROC glad_glMultiTexCoord3sv = NULL;
PFNGLMULTITEXCOORD4DPROC glad_glMultiTexCoord4d = NULL;
PFNGLMULTITEXCOORD4DVPROC glad_glMultiTexCoord4dv = NULL;
PFNGLMULTITEXCOORD4FPROC glad_glMultiTexCoord4f = NULL;
PFNGLMULTITEXCOORD4FVPROC glad_glMultiTexCoord4fv = NULL;
PFNGLMULTITEXCOORD4IPROC glad_glMultiTexCoord4i = NULL;
PFNGLMULTITEXCOORD4IVPROC glad_glMultiTexCoord4iv = NULL;
PFNGLMULTITEXCOORD4SPROC glad_glMultiTexCoord4s = NULL;
PFNGLMULTITEXCOORD4SVPROC glad_glMultiTexCoord4sv = NULL;
PFNGLMULTITEXCOORDP1UIPROC glad_glMultiTexCoordP1ui = NULL;
PFNGLMULTITEXCOORDP1UIVPROC glad_glMultiTexCoordP1uiv = NULL;
PFNGLMULTITEXCOORDP2UIPROC glad_glMultiTexCoordP2ui = NULL;
PFNGLMULTITEXCOORDP2UIVPROC glad_glMultiTexCoordP2uiv = NULL;
PFNGLMULTITEXCOORDP3UIPROC glad_glMultiTexCoordP3ui = NULL;
PFNGLMULTITEXCOORDP3UIVPROC glad_glMultiTexCoordP3uiv = NULL;
PFNGLMULTITEXCOORDP4UIPROC glad_glMultiTexCoordP4ui = NULL;
PFNGLMULTITEXCOORDP4UIVPROC glad_glMultiTexCoordP4uiv = NULL;
PFNGLNEWLISTPROC glad_glNewList = NULL;
PFNGLNORMAL3BPROC glad_glNormal3b = NULL;
PFNGLNORMAL3BVPROC glad_glNormal3bv = NULL;
PFNGLNORMAL3DPROC glad_glNormal3d = NULL;
PFNGLNORMAL3DVPROC glad_glNormal3dv = NULL;
PFNGLNORMAL3FPROC glad_glNormal3f = NULL;
PFNGLNORMAL3FVPROC glad_glNormal3fv = NULL;
PFNGLNORMAL3IPROC glad_glNormal3i = NULL;
PFNGLNORMAL3IVPROC glad_glNormal3iv = NULL;
PFNGLNORMAL3SPROC glad_glNormal3s = NULL;
PFNGLNORMAL3SVPROC glad_glNormal3sv = NULL;
PFNGLNORMALP3UIPROC glad_glNormalP3ui = NULL;
PFNGLNORMALP3UIVPROC glad_glNormalP3uiv = NULL;
PFNGLNORMALPOINTERPROC glad_glNormalPointer = NULL;
PFNGLORTHOPROC glad_glOrtho = NULL;
PFNGLPASSTHROUGHPROC glad_glPassThrough = NULL;
PFNGLPIXELMAPFVPROC glad_glPixelMapfv = NULL;
PFNGLPIXELMAPUIVPROC glad_glPixelMapuiv = NULL;
PFNGLPIXELMAPUSVPROC glad_glPixelMapusv = NULL;
PFNGLPIXELSTOREFPROC glad_glPixelStoref = NULL;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = NULL;
PFNGLPIXELTRANSFERFPROC glad_glPixelTransferf = NULL;
PFNGLPIXELTRANSFERIPROC glad_glPixelTransferi = NULL;
PFNGLPIXELZOOMPROC glad_glPixelZoom = NULL;
PFNGLPOINTPARAMETERFPROC glad_glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv = NULL;
PFNGLPOINTPARAMETERIPROC glad_glPointParameteri = NULL;
PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv = NULL;
PFNGLPOINTSIZEPROC glad_glPointSize = NULL;
PFNGLPOLYGONMODEPROC glad_glPolygonMode = NULL;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset = NULL;
PFNGLPOLYGONSTIPPLEPROC glad_glPolygonStipple = NULL;
PFNGLPOPATTRIBPROC glad_glPopAttrib = NULL;
PFNGLPOPCLIENTATTRIBPROC glad_glPopClientAttrib = NULL;
PFNGLPOPMATRIXPROC glad_glPopMatrix = NULL;
PFNGLPOPNAMEPROC glad_glPopName = NULL;
PFNGLPRIMITIVERESTARTINDEXPROC glad_glPrimitiveRestartIndex = NULL;
PFNGLPRIORITIZETEXTURESPROC glad_glPrioritizeTextures = NULL;
PFNGLPROVOKINGVERTEXPROC glad_glProvokingVertex = NULL;
PFNGLPUSHATTRIBPROC glad_glPushAttrib = NULL;
PFNGLPUSHCLIENTATTRIBPROC glad_glPushClientAttrib = NULL;
PFNGLPUSHMATRIXPROC glad_glPushMatrix = NULL;
PFNGLPUSHNAMEPROC glad_glPushName = NULL;
PFNGLQUERYCOUNTERPROC glad_glQueryCounter = NULL;
PFNGLRASTERPOS2DPROC glad_glRasterPos2d = NULL;
PFNGLRASTERPOS2DVPROC glad_glRasterPos2dv = NULL;
PFNGLRASTERPOS2FPROC glad_glRasterPos2f = NULL;
PFNGLRASTERPOS2FVPROC glad_glRasterPos2fv = NULL;
PFNGLRASTERPOS2IPROC glad_glRasterPos2i = NULL;
PFNGLRASTERPOS2IVPROC glad_glRasterPos2iv = NULL;
PFNGLRASTERPOS2SPROC glad_glRasterPos2s = NULL;
PFNGLRASTERPOS2SVPROC glad_glRasterPos2sv = NULL;
PFNGLRASTERPOS3DPROC glad_glRasterPos3d = NULL;
PFNGLRASTERPOS3DVPROC glad_glRasterPos3dv = NULL;
PFNGLRASTERPOS3FPROC glad_glRasterPos3f = NULL;
PFNGLRASTERPOS3FVPROC glad_glRasterPos3fv = NULL;
PFNGLRASTERPOS3IPROC glad_glRasterPos3i = NULL;
PFNGLRASTERPOS3IVPROC glad_glRasterPos3iv = NULL;
PFNGLRASTERPOS3SPROC glad_glRasterPos3s = NULL;
PFNGLRASTERPOS3SVPROC glad_glRasterPos3sv = NULL;
PFNGLRASTERPOS4DPROC glad_glRasterPos4d = NULL;
PFNGLRASTERPOS4DVPROC glad_glRasterPos4dv = NULL;
PFNGLRASTERPOS4FPROC glad_glRasterPos4f = NULL;
PFNGLRASTERPOS4FVPROC glad_glRasterPos4fv = NULL;
PFNGLRASTERPOS4IPROC glad_glRasterPos4i = NULL;
PFNGLRASTERPOS4IVPROC glad_glRasterPos4iv = NULL;
PFNGLRASTERPOS4SPROC glad_glRasterPos4s = NULL;
PFNGLRASTERPOS4SVPROC glad_glRasterPos4sv = NULL;
PFNGLREADBUFFERPROC glad_glReadBuffer = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
PFNGLRECTDPROC glad_glRectd = NULL;
PFNGLRECTDVPROC glad_glRectdv = NULL;
PFNGLRECTFPROC glad_glRectf = NULL;
PFNGLRECTFVPROC glad_glRectfv = NULL;
PFNGLRECTIPROC glad_glRecti = NULL;
PFNGLRECTIVPROC glad_glRectiv = NULL;
PFNGLRECTSPROC glad_glRects = NULL;
PFNGLRECTSVPROC glad_glRectsv = NULL;
PFNGLRENDERMODEPROC glad_glRenderMode = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample = NULL;
PFNGLROTATEDPROC glad_glRotated = NULL;
PFNGLROTATEFPROC glad_glRotatef = NULL;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage = NULL;
PFNGLSAMPLEMASKIPROC glad_glSampleMaski = NULL;
PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv = NULL;
PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv = NULL;
PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf = NULL;
PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv = NULL;
PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv = NULL;
PFNGLSCALEDPROC glad_glScaled = NULL;
PFNGLSCALEFPROC glad_glScalef = NULL;
PFNGLSCISSORPROC glad_glScissor = NULL;
PFNGLSECONDARYCOLOR3BPROC glad_glSecondaryColor3b = NULL;
PFNGLSECONDARYCOLOR3BVPROC glad_glSecondaryColor3bv = NULL;
PFNGLSECONDARYCOLOR3DPROC glad_glSecondaryColor3d = NULL;
PFNGLSECONDARYCOLOR3DVPROC glad_glSecondaryColor3dv = NULL;
PFNGLSECONDARYCOLOR3FPROC glad_glSecondaryColor3f = NULL;
PFNGLSECONDARYCOLOR3FVPROC glad_glSecondaryColor3fv = NULL;
PFNGLSECONDARYCOLOR3IPROC glad_glSecondaryColor3i = NULL;
PFNGLSECONDARYCOLOR3IVPROC glad_glSecondaryColor3iv = NULL;
PFNGLSECONDARYCOLOR3SPROC glad_glSecondaryColor3s = NULL;
PFNGLSECONDARYCOLOR3SVPROC glad_glSecondaryColor3sv = NULL;
PFNGLSECONDARYCOLOR3UBPROC glad_glSecondaryColor3ub = NULL;
PFNGLSECONDARYCOLOR3UBVPROC glad_glSecondaryColor3ubv = NULL;
PFNGLSECONDARYCOLOR3UIPROC glad_glSecondaryColor3ui = NULL;
PFNGLSECONDARYCOLOR3UIVPROC glad_glSecondaryColor3uiv = NULL;
PFNGLSECONDARYCOLOR3USPROC glad_glSecondaryColor3us = NULL;
PFNGLSECONDARYCOLOR3USVPROC glad_glSecondaryColor3usv = NULL;
PFNGLSECONDARYCOLORP3UIPROC glad_glSecondaryColorP3ui = NULL;
PFNGLSECONDARYCOLORP3UIVPROC glad_glSecondaryColorP3uiv = NULL;
PFNGLSECONDARYCOLORPOINTERPROC glad_glSecondaryColorPointer = NULL;
PFNGLSELECTBUFFERPROC glad_glSelectBuffer = NULL;
PFNGLSHADEMODELPROC glad_glShadeModel = NULL;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
PFNGLSTENCILFUNCPROC glad_glStencilFunc = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKPROC glad_glStencilMask = NULL;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate = NULL;
PFNGLSTENCILOPPROC glad_glStencilOp = NULL;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate = NULL;
PFNGLTEXBUFFERPROC glad_glTexBuffer = NULL;
PFNGLTEXCOORD1DPROC glad_glTexCoord1d = NULL;
PFNGLTEXCOORD1DVPROC glad_glTexCoord1dv = NULL;
PFNGLTEXCOORD1FPROC glad_glTexCoord1f = NULL;
PFNGLTEXCOORD1FVPROC glad_glTexCoord1fv = NULL;
PFNGLTEXCOORD1IPROC glad_glTexCoord1i = NULL;
PFNGLTEXCOORD1IVPROC glad_glTexCoord1iv = NULL;
PFNGLTEXCOORD1SPROC glad_glTexCoord1s = NULL;
PFNGLTEXCOORD1SVPROC glad_glTexCoord1sv = NULL;
PFNGLTEXCOORD2DPROC glad_glTexCoord2d = NULL;
PFNGLTEXCOORD2DVPROC glad_glTexCoord2dv = NULL;
PFNGLTEXCOORD2FPROC glad_glTexCoord2f = NULL;
PFNGLTEXCOORD2FVPROC glad_glTexCoord2fv = NULL;
PFNGLTEXCOORD2IPROC glad_glTexCoord2i = NULL;
PFNGLTEXCOORD2IVPROC glad_glTexCoord2iv = NULL;
PFNGLTEXCOORD2SPROC glad_glTexCoord2s = NULL;
PFNGLTEXCOORD2SVPROC glad_glTexCoord2sv = NULL;
PFNGLTEXCOORD3DPROC glad_glTexCoord3d = NULL;
PFNGLTEXCOORD3DVPROC glad_glTexCoord3dv = NULL;
PFNGLTEXCOORD3FPROC glad_glTexCoord3f = NULL;
PFNGLTEXCOORD3FVPROC glad_glTexCoord3fv = NULL;
PFNGLTEXCOORD3IPROC glad_glTexCoord3i = NULL;
PFNGLTEXCOORD3IVPROC glad_glTexCoord3iv = NULL;
PFNGLTEXCOORD3SPROC glad_glTexCoord3s = NULL;
PFNGLTEXCOORD3SVPROC glad_glTexCoord3sv = NULL;
PFNGLTEXCOORD4DPROC glad_glTexCoord4d = NULL;
PFNGLTEXCOORD4DVPROC glad_glTexCoord4dv = NULL;
PFNGLTEXCOORD4FPROC glad_glTexCoord4f = NULL;
PFNGLTEXCOORD4FVPROC glad_glTexCoord4fv = NULL;
PFNGLTEXCOORD4IPROC glad_glTexCoord4i = NULL;
PFNGLTEXCOORD4IVPROC glad_glTexCoord4iv = NULL;
PFNGLTEXCOORD4SPROC glad_glTexCoord4s = NULL;
PFNGLTEXCOORD4SVPROC glad_glTexCoord4sv = NULL;
PFNGLTEXCOORDP1UIPROC glad_glTexCoordP1ui = NULL;
PFNGLTEXCOORDP1UIVPROC glad_glTexCoordP1uiv = NULL;
PFNGLTEXCOORDP2UIPROC glad_glTexCoordP2ui = NULL;
PFNGLTEXCOORDP2UIVPROC glad_glTexCoordP2uiv = NULL;
PFNGLTEXCOORDP3UIPROC glad_glTexCoordP3ui = NULL;
PFNGLTEXCOORDP3UIVPROC glad_glTexCoordP3uiv = NULL;
PFNGLTEXCOORDP4UIPROC glad_glTexCoordP4ui = NULL;
PFNGLTEXCOORDP4UIVPROC glad_glTexCoordP4uiv = NULL;
PFNGLTEXCOORDPOINTERPROC glad_glTexCoordPointer = NULL;
PFNGLTEXENVFPROC glad_glTexEnvf = NULL;
PFNGLTEXENVFVPROC glad_glTexEnvfv = NULL;
PFNGLTEXENVIPROC glad_glTexEnvi = NULL;
PFNGLTEXENVIVPROC glad_glTexEnviv = NULL;
PFNGLTEXGENDPROC glad_glTexGend = NULL;
PFNGLTEXGENDVPROC glad_glTexGendv = NULL;
PFNGLTEXGENFPROC glad_glTexGenf = NULL;
PFNGLTEXGENFVPROC glad_glTexGenfv = NULL;
PFNGLTEXGENIPROC glad_glTexGeni = NULL;
PFNGLTEXGENIVPROC glad_glTexGeniv = NULL;
PFNGLTEXIMAGE1DPROC glad_glTexImage1D = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glad_glTexImage2DMultisample = NULL;
PFNGLTEXIMAGE3DPROC glad_glTexImage3D = NULL;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glad_glTexImage3DMultisample = NULL;
PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv = NULL;
PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv = NULL;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf = NULL;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv = NULL;
PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D = NULL;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings = NULL;
PFNGLTRANSLATEDPROC glad_glTranslated = NULL;
PFNGLTRANSLATEFPROC glad_glTranslatef = NULL;
PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
PFNGLUNIFORM1FVPROC glad_glUniform1fv = NULL;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
PFNGLUNIFORM1IVPROC glad_glUniform1iv = NULL;
PFNGLUNIFORM1UIPROC glad_glUniform1ui = NULL;
PFNGLUNIFORM1UIVPROC glad_glUniform1uiv = NULL;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
PFNGLUNIFORM2FVPROC glad_glUniform2fv = NULL;
PFNGLUNIFORM2IPROC glad_glUniform2i = NULL;
PFNGLUNIFORM2IVPROC glad_glUniform2iv = NULL;
PFNGLUNIFORM2UIPROC glad_glUniform2ui = NULL;
PFNGLUNIFORM2UIVPROC glad_glUniform2uiv = NULL;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
PFNGLUNIFORM3FVPROC glad_glUniform3fv = NULL;
PFNGLUNIFORM3IPROC glad_glUniform3i = NULL;
PFNGLUNIFORM3IVPROC glad_glUniform3iv = NULL;
PFNGLUNIFORM3UIPROC glad_glUniform3ui = NULL;
PFNGLUNIFORM3UIVPROC glad_glUniform3uiv = NULL;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
PFNGLUNIFORM4FVPROC glad_glUniform4fv = NULL;
PFNGLUNIFORM4IPROC glad_glUniform4i = NULL;
PFNGLUNIFORM4IVPROC glad_glUniform4iv = NULL;
PFNGLUNIFORM4UIPROC glad_glUniform4ui = NULL;
PFNGLUNIFORM4UIVPROC glad_glUniform4uiv = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding = NULL;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv = NULL;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer = NULL;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram = NULL;
PFNGLVERTEX2DPROC glad_glVertex2d = NULL;
PFNGLVERTEX2DVPROC glad_glVertex2dv = NULL;
PFNGLVERTEX2FPROC glad_glVertex2f = NULL;
PFNGLVERTEX2FVPROC glad_glVertex2fv = NULL;
PFNGLVERTEX2IPROC glad_glVertex2i = NULL;
PFNGLVERTEX2IVPROC glad_glVertex2iv = NULL;
PFNGLVERTEX2SPROC glad_glVertex2s = NULL;
PFNGLVERTEX2SVPROC glad_glVertex2sv = NULL;
PFNGLVERTEX3DPROC glad_glVertex3d = NULL;
PFNGLVERTEX3DVPROC glad_glVertex3dv = NULL;
PFNGLVERTEX3FPROC glad_glVertex3f = NULL;
PFNGLVERTEX3FVPROC glad_glVertex3fv = NULL;
PFNGLVERTEX3IPROC glad_glVertex3i = NULL;
PFNGLVERTEX3IVPROC glad_glVertex3iv = NULL;
PFNGLVERTEX3SPROC glad_glVertex3s = NULL;
PFNGLVERTEX3SVPROC glad_glVertex3sv = NULL;
PFNGLVERTEX4DPROC glad_glVertex4d = NULL;
PFNGLVERTEX4DVPROC glad_glVertex4dv = NULL;
PFNGLVERTEX4FPROC glad_glVertex4f = NULL;
PFNGLVERTEX4FVPROC glad_glVertex4fv = NULL;
PFNGLVERTEX4IPROC glad_glVertex4i = NULL;
PFNGLVERTEX4IVPROC glad_glVertex4iv = NULL;
PFNGLVERTEX4SPROC glad_glVertex4s = NULL;
PFNGLVERTEX4SVPROC glad_glVertex4sv = NULL;
PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor = NULL;
PFNGLVERTEXATTRIBI1IPROC glad_glVertexAttribI1i = NULL;
PFNGLVERTEXATTRIBI1IVPROC glad_glVertexAttribI1iv = NULL;
PFNGLVERTEXATTRIBI1UIPROC glad_glVertexAttribI1ui = NULL;
PFNGLVERTEXATTRIBI1UIVPROC glad_glVertexAttribI1uiv = NULL;
PFNGLVERTEXATTRIBI2IPROC glad_glVertexAttribI2i = NULL;
PFNGLVERTEXATTRIBI2IVPROC glad_glVertexAttribI2iv = NULL;
PFNGLVERTEXATTRIBI2UIPROC glad_glVertexAttribI2ui = NULL;
PFNGLVERTEXATTRIBI2UIVPROC glad_glVertexAttribI2uiv = NULL;
PFNGLVERTEXATTRIBI3IPROC glad_glVertexAttribI3i = NULL;
PFNGLVERTEXATTRIBI3IVPROC glad_glVertexAttribI3iv = NULL;
PFNGLVERTEXATTRIBI3UIPROC glad_glVertexAttribI3ui = NULL;
PFNGLVERTEXATTRIBI3UIVPROC glad_glVertexAttribI3uiv = NULL;
PFNGLVERTEXATTRIBI4BVPROC glad_glVertexAttribI4bv = NULL;
PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i = NULL;
PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv = NULL;
PFNGLVERTEXATTRIBI4SVPROC glad_glVertexAttribI4sv = NULL;
PFNGLVERTEXATTRIBI4UBVPROC glad_glVertexAttribI4ubv = NULL;
PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui = NULL;
PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv = NULL;
PFNGLVERTEXATTRIBI4USVPROC glad_glVertexAttribI4usv = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer = NULL;
PFNGLVERTEXATTRIBP1UIPROC glad_glVertexAttribP1ui = NULL;
PFNGLVERTEXATTRIBP1UIVPROC glad_glVertexAttribP1uiv = NULL;
PFNGLVERTEXATTRIBP2UIPROC glad_glVertexAttribP2ui = NULL;
PFNGLVERTEXATTRIBP2UIVPROC glad_glVertexAttribP2uiv = NULL;
PFNGLVERTEXATTRIBP3UIPROC glad_glVertexAttribP3ui = NULL;
PFNGLVERTEXATTRIBP3UIVPROC glad_glVertexAttribP3uiv = NULL;
PFNGLVERTEXATTRIBP4UIPROC glad_glVertexAttribP4ui = NULL;
PFNGLVERTEXATTRIBP4UIVPROC glad_glVertexAttribP4uiv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
PFNGLVERTEXP2UIPROC glad_glVertexP2ui = NULL;
PFNGLVERTEXP2UIVPROC glad_glVertexP2uiv = NULL;
PFNGLVERTEXP3UIPROC glad_glVertexP3ui = NULL;
PFNGLVERTEXP3UIVPROC glad_glVertexP3uiv = NULL;
PFNGLVERTEXP4UIPROC glad_glVertexP4ui = NULL;
PFNGLVERTEXP4UIVPROC glad_glVertexP4uiv = NULL;
PFNGLVERTEXPOINTERPROC glad_glVertexPointer = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
PFNGLWAITSYNCPROC glad_glWaitSync = NULL;
PFNGLWINDOWPOS2DPROC glad_glWindowPos2d = NULL;
PFNGLWINDOWPOS2DVPROC glad_glWindowPos2dv = NULL;
PFNGLWINDOWPOS2FPROC glad_glWindowPos2f = NULL;
PFNGLWINDOWPOS2FVPROC glad_glWindowPos2fv = NULL;
PFNGLWINDOWPOS2IPROC glad_glWindowPos2i = NULL;
PFNGLWINDOWPOS2IVPROC glad_glWindowPos2iv = NULL;
PFNGLWINDOWPOS2SPROC glad_glWindowPos2s = NULL;
PFNGLWINDOWPOS2SVPROC glad_glWindowPos2sv = NULL;
PFNGLWINDOWPOS3DPROC glad_glWindowPos3d = NULL;
PFNGLWINDOWPOS3DVPROC glad_glWindowPos3dv = NULL;
PFNGLWINDOWPOS3FPROC glad_glWindowPos3f = NULL;
PFNGLWINDOWPOS3FVPROC glad_glWindowPos3fv = NULL;
PFNGLWINDOWPOS3IPROC glad_glWindowPos3i = NULL;
PFNGLWINDOWPOS3IVPROC glad_glWindowPos3iv = NULL;
PFNGLWINDOWPOS3SPROC glad_glWindowPos3s = NULL;
PFNGLWINDOWPOS3SVPROC glad_glWindowPos3sv = NULL;
static void load_GL_VERSION_1_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_0) return;
	glad_glCullFace = (PFNGLCULLFACEPROC)load("glCullFace");
	glad_glFrontFace = (PFNGLFRONTFACEPROC)load("glFrontFace");
	glad_glHint = (PFNGLHINTPROC)load("glHint");
	glad_glLineWidth = (PFNGLLINEWIDTHPROC)load("glLineWidth");
	glad_glPointSize = (PFNGLPOINTSIZEPROC)load("glPointSize");
	glad_glPolygonMode = (PFNGLPOLYGONMODEPROC)load("glPolygonMode");
	glad_glScissor = (PFNGLSCISSORPROC)load("glScissor");
	glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC)load("glTexParameterf");
	glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)load("glTexParameterfv");
	glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)load("glTexParameteri");
	glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)load("glTexParameteriv");
	glad_glTexImage1D = (PFNGLTEXIMAGE1DPROC)load("glTexImage1D");
	glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)load("glTexImage2D");
	glad_glDrawBuffer = (PFNGLDRAWBUFFERPROC)load("glDrawBuffer");
	glad_glClear = (PFNGLCLEARPROC)load("glClear");
	glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
	glad_glClearStencil = (PFNGLCLEARSTENCILPROC)load("glClearStencil");
	glad_glClearDepth = (PFNGLCLEARDEPTHPROC)load("glClearDepth");
	glad_glStencilMask = (PFNGLSTENCILMASKPROC)load("glStencilMask");
	glad_glColorMask = (PFNGLCOLORMASKPROC)load("glColorMask");
	glad_glDepthMask = (PFNGLDEPTHMASKPROC)load("glDepthMask");
	glad_glDisable = (PFNGLDISABLEPROC)load("glDisable");
	glad_glEnable = (PFNGLENABLEPROC)load("glEnable");
	glad_glFinish = (PFNGLFINISHPROC)load("glFinish");
	glad_glFlush = (PFNGLFLUSHPROC)load("glFlush");
	glad_glBlendFunc = (PFNGLBLENDFUNCPROC)load("glBlendFunc");
	glad_glLogicOp = (PFNGLLOGICOPPROC)load("glLogicOp");
	glad_glStencilFunc = (PFNGLSTENCILFUNCPROC)load("glStencilFunc");
	glad_glStencilOp = (PFNGLSTENCILOPPROC)load("glStencilOp");
	glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc");
	glad_glPixelStoref = (PFNGLPIXELSTOREFPROC)load("glPixelStoref");
	glad_glPixelStorei = (PFNGLPIXELSTOREIPROC)load("glPixelStorei");
	glad_glReadBuffer = (PFNGLREADBUFFERPROC)load("glReadBuffer");
	glad_glReadPixels = (PFNGLREADPIXELSPROC)load("glReadPixels");
	glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC)load("glGetBooleanv");
	glad_glGetDoublev = (PFNGLGETDOUBLEVPROC)load("glGetDoublev");
	glad_glGetError = (PFNGLGETERRORPROC)load("glGetError");
	glad_glGetFloatv = (PFNGLGETFLOATVPROC)load("glGetFloatv");
	glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv");
	glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	glad_glGetTexImage = (PFNGLGETTEXIMAGEPROC)load("glGetTexImage");
	glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)load("glGetTexParameterfv");
	glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)load("glGetTexParameteriv");
	glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)load("glGetTexLevelParameterfv");
	glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)load("glGetTexLevelParameteriv");
	glad_glIsEnabled = (PFNGLISENABLEDPROC)load("glIsEnabled");
	glad_glDepthRange = (PFNGLDEPTHRANGEPROC)load("glDepthRange");
	glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
	glad_glNewList = (PFNGLNEWLISTPROC)load("glNewList");
	glad_glEndList = (PFNGLENDLISTPROC)load("glEndList");
	glad_glCallList = (PFNGLCALLLISTPROC)load("glCallList");
	glad_glCallLists = (PFNGLCALLLISTSPROC)load("glCallLists");
	glad_glDeleteLists = (PFNGLDELETELISTSPROC)load("glDeleteLists");
	glad_glGenLists = (PFNGLGENLISTSPROC)load("glGenLists");
	glad_glListBase = (PFNGLLISTBASEPROC)load("glListBase");
	glad_glBegin = (PFNGLBEGINPROC)load("glBegin");
	glad_glBitmap = (PFNGLBITMAPPROC)load("glBitmap");
	glad_glColor3b = (PFNGLCOLOR3BPROC)load("glColor3b");
	glad_glColor3bv = (PFNGLCOLOR3BVPROC)load("glColor3bv");
	glad_glColor3d = (PFNGLCOLOR3DPROC)load("glColor3d");
	glad_glColor3dv = (PFNGLCOLOR3DVPROC)load("glColor3dv");
	glad_glColor3f = (PFNGLCOLOR3FPROC)load("glColor3f");
	glad_glColor3fv = (PFNGLCOLOR3FVPROC)load("glColor3fv");
	glad_glColor3i = (PFNGLCOLOR3IPROC)load("glColor3i");
	glad_glColor3iv = (PFNGLCOLOR3IVPROC)load("glColor3iv");
	glad_glColor3s = (PFNGLCOLOR3SPROC)load("glColor3s");
	glad_glColor3sv = (PFNGLCOLOR3SVPROC)load("glColor3sv");
	glad_glColor3ub = (PFNGLCOLOR3UBPROC)load("glColor3ub");
	glad_glColor3ubv = (PFNGLCOLOR3UBVPROC)load("glColor3ubv");
	glad_glColor3ui = (PFNGLCOLOR3UIPROC)load("glColor3ui");
	glad_glColor3uiv = (PFNGLCOLOR3UIVPROC)load("glColor3uiv");
	glad_glColor3us = (PFNGLCOLOR3USPROC)load("glColor3us");
	glad_glColor3usv = (PFNGLCOLOR3USVPROC)load("glColor3usv");
	glad_glColor4b = (PFNGLCOLOR4BPROC)load("glColor4b");
	glad_glColor4bv = (PFNGLCOLOR4BVPROC)load("glColor4bv");
	glad_glColor4d = (PFNGLCOLOR4DPROC)load("glColor4d");
	glad_glColor4dv = (PFNGLCOLOR4DVPROC)load("glColor4dv");
	glad_glColor4f = (PFNGLCOLOR4FPROC)load("glColor4f");
	glad_glColor4fv = (PFNGLCOLOR4FVPROC)load("glColor4fv");
	glad_glColor4i = (PFNGLCOLOR4IPROC)load("glColor4i");
	glad_glColor4iv = (PFNGLCOLOR4IVPROC)load("glColor4iv");
	glad_glColor4s = (PFNGLCOLOR4SPROC)load("glColor4s");
	glad_glColor4sv = (PFNGLCOLOR4SVPROC)load("glColor4sv");
	glad_glColor4ub = (PFNGLCOLOR4UBPROC)load("glColor4ub");
	glad_glColor4ubv = (PFNGLCOLOR4UBVPROC)load("glColor4ubv");
	glad_glColor4ui = (PFNGLCOLOR4UIPROC)load("glColor4ui");
	glad_glColor4uiv = (PFNGLCOLOR4UIVPROC)load("glColor4uiv");
	glad_glColor4us = (PFNGLCOLOR4USPROC)load("glColor4us");
	glad_glColor4usv = (PFNGLCOLOR4USVPROC)load("glColor4usv");
	glad_glEdgeFlag = (PFNGLEDGEFLAGPROC)load("glEdgeFlag");
	glad_glEdgeFlagv = (PFNGLEDGEFLAGVPROC)load("glEdgeFlagv");
	glad_glEnd = (PFNGLENDPROC)load("glEnd");
	glad_glIndexd = (PFNGLINDEXDPROC)load("glIndexd");
	glad_glIndexdv = (PFNGLINDEXDVPROC)load("glIndexdv");
	glad_glIndexf = (PFNGLINDEXFPROC)load("glIndexf");
	glad_glIndexfv = (PFNGLINDEXFVPROC)load("glIndexfv");
	glad_glIndexi = (PFNGLINDEXIPROC)load("glIndexi");
	glad_glIndexiv = (PFNGLINDEXIVPROC)load("glIndexiv");
	glad_glIndexs = (PFNGLINDEXSPROC)load("glIndexs");
	glad_glIndexsv = (PFNGLINDEXSVPROC)load("glIndexsv");
	glad_glNormal3b = (PFNGLNORMAL3BPROC)load("glNormal3b");
	glad_glNormal3bv = (PFNGLNORMAL3BVPROC)load("glNormal3bv");
	glad_glNormal3d = (PFNGLNORMAL3DPROC)load("glNormal3d");
	glad_glNormal3dv = (PFNGLNORMAL3DVPROC)load("glNormal3dv");
	glad_glNormal3f = (PFNGLNORMAL3FPROC)load("glNormal3f");
	glad_glNormal3fv = (PFNGLNORMAL3FVPROC)load("glNormal3fv");
	glad_glNormal3i = (PFNGLNORMAL3IPROC)load("glNormal3i");
	glad_glNormal3iv = (PFNGLNORMAL3IVPROC)load("glNormal3iv");
	glad_glNormal3s = (PFNGLNORMAL3SPROC)load("glNormal3s");
	glad_glNormal3sv = (PFNGLNORMAL3SVPROC)load("glNormal3sv");
	glad_glRasterPos2d = (PFNGLRASTERPOS2DPROC)load("glRasterPos2d");
	glad_glRasterPos2dv = (PFNGLRASTERPOS2DVPROC)load("glRasterPos2dv");
	glad_glRasterPos2f = (PFNGLRASTERPOS2FPROC)load("glRasterPos2f");
	glad_glRasterPos2fv = (PFNGLRASTERPOS2FVPROC)load("glRasterPos2fv");
	glad_glRasterPos2i = (PFNGLRASTERPOS2IPROC)load("glRasterPos2i");
	glad_glRasterPos2iv = (PFNGLRASTERPOS2IVPROC)load("glRasterPos2iv");
	glad_glRasterPos2s = (PFNGLRASTERPOS2SPROC)load("glRasterPos2s");
	glad_glRasterPos2sv = (PFNGLRASTERPOS2SVPROC)load("glRasterPos2sv");
	glad_glRasterPos3d = (PFNGLRASTERPOS3DPROC)load("glRasterPos3d");
	glad_glRasterPos3dv = (PFNGLRASTERPOS3DVPROC)load("glRasterPos3dv");
	glad_glRasterPos3f = (PFNGLRASTERPOS3FPROC)load("glRasterPos3f");
	glad_glRasterPos3fv = (PFNGLRASTERPOS3FVPROC)load("glRasterPos3fv");
	glad_glRasterPos3i = (PFNGLRASTERPOS3IPROC)load("glRasterPos3i");
	glad_glRasterPos3iv = (PFNGLRASTERPOS3IVPROC)load("glRasterPos3iv");
	glad_glRasterPos3s = (PFNGLRASTERPOS3SPROC)load("glRasterPos3s");
	glad_glRasterPos3sv = (PFNGLRASTERPOS3SVPROC)load("glRasterPos3sv");
	glad_glRasterPos4d = (PFNGLRASTERPOS4DPROC)load("glRasterPos4d");
	glad_glRasterPos4dv = (PFNGLRASTERPOS4DVPROC)load("glRasterPos4dv");
	glad_glRasterPos4f = (PFNGLRASTERPOS4FPROC)load("glRasterPos4f");
	glad_glRasterPos4fv = (PFNGLRASTERPOS4FVPROC)load("glRasterPos4fv");
	glad_glRasterPos4i = (PFNGLRASTERPOS4IPROC)load("glRasterPos4i");
	glad_glRasterPos4iv = (PFNGLRASTERPOS4IVPROC)load("glRasterPos4iv");
	glad_glRasterPos4s = (PFNGLRASTERPOS4SPROC)load("glRasterPos4s");
	glad_glRasterPos4sv = (PFNGLRASTERPOS4SVPROC)load("glRasterPos4sv");
	glad_glRectd = (PFNGLRECTDPROC)load("glRectd");
	glad_glRectdv = (PFNGLRECTDVPROC)load("glRectdv");
	glad_glRectf = (PFNGLRECTFPROC)load("glRectf");
	glad_glRectfv = (PFNGLRECTFVPROC)load("glRectfv");
	glad_glRecti = (PFNGLRECTIPROC)load("glRecti");
	glad_glRectiv = (PFNGLRECTIVPROC)load("glRectiv");
	glad_glRects = (PFNGLRECTSPROC)load("glRects");
	glad_glRectsv = (PFNGLRECTSVPROC)load("glRectsv");
	glad_glTexCoord1d = (PFNGLTEXCOORD1DPROC)load("glTexCoord1d");
	glad_glTexCoord1dv = (PFNGLTEXCOORD1DVPROC)load("glTexCoord1dv");
	glad_glTexCoord1f = (PFNGLTEXCOORD1FPROC)load("glTexCoord1f");
	glad_glTexCoord1fv = (PFNGLTEXCOORD1FVPROC)load("glTexCoord1fv");
	glad_glTexCoord1i = (PFNGLTEXCOORD1IPROC)load("glTexCoord1i");
	glad_glTexCoord1iv = (PFNGLTEXCOORD1IVPROC)load("glTexCoord1iv");
	glad_glTexCoord1s = (PFNGLTEXCOORD1SPROC)load("glTexCoord1s");
	glad_glTexCoord1sv = (PFNGLTEXCOORD1SVPROC)load("glTexCoord1sv");
	glad_glTexCoord2d = (PFNGLTEXCOORD2DPROC)load("glTexCoord2d");
	glad_glTexCoord2dv = (PFNGLTEXCOORD2DVPROC)load("glTexCoord2dv");
	glad_glTexCoord2f = (PFNGLTEXCOORD2FPROC)load("glTexCoord2f");
	glad_glTexCoord2fv = (PFNGLTEXCOORD2FVPROC)load("glTexCoord2fv");
	glad_glTexCoord2i = (PFNGLTEXCOORD2IPROC)load("glTexCoord2i");
	glad_glTexCoord2iv = (PFNGLTEXCOORD2IVPROC)load("glTexCoord2iv");
	glad_glTexCoord2s = (PFNGLTEXCOORD2SPROC)load("glTexCoord2s");
	glad_glTexCoord2sv = (PFNGLTEXCOORD2SVPROC)load("glTexCoord2sv");
	glad_glTexCoord3d = (PFNGLTEXCOORD3DPROC)load("glTexCoord3d");
	glad_glTexCoord3dv = (PFNGLTEXCOORD3DVPROC)load("glTexCoord3dv");
	glad_glTexCoord3f = (PFNGLTEXCOORD3FPROC)load("glTexCoord3f");
	glad_glTexCoord3fv = (PFNGLTEXCOORD3FVPROC)load("glTexCoord3fv");
	glad_glTexCoord3i = (PFNGLTEXCOORD3IPROC)load("glTexCoord3i");
	glad_glTexCoord3iv = (PFNGLTEXCOORD3IVPROC)load("glTexCoord3iv");
	glad_glTexCoord3s = (PFNGLTEXCOORD3SPROC)load("glTexCoord3s");
	glad_glTexCoord3sv = (PFNGLTEXCOORD3SVPROC)load("glTexCoord3sv");
	glad_glTexCoord4d = (PFNGLTEXCOORD4DPROC)load("glTexCoord4d");
	glad_glTexCoord4dv = (PFNGLTEXCOORD4DVPROC)load("glTexCoord4dv");
	glad_glTexCoord4f = (PFNGLTEXCOORD4FPROC)load("glTexCoord4f");
	glad_glTexCoord4fv = (PFNGLTEXCOORD4FVPROC)load("glTexCoord4fv");
	glad_glTexCoord4i = (PFNGLTEXCOORD4IPROC)load("glTexCoord4i");
	glad_glTexCoord4iv = (PFNGLTEXCOORD4IVPROC)load("glTexCoord4iv");
	glad_glTexCoord4s = (PFNGLTEXCOORD4SPROC)load("glTexCoord4s");
	glad_glTexCoord4sv = (PFNGLTEXCOORD4SVPROC)load("glTexCoord4sv");
	glad_glVertex2d = (PFNGLVERTEX2DPROC)load("glVertex2d");
	glad_glVertex2dv = (PFNGLVERTEX2DVPROC)load("glVertex2dv");
	glad_glVertex2f = (PFNGLVERTEX2FPROC)load("glVertex2f");
	glad_glVertex2fv = (PFNGLVERTEX2FVPROC)load("glVertex2fv");
	glad_glVertex2i = (PFNGLVERTEX2IPROC)load("glVertex2i");
	glad_glVertex2iv = (PFNGLVERTEX2IVPROC)load("glVertex2iv");
	glad_glVertex2s = (PFNGLVERTEX2SPROC)load("glVertex2s");
	glad_glVertex2sv = (PFNGLVERTEX2SVPROC)load("glVertex2sv");
	glad_glVertex3d = (PFNGLVERTEX3DPROC)load("glVertex3d");
	glad_glVertex3dv = (PFNGLVERTEX3DVPROC)load("glVertex3dv");
	glad_glVertex3f = (PFNGLVERTEX3FPROC)load("glVertex3f");
	glad_glVertex3fv = (PFNGLVERTEX3FVPROC)load("glVertex3fv");
	glad_glVertex3i = (PFNGLVERTEX3IPROC)load("glVertex3i");
	glad_glVertex3iv = (PFNGLVERTEX3IVPROC)load("glVertex3iv");
	glad_glVertex3s = (PFNGLVERTEX3SPROC)load("glVertex3s");
	glad_glVertex3sv = (PFNGLVERTEX3SVPROC)load("glVertex3sv");
	glad_glVertex4d = (PFNGLVERTEX4DPROC)load("glVertex4d");
	glad_glVertex4dv = (PFNGLVERTEX4DVPROC)load("glVertex4dv");
	glad_glVertex4f = (PFNGLVERTEX4FPROC)load("glVertex4f");
	glad_glVertex4fv = (PFNGLVERTEX4FVPROC)load("glVertex4fv");
	glad_glVertex4i = (PFNGLVERTEX4IPROC)load("glVertex4i");
	glad_glVertex4iv = (PFNGLVERTEX4IVPROC)load("glVertex4iv");
	glad_glVertex4s = (PFNGLVERTEX4SPROC)load("glVertex4s");
	glad_glVertex4sv = (PFNGLVERTEX4SVPROC)load("glVertex4sv");
	glad_glClipPlane = (PFNGLCLIPPLANEPROC)load("glClipPlane");
	glad_glColorMaterial = (PFNGLCOLORMATERIALPROC)load("glColorMaterial");
	glad_glFogf = (PFNGLFOGFPROC)load("glFogf");
	glad_glFogfv = (PFNGLFOGFVPROC)load("glFogfv");
	glad_glFogi = (PFNGLFOGIPROC)load("glFogi");
	glad_glFogiv = (PFNGLFOGIVPROC)load("glFogiv");
	glad_glLightf = (PFNGLLIGHTFPROC)load("glLightf");
	glad_glLightfv = (PFNGLLIGHTFVPROC)load("glLightfv");
	glad_glLighti = (PFNGLLIGHTIPROC)load("glLighti");
	glad_glLightiv = (PFNGLLIGHTIVPROC)load("glLightiv");
	glad_glLightModelf = (PFNGLLIGHTMODELFPROC)load("glLightModelf");
	glad_glLightModelfv = (PFNGLLIGHTMODELFVPROC)load("glLightModelfv");
	glad_glLightModeli = (PFNGLLIGHTMODELIPROC)load("glLightModeli");
	glad_glLightModeliv = (PFNGLLIGHTMODELIVPROC)load("glLightModeliv");
	glad_glLineStipple = (PFNGLLINESTIPPLEPROC)load("glLineStipple");
	glad_glMaterialf = (PFNGLMATERIALFPROC)load("glMaterialf");
	glad_glMaterialfv = (PFNGLMATERIALFVPROC)load("glMaterialfv");
	glad_glMateriali = (PFNGLMATERIALIPROC)load("glMateriali");
	glad_glMaterialiv = (PFNGLMATERIALIVPROC)load("glMaterialiv");
	glad_glPolygonStipple = (PFNGLPOLYGONSTIPPLEPROC)load("glPolygonStipple");
	glad_glShadeModel = (PFNGLSHADEMODELPROC)load("glShadeModel");
	glad_glTexEnvf = (PFNGLTEXENVFPROC)load("glTexEnvf");
	glad_glTexEnvfv = (PFNGLTEXENVFVPROC)load("glTexEnvfv");
	glad_glTexEnvi = (PFNGLTEXENVIPROC)load("glTexEnvi");
	glad_glTexEnviv = (PFNGLTEXENVIVPROC)load("glTexEnviv");
	glad_glTexGend = (PFNGLTEXGENDPROC)load("glTexGend");
	glad_glTexGendv = (PFNGLTEXGENDVPROC)load("glTexGendv");
	glad_glTexGenf = (PFNGLTEXGENFPROC)load("glTexGenf");
	glad_glTexGenfv = (PFNGLTEXGENFVPROC)load("glTexGenfv");
	glad_glTexGeni = (PFNGLTEXGENIPROC)load("glTexGeni");
	glad_glTexGeniv = (PFNGLTEXGENIVPROC)load("glTexGeniv");
	glad_glFeedbackBuffer = (PFNGLFEEDBACKBUFFERPROC)load("glFeedbackBuffer");
	glad_glSelectBuffer = (PFNGLSELECTBUFFERPROC)load("glSelectBuffer");
	glad_glRenderMode = (PFNGLRENDERMODEPROC)load("glRenderMode");
	glad_glInitNames = (PFNGLINITNAMESPROC)load("glInitNames");
	glad_glLoadName = (PFNGLLOADNAMEPROC)load("glLoadName");
	glad_glPassThrough = (PFNGLPASSTHROUGHPROC)load("glPassThrough");
	glad_glPopName = (PFNGLPOPNAMEPROC)load("glPopName");
	glad_glPushName = (PFNGLPUSHNAMEPROC)load("glPushName");
	glad_glClearAccum = (PFNGLCLEARACCUMPROC)load("glClearAccum");
	glad_glClearIndex = (PFNGLCLEARINDEXPROC)load("glClearIndex");
	glad_glIndexMask = (PFNGLINDEXMASKPROC)load("glIndexMask");
	glad_glAccum = (PFNGLACCUMPROC)load("glAccum");
	glad_glPopAttrib = (PFNGLPOPATTRIBPROC)load("glPopAttrib");
	glad_glPushAttrib = (PFNGLPUSHATTRIBPROC)load("glPushAttrib");
	glad_glMap1d = (PFNGLMAP1DPROC)load("glMap1d");
	glad_glMap1f = (PFNGLMAP1FPROC)load("glMap1f");
	glad_glMap2d = (PFNGLMAP2DPROC)load("glMap2d");
	glad_glMap2f = (PFNGLMAP2FPROC)load("glMap2f");
	glad_glMapGrid1d = (PFNGLMAPGRID1DPROC)load("glMapGrid1d");
	glad_glMapGrid1f = (PFNGLMAPGRID1FPROC)load("glMapGrid1f");
	glad_glMapGrid2d = (PFNGLMAPGRID2DPROC)load("glMapGrid2d");
	glad_glMapGrid2f = (PFNGLMAPGRID2FPROC)load("glMapGrid2f");
	glad_glEvalCoord1d = (PFNGLEVALCOORD1DPROC)load("glEvalCoord1d");
	glad_glEvalCoord1dv = (PFNGLEVALCOORD1DVPROC)load("glEvalCoord1dv");
	glad_glEvalCoord1f = (PFNGLEVALCOORD1FPROC)load("glEvalCoord1f");
	glad_glEvalCoord1fv = (PFNGLEVALCOORD1FVPROC)load("glEvalCoord1fv");
	glad_glEvalCoord2d = (PFNGLEVALCOORD2DPROC)load("glEvalCoord2d");
	glad_glEvalCoord2dv = (PFNGLEVALCOORD2DVPROC)load("glEvalCoord2dv");
	glad_glEvalCoord2f = (PFNGLEVALCOORD2FPROC)load("glEvalCoord2f");
	glad_glEvalCoord2fv = (PFNGLEVALCOORD2FVPROC)load("glEvalCoord2fv");
	glad_glEvalMesh1 = (PFNGLEVALMESH1PROC)load("glEvalMesh1");
	glad_glEvalPoint1 = (PFNGLEVALPOINT1PROC)load("glEvalPoint1");
	glad_glEvalMesh2 = (PFNGLEVALMESH2PROC)load("glEvalMesh2");
	glad_glEvalPoint2 = (PFNGLEVALPOINT2PROC)load("glEvalPoint2");
	glad_glAlphaFunc = (PFNGLALPHAFUNCPROC)load("glAlphaFunc");
	glad_glPixelZoom = (PFNGLPIXELZOOMPROC)load("glPixelZoom");
	glad_glPixelTransferf = (PFNGLPIXELTRANSFERFPROC)load("glPixelTransferf");
	glad_glPixelTransferi = (PFNGLPIXELTRANSFERIPROC)load("glPixelTransferi");
	glad_glPixelMapfv = (PFNGLPIXELMAPFVPROC)load("glPixelMapfv");
	glad_glPixelMapuiv = (PFNGLPIXELMAPUIVPROC)load("glPixelMapuiv");
	glad_glPixelMapusv = (PFNGLPIXELMAPUSVPROC)load("glPixelMapusv");
	glad_glCopyPixels = (PFNGLCOPYPIXELSPROC)load("glCopyPixels");
	glad_glDrawPixels = (PFNGLDRAWPIXELSPROC)load("glDrawPixels");
	glad_glGetClipPlane = (PFNGLGETCLIPPLANEPROC)load("glGetClipPlane");
	glad_glGetLightfv = (PFNGLGETLIGHTFVPROC)load("glGetLightfv");
	glad_glGetLightiv = (PFNGLGETLIGHTIVPROC)load("glGetLightiv");
	glad_glGetMapdv = (PFNGLGETMAPDVPROC)load("glGetMapdv");
	glad_glGetMapfv = (PFNGLGETMAPFVPROC)load("glGetMapfv");
	glad_glGetMapiv = (PFNGLGETMAPIVPROC)load("glGetMapiv");
	glad_glGetMaterialfv = (PFNGLGETMATERIALFVPROC)load("glGetMaterialfv");
	glad_glGetMaterialiv = (PFNGLGETMATERIALIVPROC)load("glGetMaterialiv");
	glad_glGetPixelMapfv = (PFNGLGETPIXELMAPFVPROC)load("glGetPixelMapfv");
	glad_glGetPixelMapuiv = (PFNGLGETPIXELMAPUIVPROC)load("glGetPixelMapuiv");
	glad_glGetPixelMapusv = (PFNGLGETPIXELMAPUSVPROC)load("glGetPixelMapusv");
	glad_glGetPolygonStipple = (PFNGLGETPOLYGONSTIPPLEPROC)load("glGetPolygonStipple");
	glad_glGetTexEnvfv = (PFNGLGETTEXENVFVPROC)load("glGetTexEnvfv");
	glad_glGetTexEnviv = (PFNGLGETTEXENVIVPROC)load("glGetTexEnviv");
	glad_glGetTexGendv = (PFNGLGETTEXGENDVPROC)load("glGetTexGendv");
	glad_glGetTexGenfv = (PFNGLGETTEXGENFVPROC)load("glGetTexGenfv");
	glad_glGetTexGeniv = (PFNGLGETTEXGENIVPROC)load("glGetTexGeniv");
	glad_glIsList = (PFNGLISLISTPROC)load("glIsList");
	glad_glFrustum = (PFNGLFRUSTUMPROC)load("glFrustum");
	glad_glLoadIdentity = (PFNGLLOADIDENTITYPROC)load("glLoadIdentity");
	glad_glLoadMatrixf = (PFNGLLOADMATRIXFPROC)load("glLoadMatrixf");
	glad_glLoadMatrixd = (PFNGLLOADMATRIXDPROC)load("glLoadMatrixd");
	glad_glMatrixMode = (PFNGLMATRIXMODEPROC)load("glMatrixMode");
	glad_glMultMatrixf = (PFNGLMULTMATRIXFPROC)load("glMultMatrixf");
	glad_glMultMatrixd = (PFNGLMULTMATRIXDPROC)load("glMultMatrixd");
	glad_glOrtho = (PFNGLORTHOPROC)load("glOrtho");
	glad_glPopMatrix = (PFNGLPOPMATRIXPROC)load("glPopMatrix");
	glad_glPushMatrix = (PFNGLPUSHMATRIXPROC)load("glPushMatrix");
	glad_glRotated = (PFNGLROTATEDPROC)load("glRotated");
	glad_glRotatef = (PFNGLROTATEFPROC)load("glRotatef");
	glad_glScaled = (PFNGLSCALEDPROC)load("glScaled");
	glad_glScalef = (PFNGLSCALEFPROC)load("glScalef");
	glad_glTranslated = (PFNGLTRANSLATEDPROC)load("glTranslated");
	glad_glTranslatef = (PFNGLTRANSLATEFPROC)load("glTranslatef");
}
static void load_GL_VERSION_1_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_1) return;
	glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)load("glDrawArrays");
	glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
	glad_glGetPointerv = (PFNGLGETPOINTERVPROC)load("glGetPointerv");
	glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)load("glPolygonOffset");
	glad_glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)load("glCopyTexImage1D");
	glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)load("glCopyTexImage2D");
	glad_glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)load("glCopyTexSubImage1D");
	glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)load("glCopyTexSubImage2D");
	glad_glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)load("glTexSubImage1D");
	glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)load("glTexSubImage2D");
	glad_glBindTexture = (PFNGLBINDTEXTUREPROC)load("glBindTexture");
	glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)load("glDeleteTextures");
	glad_glGenTextures = (PFNGLGENTEXTURESPROC)load("glGenTextures");
	glad_glIsTexture = (PFNGLISTEXTUREPROC)load("glIsTexture");
	glad_glArrayElement = (PFNGLARRAYELEMENTPROC)load("glArrayElement");
	glad_glColorPointer = (PFNGLCOLORPOINTERPROC)load("glColorPointer");
	glad_glDisableClientState = (PFNGLDISABLECLIENTSTATEPROC)load("glDisableClientState");
	glad_glEdgeFlagPointer = (PFNGLEDGEFLAGPOINTERPROC)load("glEdgeFlagPointer");
	glad_glEnableClientState = (PFNGLENABLECLIENTSTATEPROC)load("glEnableClientState");
	glad_glIndexPointer = (PFNGLINDEXPOINTERPROC)load("glIndexPointer");
	glad_glInterleavedArrays = (PFNGLINTERLEAVEDARRAYSPROC)load("glInterleavedArrays");
	glad_glNormalPointer = (PFNGLNORMALPOINTERPROC)load("glNormalPointer");
	glad_glTexCoordPointer = (PFNGLTEXCOORDPOINTERPROC)load("glTexCoordPointer");
	glad_glVertexPointer = (PFNGLVERTEXPOINTERPROC)load("glVertexPointer");
	glad_glAreTexturesResident = (PFNGLARETEXTURESRESIDENTPROC)load("glAreTexturesResident");
	glad_glPrioritizeTextures = (PFNGLPRIORITIZETEXTURESPROC)load("glPrioritizeTextures");
	glad_glIndexub = (PFNGLINDEXUBPROC)load("glIndexub");
	glad_glIndexubv = (PFNGLINDEXUBVPROC)load("glIndexubv");
	glad_glPopClientAttrib = (PFNGLPOPCLIENTATTRIBPROC)load("glPopClientAttrib");
	glad_glPushClientAttrib = (PFNGLPUSHCLIENTATTRIBPROC)load("glPushClientAttrib");
}
static void load_GL_VERSION_1_2(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_2) return;
	glad_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)load("glDrawRangeElements");
	glad_glTexImage3D = (PFNGLTEXIMAGE3DPROC)load("glTexImage3D");
	glad_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)load("glTexSubImage3D");
	glad_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)load("glCopyTexSubImage3D");
}
static void load_GL_VERSION_1_3(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_3) return;
	glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)load("glActiveTexture");
	glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)load("glSampleCoverage");
	glad_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)load("glCompressedTexImage3D");
	glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)load("glCompressedTexImage2D");
	glad_glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)load("glCompressedTexImage1D");
	glad_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)load("glCompressedTexSubImage3D");
	glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)load("glCompressedTexSubImage2D");
	glad_glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)load("glCompressedTexSubImage1D");
	glad_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)load("glGetCompressedTexImage");
	glad_glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)load("glClientActiveTexture");
	glad_glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC)load("glMultiTexCoord1d");
	glad_glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC)load("glMultiTexCoord1dv");
	glad_glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC)load("glMultiTexCoord1f");
	glad_glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC)load("glMultiTexCoord1fv");
	glad_glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC)load("glMultiTexCoord1i");
	glad_glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC)load("glMultiTexCoord1iv");
	glad_glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC)load("glMultiTexCoord1s");
	glad_glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC)load("glMultiTexCoord1sv");
	glad_glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC)load("glMultiTexCoord2d");
	glad_glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC)load("glMultiTexCoord2dv");
	glad_glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)load("glMultiTexCoord2f");
	glad_glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)load("glMultiTexCoord2fv");
	glad_glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC)load("glMultiTexCoord2i");
	glad_glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC)load("glMultiTexCoord2iv");
	glad_glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC)load("glMultiTexCoord2s");
	glad_glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC)load("glMultiTexCoord2sv");
	glad_glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC)load("glMultiTexCoord3d");
	glad_glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC)load("glMultiTexCoord3dv");
	glad_glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)load("glMultiTexCoord3f");
	glad_glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC)load("glMultiTexCoord3fv");
	glad_glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC)load("glMultiTexCoord3i");
	glad_glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC)load("glMultiTexCoord3iv");
	glad_glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC)load("glMultiTexCoord3s");
	glad_glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC)load("glMultiTexCoord3sv");
	glad_glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC)load("glMultiTexCoord4d");
	glad_glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC)load("glMultiTexCoord4dv");
	glad_glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)load("glMultiTexCoord4f");
	glad_glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC)load("glMultiTexCoord4fv");
	glad_glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC)load("glMultiTexCoord4i");
	glad_glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC)load("glMultiTexCoord4iv");
	glad_glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC)load("glMultiTexCoord4s");
	glad_glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC)load("glMultiTexCoord4sv");
	glad_glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)load("glLoadTransposeMatrixf");
	glad_glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)load("glLoadTransposeMatrixd");
	glad_glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)load("glMultTransposeMatrixf");
	glad_glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)load("glMultTransposeMatrixd");
}
static void load_GL_VERSION_1_4(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_4) return;
	glad_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)load("glBlendFuncSeparate");
	glad_glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)load("glMultiDrawArrays");
	glad_glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)load("glMultiDrawElements");
	glad_glPointParameterf = (PFNGLPOINTPARAMETERFPROC)load("glPointParameterf");
	glad_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)load("glPointParameterfv");
	glad_glPointParameteri = (PFNGLPOINTPARAMETERIPROC)load("glPointParameteri");
	glad_glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)load("glPointParameteriv");
	glad_glFogCoordf = (PFNGLFOGCOORDFPROC)load("glFogCoordf");
	glad_glFogCoordfv = (PFNGLFOGCOORDFVPROC)load("glFogCoordfv");
	glad_glFogCoordd = (PFNGLFOGCOORDDPROC)load("glFogCoordd");
	glad_glFogCoorddv = (PFNGLFOGCOORDDVPROC)load("glFogCoorddv");
	glad_glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC)load("glFogCoordPointer");
	glad_glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC)load("glSecondaryColor3b");
	glad_glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC)load("glSecondaryColor3bv");
	glad_glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC)load("glSecondaryColor3d");
	glad_glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC)load("glSecondaryColor3dv");
	glad_glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC)load("glSecondaryColor3f");
	glad_glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC)load("glSecondaryColor3fv");
	glad_glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC)load("glSecondaryColor3i");
	glad_glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC)load("glSecondaryColor3iv");
	glad_glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC)load("glSecondaryColor3s");
	glad_glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC)load("glSecondaryColor3sv");
	glad_glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC)load("glSecondaryColor3ub");
	glad_glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC)load("glSecondaryColor3ubv");
	glad_glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC)load("glSecondaryColor3ui");
	glad_glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC)load("glSecondaryColor3uiv");
	glad_glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC)load("glSecondaryColor3us");
	glad_glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC)load("glSecondaryColor3usv");
	glad_glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC)load("glSecondaryColorPointer");
	glad_glWindowPos2d = (PFNGLWINDOWPOS2DPROC)load("glWindowPos2d");
	glad_glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC)load("glWindowPos2dv");
	glad_glWindowPos2f = (PFNGLWINDOWPOS2FPROC)load("glWindowPos2f");
	glad_glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC)load("glWindowPos2fv");
	glad_glWindowPos2i = (PFNGLWINDOWPOS2IPROC)load("glWindowPos2i");
	glad_glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC)load("glWindowPos2iv");
	glad_glWindowPos2s = (PFNGLWINDOWPOS2SPROC)load("glWindowPos2s");
	glad_glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC)load("glWindowPos2sv");
	glad_glWindowPos3d = (PFNGLWINDOWPOS3DPROC)load("glWindowPos3d");
	glad_glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC)load("glWindowPos3dv");
	glad_glWindowPos3f = (PFNGLWINDOWPOS3FPROC)load("glWindowPos3f");
	glad_glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC)load("glWindowPos3fv");
	glad_glWindowPos3i = (PFNGLWINDOWPOS3IPROC)load("glWindowPos3i");
	glad_glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC)load("glWindowPos3iv");
	glad_glWindowPos3s = (PFNGLWINDOWPOS3SPROC)load("glWindowPos3s");
	glad_glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC)load("glWindowPos3sv");
	glad_glBlendColor = (PFNGLBLENDCOLORPROC)load("glBlendColor");
	glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC)load("glBlendEquation");
}
static void load_GL_VERSION_1_5(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_5) return;
	glad_glGenQueries = (PFNGLGENQUERIESPROC)load("glGenQueries");
	glad_glDeleteQueries = (PFNGLDELETEQUERIESPROC)load("glDeleteQueries");
	glad_glIsQuery = (PFNGLISQUERYPROC)load("glIsQuery");
	glad_glBeginQuery = (PFNGLBEGINQUERYPROC)load("glBeginQuery");
	glad_glEndQuery = (PFNGLENDQUERYPROC)load("glEndQuery");
	glad_glGetQueryiv = (PFNGLGETQUERYIVPROC)load("glGetQueryiv");
	glad_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)load("glGetQueryObjectiv");
	glad_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)load("glGetQueryObjectuiv");
	glad_glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
	glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
	glad_glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers");
	glad_glIsBuffer = (PFNGLISBUFFERPROC)load("glIsBuffer");
	glad_glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData");
	glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)load("glBufferSubData");
	glad_glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)load("glGetBufferSubData");
	glad_glMapBuffer = (PFNGLMAPBUFFERPROC)load("glMapBuffer");
	glad_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)load("glUnmapBuffer");
	glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)load("glGetBufferParameteriv");
	glad_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)load("glGetBufferPointerv");
}
static void load_GL_VERSION_2_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_2_0) return;
	glad_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)load("glBlendEquationSeparate");
	glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)load("glDrawBuffers");
	glad_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)load("glStencilOpSeparate");
	glad_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)load("glStencilFuncSeparate");
	glad_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)load("glStencilMaskSeparate");
	glad_glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader");
	glad_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)load("glBindAttribLocation");
	glad_glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader");
	glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram");
	glad_glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader");
	glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load("glDeleteProgram");
	glad_glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader");
	glad_glDetachShader = (PFNGLDETACHSHADERPROC)load("glDetachShader");
	glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)load("glDisableVertexAttribArray");
	glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray");
	glad_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)load("glGetActiveAttrib");
	glad_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)load("glGetActiveUniform");
	glad_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)load("glGetAttachedShaders");
	glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)load("glGetAttribLocation");
	glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv");
	glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog");
	glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv");
	glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog");
	glad_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)load("glGetShaderSource");
	glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation");
	glad_glGetUniformfv = (PFNGLGETUNIFORMFVPROC)load("glGetUniformfv");
	glad_glGetUniformiv = (PFNGLGETUNIFORMIVPROC)load("glGetUniformiv");
	glad_glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)load("glGetVertexAttribdv");
	glad_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)load("glGetVertexAttribfv");
	glad_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)load("glGetVertexAttribiv");
	glad_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)load("glGetVertexAttribPointerv");
	glad_glIsProgram = (PFNGLISPROGRAMPROC)load("glIsProgram");
	glad_glIsShader = (PFNGLISSHADERPROC)load("glIsShader");
	glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram");
	glad_glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource");
	glad_glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram");
	glad_glUniform1f = (PFNGLUNIFORM1FPROC)load("glUniform1f");
	glad_glUniform2f = (PFNGLUNIFORM2FPROC)load("glUniform2f");
	glad_glUniform3f = (PFNGLUNIFORM3FPROC)load("glUniform3f");
	glad_glUniform4f = (PFNGLUNIFORM4FPROC)load("glUniform4f");
	glad_glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i");
	glad_glUniform2i = (PFNGLUNIFORM2IPROC)load("glUniform2i");
	glad_glUniform3i = (PFNGLUNIFORM3IPROC)load("glUniform3i");
	glad_glUniform4i = (PFNGLUNIFORM4IPROC)load("glUniform4i");
	glad_glUniform1fv = (PFNGLUNIFORM1FVPROC)load("glUniform1fv");
	glad_glUniform2fv = (PFNGLUNIFORM2FVPROC)load("glUniform2fv");
	glad_glUniform3fv = (PFNGLUNIFORM3FVPROC)load("glUniform3fv");
	glad_glUniform4fv = (PFNGLUNIFORM4FVPROC)load("glUniform4fv");
	glad_glUniform1iv = (PFNGLUNIFORM1IVPROC)load("glUniform1iv");
	glad_glUniform2iv = (PFNGLUNIFORM2IVPROC)load("glUniform2iv");
	glad_glUniform3iv = (PFNGLUNIFORM3IVPROC)load("glUniform3iv");
	glad_glUniform4iv = (PFNGLUNIFORM4IVPROC)load("glUniform4iv");
	glad_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)load("glUniformMatrix2fv");
	glad_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)load("glUniformMatrix3fv");
	glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load("glUniformMatrix4fv");
	glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)load("glValidateProgram");
	glad_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)load("glVertexAttrib1d");
	glad_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)load("glVertexAttrib1dv");
	glad_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)load("glVertexAttrib1f");
	glad_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)load("glVertexAttrib1fv");
	glad_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)load("glVertexAttrib1s");
	glad_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)load("glVertexAttrib1sv");
	glad_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)load("glVertexAttrib2d");
	glad_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)load("glVertexAttrib2dv");
	glad_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)load("glVertexAttrib2f");
	glad_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)load("glVertexAttrib2fv");
	glad_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)load("glVertexAttrib2s");
	glad_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)load("glVertexAttrib2sv");
	glad_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)load("glVertexAttrib3d");
	glad_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)load("glVertexAttrib3dv");
	glad_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)load("glVertexAttrib3f");
	glad_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)load("glVertexAttrib3fv");
	glad_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)load("glVertexAttrib3s");
	glad_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)load("glVertexAttrib3sv");
	glad_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)load("glVertexAttrib4Nbv");
	glad_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)load("glVertexAttrib4Niv");
	glad_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)load("glVertexAttrib4Nsv");
	glad_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)load("glVertexAttrib4Nub");
	glad_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)load("glVertexAttrib4Nubv");
	glad_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)load("glVertexAttrib4Nuiv");
	glad_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)load("glVertexAttrib4Nusv");
	glad_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)load("glVertexAttrib4bv");
	glad_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)load("glVertexAttrib4d");
	glad_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)load("glVertexAttrib4dv");
	glad_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)load("glVertexAttrib4f");
	glad_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)load("glVertexAttrib4fv");
	glad_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)load("glVertexAttrib4iv");
	glad_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)load("glVertexAttrib4s");
	glad_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)load("glVertexAttrib4sv");
	glad_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)load("glVertexAttrib4ubv");
	glad_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)load("glVertexAttrib4uiv");
	glad_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)load("glVertexAttrib4usv");
	glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer");
}
static void load_GL_VERSION_2_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_2_1) return;
	glad_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)load("glUniformMatrix2x3fv");
	glad_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)load("glUniformMatrix3x2fv");
	glad_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)load("glUniformMatrix2x4fv");
	glad_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)load("glUniformMatrix4x2fv");
	glad_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)load("glUniformMatrix3x4fv");
	glad_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)load("glUniformMatrix4x3fv");
}
static void load_GL_VERSION_3_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_0) return;
	glad_glColorMaski = (PFNGLCOLORMASKIPROC)load("glColorMaski");
	glad_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)load("glGetBooleani_v");
	glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v");
	glad_glEnablei = (PFNGLENABLEIPROC)load("glEnablei");
	glad_glDisablei = (PFNGLDISABLEIPROC)load("glDisablei");
	glad_glIsEnabledi = (PFNGLISENABLEDIPROC)load("glIsEnabledi");
	glad_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)load("glBeginTransformFeedback");
	glad_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)load("glEndTransformFeedback");
	glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange");
	glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase");
	glad_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)load("glTransformFeedbackVaryings");
	glad_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)load("glGetTransformFeedbackVarying");
	glad_glClampColor = (PFNGLCLAMPCOLORPROC)load("glClampColor");
	glad_glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)load("glBeginConditionalRender");
	glad_glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)load("glEndConditionalRender");
	glad_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)load("glVertexAttribIPointer");
	glad_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)load("glGetVertexAttribIiv");
	glad_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)load("glGetVertexAttribIuiv");
	glad_glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)load("glVertexAttribI1i");
	glad_glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)load("glVertexAttribI2i");
	glad_glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)load("glVertexAttribI3i");
	glad_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)load("glVertexAttribI4i");
	glad_glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)load("glVertexAttribI1ui");
	glad_glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)load("glVertexAttribI2ui");
	glad_glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)load("glVertexAttribI3ui");
	glad_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)load("glVertexAttribI4ui");
	glad_glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)load("glVertexAttribI1iv");
	glad_glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)load("glVertexAttribI2iv");
	glad_glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)load("glVertexAttribI3iv");
	glad_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)load("glVertexAttribI4iv");
	glad_glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)load("glVertexAttribI1uiv");
	glad_glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)load("glVertexAttribI2uiv");
	glad_glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)load("glVertexAttribI3uiv");
	glad_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)load("glVertexAttribI4uiv");
	glad_glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)load("glVertexAttribI4bv");
	glad_glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)load("glVertexAttribI4sv");
	glad_glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)load("glVertexAttribI4ubv");
	glad_glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)load("glVertexAttribI4usv");
	glad_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)load("glGetUniformuiv");
	glad_glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)load("glBindFragDataLocation");
	glad_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)load("glGetFragDataLocation");
	glad_glUniform1ui = (PFNGLUNIFORM1UIPROC)load("glUniform1ui");
	glad_glUniform2ui = (PFNGLUNIFORM2UIPROC)load("glUniform2ui");
	glad_glUniform3ui = (PFNGLUNIFORM3UIPROC)load("glUniform3ui");
	glad_glUniform4ui = (PFNGLUNIFORM4UIPROC)load("glUniform4ui");
	glad_glUniform1uiv = (PFNGLUNIFORM1UIVPROC)load("glUniform1uiv");
	glad_glUniform2uiv = (PFNGLUNIFORM2UIVPROC)load("glUniform2uiv");
	glad_glUniform3uiv = (PFNGLUNIFORM3UIVPROC)load("glUniform3uiv");
	glad_glUniform4uiv = (PFNGLUNIFORM4UIVPROC)load("glUniform4uiv");
	glad_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)load("glTexParameterIiv");
	glad_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)load("glTexParameterIuiv");
	glad_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)load("glGetTexParameterIiv");
	glad_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)load("glGetTexParameterIuiv");
	glad_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)load("glClearBufferiv");
	glad_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)load("glClearBufferuiv");
	glad_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)load("glClearBufferfv");
	glad_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)load("glClearBufferfi");
	glad_glGetStringi = (PFNGLGETSTRINGIPROC)load("glGetStringi");
	glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)load("glIsRenderbuffer");
	glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load("glBindRenderbuffer");
	glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load("glDeleteRenderbuffers");
	glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)load("glGenRenderbuffers");
	glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)load("glRenderbufferStorage");
	glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)load("glGetRenderbufferParameteriv");
	glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)load("glIsFramebuffer");
	glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load("glBindFramebuffer");
	glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load("glDeleteFramebuffers");
	glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)load("glGenFramebuffers");
	glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)load("glCheckFramebufferStatus");
	glad_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)load("glFramebufferTexture1D");
	glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)load("glFramebufferTexture2D");
	glad_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)load("glFramebufferTexture3D");
	glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)load("glFramebufferRenderbuffer");
	glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetFramebufferAttachmentParameteriv");
	glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)load("glGenerateMipmap");
	glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)load("glBlitFramebuffer");
	glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glRenderbufferStorageMultisample");
	glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)load("glFramebufferTextureLayer");
	glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)load("glMapBufferRange");
	glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)load("glFlushMappedBufferRange");
	glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray");
	glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays");
	glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays");
	glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)load("glIsVertexArray");
}
static void load_GL_VERSION_3_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_1) return;
	glad_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)load("glDrawArraysInstanced");
	glad_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)load("glDrawElementsInstanced");
	glad_glTexBuffer = (PFNGLTEXBUFFERPROC)load("glTexBuffer");
	glad_glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)load("glPrimitiveRestartIndex");
	glad_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)load("glCopyBufferSubData");
	glad_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)load("glGetUniformIndices");
	glad_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)load("glGetActiveUniformsiv");
	glad_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)load("glGetActiveUniformName");
	glad_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)load("glGetUniformBlockIndex");
	glad_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)load("glGetActiveUniformBlockiv");
	glad_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)load("glGetActiveUniformBlockName");
	glad_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)load("glUniformBlockBinding");
	glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange");
	glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase");
	glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v");
}
static void load_GL_VERSION_3_2(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_2) return;
	glad_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)load("glDrawElementsBaseVertex");
	glad_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)load("glDrawRangeElementsBaseVertex");
	glad_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)load("glDrawElementsInstancedBaseVertex");
	glad_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)load("glMultiDrawElementsBaseVertex");
	glad_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)load("glProvokingVertex");
	glad_glFenceSync = (PFNGLFENCESYNCPROC)load("glFenceSync");
	glad_glIsSync = (PFNGLISSYNCPROC)load("glIsSync");
	glad_glDeleteSync = (PFNGLDELETESYNCPROC)load("glDeleteSync");
	glad_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)load("glClientWaitSync");
	glad_glWaitSync = (PFNGLWAITSYNCPROC)load("glWaitSync");
	glad_glGetInteger64v = (PFNGLGETINTEGER64VPROC)load("glGetInteger64v");
	glad_glGetSynciv = (PFNGLGETSYNCIVPROC)load("glGetSynciv");
	glad_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)load("glGetInteger64i_v");
	glad_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)load("glGetBufferParameteri64v");
	glad_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)load("glFramebufferTexture");
	glad_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)load("glTexImage2DMultisample");
	glad_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)load("glTexImage3DMultisample");
	glad_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)load("glGetMultisamplefv");
	glad_glSampleMaski = (PFNGLSAMPLEMASKIPROC)load("glSampleMaski");
}
static void load_GL_VERSION_3_3(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_3) return;
	glad_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)load("glBindFragDataLocationIndexed");
	glad_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)load("glGetFragDataIndex");
	glad_glGenSamplers = (PFNGLGENSAMPLERSPROC)load("glGenSamplers");
	glad_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)load("glDeleteSamplers");
	glad_glIsSampler = (PFNGLISSAMPLERPROC)load("glIsSampler");
	glad_glBindSampler = (PFNGLBINDSAMPLERPROC)load("glBindSampler");
	glad_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)load("glSamplerParameteri");
	glad_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)load("glSamplerParameteriv");
	glad_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)load("glSamplerParameterf");
	glad_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)load("glSamplerParameterfv");
	glad_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)load("glSamplerParameterIiv");
	glad_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)load("glSamplerParameterIuiv");
	glad_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)load("glGetSamplerParameteriv");
	glad_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)load("glGetSamplerParameterIiv");
	glad_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)load("glGetSamplerParameterfv");
	glad_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)load("glGetSamplerParameterIuiv");
	glad_glQueryCounter = (PFNGLQUERYCOUNTERPROC)load("glQueryCounter");
	glad_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)load("glGetQueryObjecti64v");
	glad_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)load("glGetQueryObjectui64v");
	glad_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)load("glVertexAttribDivisor");
	glad_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)load("glVertexAttribP1ui");
	glad_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)load("glVertexAttribP1uiv");
	glad_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)load("glVertexAttribP2ui");
	glad_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)load("glVertexAttribP2uiv");
	glad_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)load("glVertexAttribP3ui");
	glad_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)load("glVertexAttribP3uiv");
	glad_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)load("glVertexAttribP4ui");
	glad_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)load("glVertexAttribP4uiv");
	glad_glVertexP2ui = (PFNGLVERTEXP2UIPROC)load("glVertexP2ui");
	glad_glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)load("glVertexP2uiv");
	glad_glVertexP3ui = (PFNGLVERTEXP3UIPROC)load("glVertexP3ui");
	glad_glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)load("glVertexP3uiv");
	glad_glVertexP4ui = (PFNGLVERTEXP4UIPROC)load("glVertexP4ui");
	glad_glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)load("glVertexP4uiv");
	glad_glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)load("glTexCoordP1ui");
	glad_glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)load("glTexCoordP1uiv");
	glad_glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)load("glTexCoordP2ui");
	glad_glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)load("glTexCoordP2uiv");
	glad_glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)load("glTexCoordP3ui");
	glad_glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)load("glTexCoordP3uiv");
	glad_glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)load("glTexCoordP4ui");
	glad_glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)load("glTexCoordP4uiv");
	glad_glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)load("glMultiTexCoordP1ui");
	glad_glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)load("glMultiTexCoordP1uiv");
	glad_glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)load("glMultiTexCoordP2ui");
	glad_glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)load("glMultiTexCoordP2uiv");
	glad_glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)load("glMultiTexCoordP3ui");
	glad_glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)load("glMultiTexCoordP3uiv");
	glad_glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)load("glMultiTexCoordP4ui");
	glad_glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)load("glMultiTexCoordP4uiv");
	glad_glNormalP3ui = (PFNGLNORMALP3UIPROC)load("glNormalP3ui");
	glad_glNormalP3uiv = (PFNGLNORMALP3UIVPROC)load("glNormalP3uiv");
	glad_glColorP3ui = (PFNGLCOLORP3UIPROC)load("glColorP3ui");
	glad_glColorP3uiv = (PFNGLCOLORP3UIVPROC)load("glColorP3uiv");
	glad_glColorP4ui = (PFNGLCOLORP4UIPROC)load("glColorP4ui");
	glad_glColorP4uiv = (PFNGLCOLORP4UIVPROC)load("glColorP4uiv");
	glad_glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)load("glSecondaryColorP3ui");
	glad_glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)load("glSecondaryColorP3uiv");
}
static int find_extensionsGL(void) {
	if (!get_exts()) return 0;
	(void)&has_ext;
	free_exts();
	return 1;
}

static void find_coreGL(void) {

    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    int i, major, minor;

    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    version = (const char*) glGetString(GL_VERSION);
    if (!version) return;

    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &major, &minor);
#endif

    GLVersion.major = major; GLVersion.minor = minor;
    max_loaded_major = major; max_loaded_minor = minor;
	GLAD_GL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
	GLAD_GL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
	GLAD_GL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
	GLAD_GL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
	GLAD_GL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
	GLAD_GL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
	GLAD_GL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
	GLAD_GL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
	GLAD_GL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
	GLAD_GL_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
	GLAD_GL_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;
	GLAD_GL_VERSION_3_3 = (major == 3 && minor >= 3) || major > 3;
	if (GLVersion.major > 3 || (GLVersion.major >= 3 && GLVersion.minor >= 3)) {
		max_loaded_major = 3;
		max_loaded_minor = 3;
	}
}

int gladLoadGLLoader(GLADloadproc load) {
	GLVersion.major = 0; GLVersion.minor = 0;
	glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	if(glGetString == NULL) return 0;
	if(glGetString(GL_VERSION) == NULL) return 0;
	find_coreGL();
	load_GL_VERSION_1_0(load);
	load_GL_VERSION_1_1(load);
	load_GL_VERSION_1_2(load);
	load_GL_VERSION_1_3(load);
	load_GL_VERSION_1_4(load);
	load_GL_VERSION_1_5(load);
	load_GL_VERSION_2_0(load);
	load_GL_VERSION_2_1(load);
	load_GL_VERSION_3_0(load);
	load_GL_VERSION_3_1(load);
	load_GL_VERSION_3_2(load);
	load_GL_VERSION_3_3(load);

	if (!find_extensionsGL()) return 0;
	return GLVersion.major != 0 || GLVersion.minor != 0;
}

#endif

#pragma endregion

#pragma endregion

#pragma region entry point
#ifdef RAYFORK_IMPLEMENTATION

static rf_context rf__global_ctx;
static rf_renderer_memory_buffers rf__global_mem;

static bool rf__first_frame;
static float rf__delta_time;
static uint64_t rf__frame_time_begin;

extern void rf_on_init(void);
extern void rf_on_frame(float delta_time);
extern void rf_on_event(const sapp_event* event);

static void rf__on_init(void)
{
    gladLoadGL();

    stm_setup();

    rf_init(&rf__global_ctx, &rf__global_mem, RF_WINDOW_WIDTH, RF_WINDOW_HEIGHT, RF_DEFAULT_OPENGL_PROCS);

    rf_on_init();
}

static void rf__on_frame(void)
{
    if (rf__first_frame)
    {
        rf__first_frame = false;
    }
    else
    {
        rf__delta_time = stm_sec(stm_since(rf__frame_time_begin));
        rf__frame_time_begin = stm_now();
    }

    rf_on_frame(rf__delta_time);
}

sapp_desc sokol_main(int argc, char** argv)
{
    return (sapp_desc)
    {
        .width = RF_WINDOW_WIDTH,
        .height = RF_WINDOW_HEIGHT,
        .init_cb = rf__on_init,
        .frame_cb = rf__on_frame,
        .event_cb = rf_on_event,
        .window_title = RF_WINDOW_TITLE,
    };
}
#endif //RAYFORK_IMPLEMENTATION
#pragma endregion