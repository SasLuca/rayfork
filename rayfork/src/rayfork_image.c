#include "rayfork_common_internal.h"
#include "rayfork_image.h"
#include "rayfork_math.h"

//region stb_image
//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
static RF_THREAD_LOCAL rf_allocator* _rf_stbi_allocator;

//Example: RF_WITH_STBI_ALLOCATOR(&allocator, { image.data = stbi_load_from_memory(image_file_buffer, file_size, &img_width, &img_height, &img_bpp, 0); });
#define RF_WITH_STBI_ALLOCATOR(allocator, ...) { (_rf_stbi_allocator = (allocator)); __VA_ARGS__; _rf_stbi_allocator = NULL; }

//#define STBI_NO_GIF
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)                     RF_ALLOC(*_rf_stbi_allocator, sz)
#define STBI_FREE(p)                        RF_FREE(*_rf_stbi_allocator, p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) _rf_realloc_wrapper(*_rf_stbi_allocator, p, oldsz, newsz);
#define STBI_ASSERT(it)                     RF_ASSERT(it)
#define STBIDEF                             RF_INTERNAL
#include "stb/stb_image.h"
//endregion

//region stb_image_resize
//Global thread-local alloctor for stb image. Everytime we call a function from stbi we set the allocator and then set it to null afterwards.
static RF_THREAD_LOCAL rf_allocator* _rf_stbir_allocator;

//Example: RF_WITH_STBI_ALLOCATOR(&allocator, { image.data = stbi_load_from_memory(image_file_buffer, file_size, &img_width, &img_height, &img_bpp, 0); });
#define RF_WITH_STBIR_ALLOCATOR(allocator, ...) { (_rf_stbir_allocator = (allocator)); __VA_ARGS__; _rf_stbir_allocator = NULL; }

#define STB_IMAGE_IMPLEMENTATION
#define STBIR_MALLOC(sz) RF_ALLOC(*_rf_stbir_allocator, sz)
#define STBIR_FREE(p)    RF_FREE(*_rf_stbir_allocator, p)
#define STBIR_ASSERT(it) RF_ASSERT(it)
#define STBIRDEF         RF_INTERNAL
//#include "stb/stb_image_resize.h"
//endregion

//region extract image data functions

// Returns the size of the image in bytes
RF_API int rf_image_size(rf_image image)
{
    int size = 0;
    int width = image.width;
    int height = image.height;

    for (int i = 0; i < image.mipmaps; i++)
    {
        size += rf_get_buffer_size_for_pixel_format(image.width, image.height, image.format);

        width /= 2;
        height /= 2;

        // Security check for NPOT textures
        if (width < 1) width = 1;
        if (height < 1) height = 1;
    }

    return size;
}

// Get pixel data from image in the form of rf_color struct array
RF_API rf_color* rf_get_image_pixel_data(rf_image image, rf_allocator allocator)
{
    rf_color* pixels = (rf_color*) RF_ALLOC(allocator, image.width * image.height * sizeof(rf_color));

    if (pixels == NULL) return pixels;

    if (image.format >= RF_COMPRESSED_DXT1_RGB) RF_LOG_V(RF_LOG_WARNING, "Pixel data retrieval not supported for compressed image formats");
    else
    {
        if ((image.format == RF_UNCOMPRESSED_R32) ||
            (image.format == RF_UNCOMPRESSED_R32G32B32) ||
            (image.format == RF_UNCOMPRESSED_R32G32B32A32)) RF_LOG_V(RF_LOG_WARNING, "32bit pixel format converted to 8bit per channel");

        for (int i = 0, k = 0; i < image.width*image.height; i++)
        {
            switch (image.format)
            {
                case RF_UNCOMPRESSED_GRAYSCALE:
                {
                    pixels[i].r = ((unsigned char* )image.data)[i];
                    pixels[i].g = ((unsigned char* )image.data)[i];
                    pixels[i].b = ((unsigned char* )image.data)[i];
                    pixels[i].a = 255;

                } break;
                case RF_UNCOMPRESSED_GRAY_ALPHA:
                {
                    pixels[i].r = ((unsigned char* )image.data)[k];
                    pixels[i].g = ((unsigned char* )image.data)[k];
                    pixels[i].b = ((unsigned char* )image.data)[k];
                    pixels[i].a = ((unsigned char* )image.data)[k + 1];

                    k += 2;
                } break;
                case RF_UNCOMPRESSED_R5G5B5A1:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111000000) >> 6)*(255/31));
                    pixels[i].b = (unsigned char)((float)((pixel & 0b0000000000111110) >> 1)*(255/31));
                    pixels[i].a = (unsigned char)((pixel & 0b0000000000000001)*255);

                } break;
                case RF_UNCOMPRESSED_R5G6B5:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111100000) >> 5)*(255/63));
                    pixels[i].b = (unsigned char)((float)(pixel & 0b0000000000011111)*(255/31));
                    pixels[i].a = 255;

                } break;
                case RF_UNCOMPRESSED_R4G4B4A4:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12)*(255/15));
                    pixels[i].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)*(255/15));
                    pixels[i].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)*(255/15));
                    pixels[i].a = (unsigned char)((float)(pixel & 0b0000000000001111)*(255/15));

                } break;
                case RF_UNCOMPRESSED_R8G8B8A8:
                {
                    pixels[i].r = ((unsigned char* )image.data)[k];
                    pixels[i].g = ((unsigned char* )image.data)[k + 1];
                    pixels[i].b = ((unsigned char* )image.data)[k + 2];
                    pixels[i].a = ((unsigned char* )image.data)[k + 3];

                    k += 4;
                } break;
                case RF_UNCOMPRESSED_R8G8B8:
                {
                    pixels[i].r = (unsigned char)((unsigned char* )image.data)[k];
                    pixels[i].g = (unsigned char)((unsigned char* )image.data)[k + 1];
                    pixels[i].b = (unsigned char)((unsigned char* )image.data)[k + 2];
                    pixels[i].a = 255;

                    k += 3;
                } break;
                case RF_UNCOMPRESSED_R32:
                {
                    pixels[i].r = (unsigned char)(((float* )image.data)[k]*255.0f);
                    pixels[i].g = 0;
                    pixels[i].b = 0;
                    pixels[i].a = 255;

                } break;
                case RF_UNCOMPRESSED_R32G32B32:
                {
                    pixels[i].r = (unsigned char)(((float* )image.data)[k]*255.0f);
                    pixels[i].g = (unsigned char)(((float* )image.data)[k + 1]*255.0f);
                    pixels[i].b = (unsigned char)(((float* )image.data)[k + 2]*255.0f);
                    pixels[i].a = 255;

                    k += 3;
                } break;
                case RF_UNCOMPRESSED_R32G32B32A32:
                {
                    pixels[i].r = (unsigned char)(((float* )image.data)[k]*255.0f);
                    pixels[i].g = (unsigned char)(((float* )image.data)[k]*255.0f);
                    pixels[i].b = (unsigned char)(((float* )image.data)[k]*255.0f);
                    pixels[i].a = (unsigned char)(((float* )image.data)[k]*255.0f);

                    k += 4;
                } break;
                default: break;
            }
        }
    }

    return pixels;
}

// Get pixel data from image as rf_vec4 array (float normalized)
RF_API rf_vec4* rf_get_image_data_normalized(rf_image image, rf_allocator allocator)
{
    rf_vec4*  pixels = (rf_vec4*) RF_ALLOC(allocator, image.width * image.height * sizeof(rf_vec4));

    if (image.format >= RF_COMPRESSED_DXT1_RGB) RF_LOG_V(RF_LOG_WARNING, "Pixel data retrieval not supported for compressed image formats");
    else
    {
        for (int i = 0, k = 0; i < image.width*image.height; i++)
        {
            switch (image.format)
            {
                case RF_UNCOMPRESSED_GRAYSCALE:
                {
                    pixels[i].x = (float)((unsigned char* )image.data)[i]/255.0f;
                    pixels[i].y = (float)((unsigned char* )image.data)[i]/255.0f;
                    pixels[i].z = (float)((unsigned char* )image.data)[i]/255.0f;
                    pixels[i].w = 1.0f;

                } break;
                case RF_UNCOMPRESSED_GRAY_ALPHA:
                {
                    pixels[i].x = (float)((unsigned char* )image.data)[k]/255.0f;
                    pixels[i].y = (float)((unsigned char* )image.data)[k]/255.0f;
                    pixels[i].z = (float)((unsigned char* )image.data)[k]/255.0f;
                    pixels[i].w = (float)((unsigned char* )image.data)[k + 1]/255.0f;

                    k += 2;
                } break;
                case RF_UNCOMPRESSED_R5G5B5A1:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].x = (float)((pixel & 0b1111100000000000) >> 11)*(1.0f/31);
                    pixels[i].y = (float)((pixel & 0b0000011111000000) >> 6)*(1.0f/31);
                    pixels[i].z = (float)((pixel & 0b0000000000111110) >> 1)*(1.0f/31);
                    pixels[i].w = ((pixel & 0b0000000000000001) == 0)? 0.0f : 1.0f;

                } break;
                case RF_UNCOMPRESSED_R5G6B5:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].x = (float)((pixel & 0b1111100000000000) >> 11)*(1.0f/31);
                    pixels[i].y = (float)((pixel & 0b0000011111100000) >> 5)*(1.0f/63);
                    pixels[i].z = (float)(pixel & 0b0000000000011111)*(1.0f/31);
                    pixels[i].w = 1.0f;

                } break;
                case RF_UNCOMPRESSED_R4G4B4A4:
                {
                    unsigned short pixel = ((unsigned short *)image.data)[i];

                    pixels[i].x = (float)((pixel & 0b1111000000000000) >> 12)*(1.0f/15);
                    pixels[i].y = (float)((pixel & 0b0000111100000000) >> 8)*(1.0f/15);
                    pixels[i].z = (float)((pixel & 0b0000000011110000) >> 4)*(1.0f/15);
                    pixels[i].w = (float)(pixel & 0b0000000000001111)*(1.0f/15);

                } break;
                case RF_UNCOMPRESSED_R8G8B8A8:
                {
                    pixels[i].x = (float)((unsigned char* )image.data)[k]/255.0f;
                    pixels[i].y = (float)((unsigned char* )image.data)[k + 1]/255.0f;
                    pixels[i].z = (float)((unsigned char* )image.data)[k + 2]/255.0f;
                    pixels[i].w = (float)((unsigned char* )image.data)[k + 3]/255.0f;

                    k += 4;
                } break;
                case RF_UNCOMPRESSED_R8G8B8:
                {
                    pixels[i].x = (float)((unsigned char* )image.data)[k]/255.0f;
                    pixels[i].y = (float)((unsigned char* )image.data)[k + 1]/255.0f;
                    pixels[i].z = (float)((unsigned char* )image.data)[k + 2]/255.0f;
                    pixels[i].w = 1.0f;

                    k += 3;
                } break;
                case RF_UNCOMPRESSED_R32:
                {
                    pixels[i].x = ((float* )image.data)[k];
                    pixels[i].y = 0.0f;
                    pixels[i].z = 0.0f;
                    pixels[i].w = 1.0f;

                } break;
                case RF_UNCOMPRESSED_R32G32B32:
                {
                    pixels[i].x = ((float* )image.data)[k];
                    pixels[i].y = ((float* )image.data)[k + 1];
                    pixels[i].z = ((float* )image.data)[k + 2];
                    pixels[i].w = 1.0f;

                    k += 3;
                } break;
                case RF_UNCOMPRESSED_R32G32B32A32:
                {
                    pixels[i].x = ((float* )image.data)[k];
                    pixels[i].y = ((float* )image.data)[k + 1];
                    pixels[i].z = ((float* )image.data)[k + 2];
                    pixels[i].w = ((float* )image.data)[k + 3];

                    k += 4;
                } break;
                default: break;
            }
        }
    }

    return pixels;
}

// Extract color palette from image to maximum size.
RF_API rf_color* rf_image_extract_palette(rf_image image, int max_palette_size, int* extract_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_color* pixels = rf_get_image_pixel_data(image, temp_allocator);
    rf_color* palette = (rf_color*) RF_ALLOC(allocator, max_palette_size * sizeof(rf_color));

    int pal_count = 0;
    for (int i = 0; i < max_palette_size; i++)
    {
        palette[i] = RF_BLANK; // Set all colors to RF_BLANK
    }

    for (int i = 0; i < image.width*image.height; i++)
    {
        if (pixels[i].a > 0)
        {
            bool color_in_palette = false;

            // Check if the color is already on palette
            for (int j = 0; j < max_palette_size; j++)
            {
                //If the colors are equal
                if (pixels[i].r == palette[j].r &&
                    pixels[i].g == palette[j].g &&
                    pixels[i].b == palette[j].b &&
                    pixels[i].a == palette[j].a)
                {
                    color_in_palette = true;
                    break;
                }
            }

            // Store color if not on the palette
            if (!color_in_palette)
            {
                palette[pal_count] = pixels[i]; // Add pixels[i] to palette
                pal_count++;

                // We reached the limit of colors supported by palette
                if (pal_count >= max_palette_size)
                {
                    i = image.width * image.height; // Finish palette get
                    RF_LOG_V(RF_LOG_WARNING, "rf_image palette is greater than %i colors!", max_palette_size);
                }
            }
        }
    }

    RF_FREE(temp_allocator, pixels);

    *extract_count = pal_count;

    return palette;
}
//endregion

//region loading & unloading functions
// Load image from file into CPU memory (RAM)
RF_API rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_image image = { 0 };

    if ((   _rf_is_file_extension(filename, ".png"))
        || (_rf_is_file_extension(filename, ".bmp"))
        || (_rf_is_file_extension(filename, ".tga"))
        || (_rf_is_file_extension(filename, ".gif"))
        || (_rf_is_file_extension(filename, ".pic"))
        || (_rf_is_file_extension(filename, ".psd")))
    {

        int img_width = 0;
        int img_height = 0;
        int img_bpp = 0;

        int file_size = io.get_file_size_cb(filename);
        unsigned char* image_file_buffer = (unsigned char*) RF_ALLOC(temp_allocator, file_size);
        io.read_file_into_buffer_cb(filename, image_file_buffer, file_size);

        if (image_file_buffer != NULL)
        {
            // NOTE: Using stb_image to load images (Supports multiple image formats)
            RF_WITH_STBI_ALLOCATOR(&allocator,
            {
                image.data = stbi_load_from_memory(image_file_buffer, file_size, &img_width, &img_height, &img_bpp, 0);
            });

            image.width     = img_width;
            image.height    = img_height;
            image.mipmaps   = 1;
            image.allocator = allocator;

            if (img_bpp == 1)      image.format = RF_UNCOMPRESSED_GRAYSCALE;
            else if (img_bpp == 2) image.format = RF_UNCOMPRESSED_GRAY_ALPHA;
            else if (img_bpp == 3) image.format = RF_UNCOMPRESSED_R8G8B8;
            else if (img_bpp == 4) image.format = RF_UNCOMPRESSED_R8G8B8A8;
        }

        RF_FREE(temp_allocator, image_file_buffer);
    }
    else
    {
        RF_LOG_V(RF_LOG_WARNING, "[%s] rf_image fileformat not supported", filename);
    }

    if (image.data != NULL) RF_LOG_V(RF_LOG_INFO, "[%s] rf_image loaded successfully (%ix%i)", filename, image.width, image.height);
    else RF_LOG_V(RF_LOG_WARNING, "[%s] rf_image could not be loaded", filename);

    return image;
}

// Load image from file data into CPU memory (RAM)
RF_API rf_image rf_load_image_from_data(void* data, int data_size, rf_allocator allocator)
{
    if (data == NULL || data_size == 0) return (rf_image) { 0 };

    int img_width   = 0;
    int img_height  = 0;
    int img_bpp     = 0;
    rf_image image = { 0 };

    // NOTE: Using stb_image to load images (Supports multiple image formats)
    RF_WITH_STBI_ALLOCATOR(&allocator,
    {
        image.data = stbi_load_from_memory(data, data_size, &img_width, &img_height, &img_bpp, 0);
    });

    image.width     = img_width;
    image.height    = img_height;
    image.mipmaps   = 1;
    image.allocator = allocator;

    if (img_bpp == 1)      image.format = RF_UNCOMPRESSED_GRAYSCALE;
    else if (img_bpp == 2) image.format = RF_UNCOMPRESSED_GRAY_ALPHA;
    else if (img_bpp == 3) image.format = RF_UNCOMPRESSED_R8G8B8;
    else if (img_bpp == 4) image.format = RF_UNCOMPRESSED_R8G8B8A8;

    if (image.data == NULL)
    {
        RF_LOG(RF_LOG_WARNING, "rf_image fileformat not supported or could not be loaded");
    }

    return image;
}

// Load image from rf_color array data (RGBA - 32bit)
RF_API rf_image rf_load_image_from_pixels(rf_color* pixels, int width, int height, rf_allocator allocator)
{
    rf_image image;
    image.data      = NULL;
    image.width     = width;
    image.height    = height;
    image.mipmaps   = 1;
    image.format    = RF_UNCOMPRESSED_R8G8B8A8;
    image.allocator = allocator;

    int k = 0;

    image.data = (unsigned char*) RF_ALLOC(allocator,image.width * image.height * 4 * sizeof(unsigned char));

    for (int i = 0; i < image.width * image.height * 4; i += 4)
    {
        ((unsigned char* )image.data)[i    ] = pixels[k].r;
        ((unsigned char* )image.data)[i + 1] = pixels[k].g;
        ((unsigned char* )image.data)[i + 2] = pixels[k].b;
        ((unsigned char* )image.data)[i + 3] = pixels[k].a;
        k++;
    }

    return image;
}

// Load image from raw data with parameters
RF_API rf_image rf_load_image_from_data_with_params(void* data, int data_size, int width, int height, int format, rf_allocator allocator)
{
    rf_image src_image = { 0 };

    src_image.data = ((void*)data); //Safe const-cast
    src_image.width = width;
    src_image.height = height;
    src_image.mipmaps = 1;
    src_image.format = format;

    rf_image dst_image = rf_image_copy(src_image, allocator);

    return dst_image;
}

// Unloads the image using its allocator
RF_API void rf_unload_image(rf_image image)
{
    RF_FREE(image.allocator, image.data);
}
//endregion

//region basic image manipulation
// Copy an image to a new image
RF_API rf_image rf_image_copy(rf_image image, rf_allocator allocator)
{
    rf_image new_image = { 0 };
    new_image.allocator = allocator;

    int width = image.width;
    int height = image.height;
    int size = 0;

    for (int i = 0; i < image.mipmaps; i++)
    {
        size += rf_get_buffer_size_for_pixel_format(width, height, image.format);

        width /= 2;
        height /= 2;

        // Security check for NPOT textures
        if (width < 1) width = 1;
        if (height < 1) height = 1;
    }

    new_image.data = RF_ALLOC(allocator, size);

    if (new_image.data != NULL)
    {
        // NOTE: Size must be provided in bytes
        memcpy(new_image.data, image.data, size);

        new_image.width   = image.width;
        new_image.height  = image.height;
        new_image.mipmaps = image.mipmaps;
        new_image.format  = image.format;
    }

    return new_image;
}

// Create an image from another image piece
RF_API rf_image rf_image_from_image(rf_image image, rf_rec rec, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = rf_image_copy(image, allocator);

    rf_image_crop(&result, rec, temp_allocator);

    return result;
}

// Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
RF_API void rf_image_gen_mipmaps(rf_image* image, rf_allocator temp_allocator)
{
    // Security check to avoid program crash
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;

    int mip_count = 1; // Required mipmap levels count (including base level)
    int mip_width = image->width; // Base image width
    int mip_height = image->height; // Base image height
    int mip_size = rf_get_buffer_size_for_pixel_format(mip_width, mip_height, image->format); // rf_image data size (in bytes)

    // Count mipmap levels required
    while ((mip_width != 1) || (mip_height != 1))
    {
        if (mip_width != 1) mip_width /= 2;
        if (mip_height != 1) mip_height /= 2;

        // Security check for NPOT textures
        if (mip_width < 1) mip_width = 1;
        if (mip_height < 1) mip_height = 1;

        RF_LOG_V(RF_LOG_DEBUG, "Next mipmap level: %i x %i - current size %i", mip_width, mip_height, mip_size);

        mip_count++;
        mip_size += rf_get_buffer_size_for_pixel_format(mip_width, mip_height, image->format); // Add mipmap size (in bytes)
    }

    RF_LOG_V(RF_LOG_DEBUG, "Mipmaps available: %i - Mipmaps required: %i", image->mipmaps, mip_count);
    RF_LOG_V(RF_LOG_DEBUG, "Mipmaps total size required: %i", mip_size);
    RF_LOG_V(RF_LOG_DEBUG, "rf_image data memory start address: 0x%x", image->data);

    if (image->mipmaps >= mip_count)
    {
        RF_LOG_V(RF_LOG_WARNING, "rf_image mipmaps already available");
        return;
    }

    void* temp = _rf_realloc_wrapper(image->allocator, image->data, rf_image_size(*image), mip_size);

    if (temp != NULL)
    {
        image->data = temp; // Assign new pointer (new size) to store mipmaps data
        RF_LOG_V(RF_LOG_DEBUG, "rf_image data memory point reallocated: 0x%x", temp);
    }
    else RF_LOG_V(RF_LOG_WARNING, "Mipmaps required memory could not be allocated");

    // Pointer to allocated memory point where store next mipmap level data
    unsigned char* nextmip = (unsigned char*) image->data + rf_get_buffer_size_for_pixel_format(image->width, image->height, image->format);

    mip_width  = image->width / 2;
    mip_height = image->height / 2;
    mip_size   = rf_get_buffer_size_for_pixel_format(mip_width, mip_height, image->format);

    //Looks like a good candidate to remove the temporary allocation
    rf_image im_copy = rf_image_copy(*image, temp_allocator);

    for (int i = 1; i < mip_count; i++)
    {
        RF_LOG_V(RF_LOG_DEBUG, "Gen mipmap level: %i (%i x %i) - size: %i - offset: 0x%x", i, mip_width, mip_height, mip_size, nextmip);

        rf_image_resize(&im_copy, mip_width, mip_height, temp_allocator); // Uses internally Mitchell cubic downscale filter

        memcpy(nextmip, im_copy.data, mip_size);
        nextmip += mip_size;
        image->mipmaps++;

        mip_width /= 2;
        mip_height /= 2;

        // Security check for NPOT textures
        if (mip_width < 1) mip_width = 1;
        if (mip_height < 1) mip_height = 1;

        mip_size = rf_get_buffer_size_for_pixel_format(mip_width, mip_height, image->format);
    }

    rf_unload_image(im_copy);
}

RF_API void rf_image_to_pot(rf_image* image, rf_color fill_color, rf_allocator temp_allocator)
{
    // Security check to avoid program crash
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;

    rf_color* pixels = rf_get_image_pixel_data(*image, temp_allocator); // Get pixels data

    // Calculate next power-of-two values
    // NOTE: Just add the required amount of pixels at the right and bottom sides of image...
    int pot_width  = (int) powf(2, ceilf(logf((float)image->width)/logf(2)));
    int pot_height = (int) powf(2, ceilf(logf((float)image->height)/logf(2)));

    // Check if POT image generation is required (if texture is not already POT)
    if ((pot_width == image->width) && (pot_height == image->height)) return;

    rf_color* pixels_pot = NULL;

    // Generate POT array from NPOT data
    pixels_pot = (rf_color*) RF_ALLOC(temp_allocator, pot_width * pot_height * sizeof(rf_color));

    for (int j = 0; j < pot_height; j++)
    {
        for (int i = 0; i < pot_width; i++)
        {
            if ((j < image->height) && (i < image->width)) pixels_pot[j*pot_width + i] = pixels[j*image->width + i];
            else pixels_pot[j*pot_width + i] = fill_color;
        }
    }

    RF_LOG_V(RF_LOG_WARNING, "rf_image converted to POT: (%ix%i) -> (%ix%i)", image->width, image->height, pot_width, pot_height);

    RF_FREE(temp_allocator, pixels); // Free pixels data
    RF_FREE(temp_allocator, image->data); // Free old image data

    int format = image->format; // Store image data format to reconvert later

    // NOTE: rf_image size changes, new width and height
    *image = rf_load_image_from_pixels(pixels_pot, pot_width, pot_height, image->allocator);

    RF_FREE(temp_allocator, pixels_pot); // Free POT pixels data

    rf_image_format(image, format, temp_allocator); // Reconvert image to previous format
}

// Convert image data to desired format
RF_API void rf_image_format(rf_image* image, int new_format, rf_allocator temp_allocator)
{
    // Security check to avoid program crash
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;

    if ((new_format != 0) && (image->format != new_format))
    {
        if ((image->format < RF_COMPRESSED_DXT1_RGB) && (new_format < RF_COMPRESSED_DXT1_RGB))
        {
            rf_vec4* pixels = rf_get_image_data_normalized(*image, temp_allocator); // Supports 8 to 32 bit per channel

            RF_FREE(image->allocator, image->data); // WARNING! We loose mipmaps data --> Regenerated at the end...
            image->data = NULL;
            image->format = new_format;

            int k = 0;

            switch (image->format)
            {
                case RF_UNCOMPRESSED_GRAYSCALE:
                {
                    image->data = (unsigned char*) RF_ALLOC(image->allocator, image->width*image->height * sizeof(unsigned char));

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        ((unsigned char* )image->data)[i] = (unsigned char)((pixels[i].x*0.299f + pixels[i].y*0.587f + pixels[i].z*0.114f)*255.0f);
                    }

                } break;
                case RF_UNCOMPRESSED_GRAY_ALPHA:
                {
                    image->data = (unsigned char*) RF_ALLOC(image->allocator, image->width*image->height * 2 * sizeof(unsigned char));

                    for (int i = 0; i < image->width*image->height * 2; i += 2, k++)
                    {
                        ((unsigned char* )image->data)[i] = (unsigned char)((pixels[k].x*0.299f + (float)pixels[k].y*0.587f + (float)pixels[k].z*0.114f)*255.0f);
                        ((unsigned char* )image->data)[i + 1] = (unsigned char)(pixels[k].w*255.0f);
                    }

                } break;
                case RF_UNCOMPRESSED_R5G6B5:
                {
                    image->data = (unsigned short*) RF_ALLOC(image->allocator, image->width*image->height * sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        r = (unsigned char)(round(pixels[i].x * 31.0f));
                        g = (unsigned char)(round(pixels[i].y*63.0f));
                        b = (unsigned char)(round(pixels[i].z * 31.0f));

                        ((unsigned short *)image->data)[i] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
                    }

                } break;
                case RF_UNCOMPRESSED_R8G8B8:
                {
                    image->data = (unsigned char*) RF_ALLOC(image->allocator, image->width*image->height * 3 * sizeof(unsigned char));

                    for (int i = 0, kk = 0; i < image->width * image->height * 3; i += 3, kk++)
                    {
                        ((unsigned char* )image->data)[i] = (unsigned char)(pixels[kk].x * 255.0f);
                        ((unsigned char* )image->data)[i + 1] = (unsigned char)(pixels[kk].y * 255.0f);
                        ((unsigned char* )image->data)[i + 2] = (unsigned char)(pixels[kk].z * 255.0f);
                    }
                } break;
                case RF_UNCOMPRESSED_R5G5B5A1:
                {
                    int ALPHA_THRESHOLD = 50;

                    image->data = (unsigned short*) RF_ALLOC(image->allocator, image->width*image->height * sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 0;

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        r = (unsigned char)(round(pixels[i].x * 31.0f));
                        g = (unsigned char)(round(pixels[i].y * 31.0f));
                        b = (unsigned char)(round(pixels[i].z * 31.0f));
                        a = (pixels[i].w > ((float)ALPHA_THRESHOLD/255.0f))? 1 : 0;

                        ((unsigned short *)image->data)[i] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
                    }

                } break;
                case RF_UNCOMPRESSED_R4G4B4A4:
                {
                    image->data = (unsigned short*) RF_ALLOC(image->allocator, image->width*image->height * sizeof(unsigned short));

                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 0;

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        r = (unsigned char)(round(pixels[i].x*15.0f));
                        g = (unsigned char)(round(pixels[i].y*15.0f));
                        b = (unsigned char)(round(pixels[i].z*15.0f));
                        a = (unsigned char)(round(pixels[i].w*15.0f));

                        ((unsigned short *)image->data)[i] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
                    }

                } break;
                case RF_UNCOMPRESSED_R8G8B8A8:
                {
                    image->data = (unsigned char*) RF_ALLOC(image->allocator, image->width*image->height * 4 * sizeof(unsigned char));

                    for (int i = 0, kk = 0; i < image->width * image->height * 4; i += 4, kk++)
                    {
                        ((unsigned char* )image->data)[i] = (unsigned char)(pixels[kk].x * 255.0f);
                        ((unsigned char* )image->data)[i + 1] = (unsigned char)(pixels[kk].y * 255.0f);
                        ((unsigned char* )image->data)[i + 2] = (unsigned char)(pixels[kk].z * 255.0f);
                        ((unsigned char* )image->data)[i + 3] = (unsigned char)(pixels[kk].w * 255.0f);
                    }
                } break;
                case RF_UNCOMPRESSED_R32:
                {
                    // WARNING: rf_image is converted to GRAYSCALE eqeuivalent 32bit

                    image->data = (float*) RF_ALLOC(image->allocator, image->width*image->height * sizeof(float));

                    for (int i = 0; i < image->width*image->height; i++)
                    {
                        ((float* )image->data)[i] = (float)(pixels[i].x*0.299f + pixels[i].y*0.587f + pixels[i].z*0.114f);
                    }
                } break;
                case RF_UNCOMPRESSED_R32G32B32:
                {
                    image->data = (float*) RF_ALLOC(image->allocator, image->width*image->height * 3 * sizeof(float));

                    for (int i = 0, kk = 0; i < image->width * image->height * 3; i += 3, kk++)
                    {
                        ((float* )image->data)[i] = pixels[kk].x;
                        ((float* )image->data)[i + 1] = pixels[kk].y;
                        ((float* )image->data)[i + 2] = pixels[kk].z;
                    }
                } break;
                case RF_UNCOMPRESSED_R32G32B32A32:
                {
                    image->data = (float*) RF_ALLOC(image->allocator, image->width*image->height * 4 * sizeof(float));

                    for (int i = 0, kk = 0; i < image->width * image->height * 4; i += 4, kk++)
                    {
                        ((float* )image->data)[i] = pixels[kk].x;
                        ((float* )image->data)[i + 1] = pixels[kk].y;
                        ((float* )image->data)[i + 2] = pixels[kk].z;
                        ((float* )image->data)[i + 3] = pixels[kk].w;
                    }
                } break;
                default: break;
            }

            RF_FREE(temp_allocator, pixels);
            pixels = NULL;

            // In case original image had mipmaps, generate mipmaps for formated image
            // NOTE: Original mipmaps are replaced by new ones, if custom mipmaps were used, they are lost
            if (image->mipmaps > 1)
            {
                image->mipmaps = 1;

                if (image->data != NULL) rf_image_gen_mipmaps(image, temp_allocator);

            }
        }
        else RF_LOG_V(RF_LOG_WARNING, "rf_image data format is compressed, can not be converted");
    }
}

// Crop an image to area defined by a rectangle
RF_API void rf_image_crop(rf_image* image, rf_rec crop, rf_allocator temp_allocator)
{
    // Security check to avoid program crash
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0)) return;

    // Security checks to validate crop rectangle
    if (crop.x < 0) { crop.width += crop.x; crop.x = 0; }
    if (crop.y < 0) { crop.height += crop.y; crop.y = 0; }
    if ((crop.x + crop.width) > image->width) crop.width = image->width - crop.x;
    if ((crop.y + crop.height) > image->height) crop.height = image->height - crop.y;

    if ((crop.x < image->width) && (crop.y < image->height))
    {
        // Start the cropping process
        rf_color* pixels = rf_get_image_pixel_data(*image, temp_allocator); // Get data as rf_color pixels array
        rf_color* crop_pixels = (rf_color*) RF_ALLOC(temp_allocator, (int)crop.width*(int)crop.height * sizeof(rf_color));

        for (int j = (int)crop.y; j < (int)(crop.y + crop.height); j++)
        {
            for (int i = (int)crop.x; i < (int)(crop.x + crop.width); i++)
            {
                crop_pixels[(j - (int)crop.y)*(int)crop.width + (i - (int)crop.x)] = pixels[j*image->width + i];
            }
        }

        RF_FREE(temp_allocator, pixels);

        int format = image->format;

        RF_FREE(image->allocator, image->data);

        *image = rf_load_image_from_pixels(crop_pixels, (int)crop.width, (int)crop.height, image->allocator);

        RF_FREE(temp_allocator, crop_pixels);

        // Reformat 32bit RGBA image to original format
        rf_image_format(image, format, temp_allocator);
    }
    else RF_LOG_V(RF_LOG_WARNING, "rf_image can not be cropped, crop rectangle out of bounds");
}
//endregion