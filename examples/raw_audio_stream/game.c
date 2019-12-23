//Implementation of the geometric shapes example from raylib using rayfork

#define RF_GRAPHICS_API_OPENGL_33
#include "glad/glad.h"
#include "sokol_app.h"
#include "rayfork.h"
#include "rayfork_audio.h"

#include <math.h>
#include <stdio.h>

#define MAX_SAMPLES               512
#define MAX_SAMPLES_PER_UPDATE   4096

rf_context rf_ctx;
rf_audio_stream stream;
short* data;
short* writeBuf;
rf_vector2 mousePosition = { -100.0f, -100.0f };  // Position read in to determine next frequency
float frequency;
float oldFrequency;
int readCursor;
int waveLength;
rf_vector2 position;
bool is_left_button_down;

#define screen_width 800
#define screen_height 450

void on_init(void)
{
    //Load opengl with glad
    gladLoadGL();

    //Initialise rayfork and load the default font
    rf_context_init(&rf_ctx, screen_width, screen_height);
    rf_load_font_default(&rf_ctx);

    rf_audio_init(); // Initialize audio device

    // Init raw audio stream (sample rate: 22050, sample size: 16bit-short, channels: 1-mono)
    stream = rf_create_audio_stream(22050, 16, 1);

    // Buffer for the single cycle waveform we are synthesizing
    data = (short*)malloc(sizeof(short)*MAX_SAMPLES);

    // Frame buffer, describing the waveform when repeated over the course of a frame
    writeBuf = (short*)malloc(sizeof(short)*MAX_SAMPLES_PER_UPDATE);

    rf_play_audio_stream(stream); // Start processing stream buffer (no data loaded currently)

    // Cycles per second (hz)
    frequency = 440.0f;

    // Previous value, used to test if sine needs to be rewritten, and to smoothly modulate frequency
    oldFrequency = 1.0f;

    // Cursor to read and copy the samples of the sine wave buffer
    readCursor = 0;

    // Computed size in samples of the sine wave
    waveLength = 1;

    rf_set_target_fps(&rf_ctx, 30); // Set our game to run at 30 frames-per-second
}

void on_frame(void)
{
    if (is_left_button_down)
    {
        float fp = (float)(mousePosition.y);
        frequency = 40.0f + (float)(fp);
    }

    // Rewrite the sine wave.
    // Compute two cycles to allow the buffer padding, simplifying any modulation, resampling, etc.
    if (frequency != oldFrequency)
    {
        // Compute wavelength. Limit size in both directions.
        int oldWavelength = waveLength;
        waveLength = (int)(22050/frequency);
        if (waveLength > MAX_SAMPLES/2) waveLength = MAX_SAMPLES/2;
        if (waveLength < 1) waveLength = 1;

        // Write sine wave.
        for (int i = 0; i < waveLength*2; i++)
        {
            data[i] = (short)(sinf(((2*RF_PI*(float)i/waveLength)))*32000);
        }

        // Scale read cursor's position to minimize transition artifacts
        readCursor = (int)(readCursor * ((float)waveLength / (float)oldWavelength));
        oldFrequency = frequency;
    }

    // Refill audio stream if required
    if (rf_is_audio_stream_processed(stream))
    {
        // Synthesize a buffer that is exactly the requested size
        int writeCursor = 0;

        while (writeCursor < MAX_SAMPLES_PER_UPDATE)
        {
            // Start by trying to write the whole chunk at once
            int writeLength = MAX_SAMPLES_PER_UPDATE-writeCursor;

            // Limit to the maximum readable size
            int readLength = waveLength-readCursor;

            if (writeLength > readLength) writeLength = readLength;

            // Write the slice
            memcpy(writeBuf + writeCursor, data + readCursor, writeLength*sizeof(short));

            // Update cursors and loop audio
            readCursor = (readCursor + writeLength) % waveLength;

            writeCursor += writeLength;
        }

        // Copy finished frame to audio stream
        rf_update_audio_stream(stream, writeBuf, MAX_SAMPLES_PER_UPDATE);
    }

    // Draw
    rf_begin_drawing(&rf_ctx);

    rf_clear_background(rf_raywhite);

    char buff[500];
    snprintf(buff, 500, "sine frequency: %i", (int)frequency);
    rf_draw_text(&rf_ctx, buff, 800 - 220, 10, 20, rf_red);
    rf_draw_text(&rf_ctx, "click mouse button to change frequency", 10, 10, 20, rf_darkgray);

    // Draw the current buffer state proportionate to the screen
    for (int i = 0; i < screen_width; i++)
    {
        position.x = i;
        position.y = 250 + 50*data[i*MAX_SAMPLES/screen_width]/32000;

        rf_draw_pixel_v(&rf_ctx, position, rf_red);
    }

    rf_end_drawing(&rf_ctx);
}

void on_cleanup(void)
{
    //Empty
}

void on_event(const sapp_event* event)
{
    //Empty
}