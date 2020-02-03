//Implementation of the geometric shapes example from raylib using rayfork

#define RF_AUDIO_IMPL
#define RF_RENDERER_IMPL
#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "rayfork_audio.h"
#include "rayfork.h"
#include "sokol_app.h"

#include <math.h>
#include <stdio.h>

#define max_samples               512
#define max_samples_per_update   4096

#define screen_width 800
#define screen_height 450

rf_renderer_context rf_ctx;
rf_audio_stream stream;
short* data;
short* write_buffer;
rf_vec2 mouse_pos = {-100.0f, -100.0f };  // Position read in to determine next frequency
float frequency;
float old_frequency;
int read_cursor;
int wave_length;
rf_vec2 position;
bool is_left_button_down;

// With rayfork_audio the context struct must be in the same translation unit as the rayfork_audio implementation.
// This is done in order to only include miniaudio (which is very big) in the translation unit with the implementation.
rf_audio_context global_audio_ctx;

void on_init(void)
{
    // Load opengl with glad
    gladLoadGL();

    // Initialize audio device
    rf_audio_init(&global_audio_ctx);

    // Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, screen_width, screen_height);
    rf_load_font_default();

    // Init raw audio stream (sample rate: 22050, sample size: 16bit-short, channels: 1-mono)
    stream = rf_create_audio_stream(22050, 16, 1);

    // Buffer for the single cycle waveform we are synthesizing
    data = (short*)malloc(sizeof(short)*max_samples);

    // Frame buffer, describing the waveform when repeated over the course of a frame
    write_buffer = (short*)malloc(sizeof(short)*max_samples_per_update);

    rf_play_audio_stream(stream); // Start processing stream buffer (no data loaded currently)

    // Cycles per second (hz)
    frequency = 440.0f;

    // Previous value, used to test if sine needs to be rewritten, and to smoothly modulate frequency
    old_frequency = 1.0f;

    // Cursor to read and copy the samples of the sine wave buffer
    read_cursor = 0;

    // Computed size in samples of the sine wave
    wave_length = 1;

    rf_set_target_fps(30); // Set our game to run at 30 frames-per-second
}

void on_frame(void)
{
    if (is_left_button_down)
    {
        float fp = (float)(mouse_pos.y);
        frequency = 40.0f + (float)(fp);
    }

    // Rewrite the sine wave.
    // Compute two cycles to allow the buffer padding, simplifying any modulation, resampling, etc.
    if (frequency != old_frequency)
    {
        // Compute wavelength. Limit size in both directions.
        int oldWavelength = wave_length;
        wave_length = (int)(22050/frequency);
        if (wave_length > max_samples/2) wave_length = max_samples/2;
        if (wave_length < 1) wave_length = 1;

        // Write sine wave.
        for (int i = 0; i < wave_length*2; i++)
        {
            data[i] = (short)(sinf(((2*RF_PI*(float)i/wave_length)))*32000);
        }

        // Scale read cursor's position to minimize transition artifacts
        read_cursor = (int)(read_cursor * ((float)wave_length / (float)oldWavelength));
        old_frequency = frequency;
    }

    // Refill audio stream if required
    if (rf_is_audio_stream_processed(stream))
    {
        // Synthesize a buffer that is exactly the requested size
        int writeCursor = 0;

        while (writeCursor < max_samples_per_update)
        {
            // Start by trying to write the whole chunk at once
            int writeLength = max_samples_per_update-writeCursor;

            // Limit to the maximum readable size
            int readLength = wave_length-read_cursor;

            if (writeLength > readLength) writeLength = readLength;

            // Write the slice
            memcpy(write_buffer + writeCursor, data + read_cursor, writeLength*sizeof(short));

            // Update cursors and loop audio
            read_cursor = (read_cursor + writeLength) % wave_length;

            writeCursor += writeLength;
        }

        // Copy finished frame to audio stream
        rf_update_audio_stream(stream, write_buffer, max_samples_per_update);
    }

    // Draw
    rf_begin_drawing();

    rf_clear_background(rf_raywhite);

    char buff[500];
    snprintf(buff, 500, "sine frequency: %i", (int)frequency);
    rf_draw_text(buff, 800 - 220, 10, 20, rf_red);
    rf_draw_text("click mouse button to change frequency", 10, 10, 20, rf_darkgray);

    // Draw the current buffer state proportionate to the screen
    for (int i = 0; i < screen_width; i++)
    {
        position.x = i;
        position.y = 250 + 50 * data[i * max_samples / screen_width] / 32000;

        rf_draw_pixel_v(position, rf_red);
    }

    rf_end_drawing();
}

void on_event(const sapp_event* event)
{
    if (event->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
        mouse_pos = (rf_vec2) {event->mouse_x, event->mouse_y };
    }

    if (event->type == SAPP_EVENTTYPE_MOUSE_DOWN && event->mouse_button == SAPP_MOUSEBUTTON_LEFT)
    {
        const float fp = mouse_pos.y;
        frequency = 40.0f + fp;
    }
}

void on_cleanup(void)
{
    //Empty
}