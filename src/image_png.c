/*
 * @file image_png.c
 * @brief PNG export implementation (Team 3)
 *
 * This file contains the implementation of the PNG export functionality
 * for converting RGBA pixel data into an image file using libpng.
 *
 * @authors Team 3 - Output & PNG Rendering
 * @date 2025-07-04
 *
 * This module is responsible for the final image output step. It receives
 * an RGBA buffer (wrapped in an image_t struct) and writes the data to disk.
 */

#include "../inc/common.h"
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/image_png.h"

/**
 * @brief Saves an image buffer as a PNG file using libpng.
 *
 * @param img Pointer to image_t containing pixel data and dimensions.
 * @param filename Path and name of the output PNG file.
 *
 * This function initializes libpng, prepares the row pointers from the image
 * buffer, writes the image to disk, and frees all associated resources.
 */
void save_png(image_t *img, const char *filename)
{
    // Open file for binary writing
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Error: Cannot open file %s for writing.\n", filename);
        return;
    }

    // Create PNG write structure
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        fclose(fp);
        return;
    }

    // Create PNG info structure
    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    // Error handling using setjmp/longjmp (libpng convention)
    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    // Initialize output stream
    png_init_io(png, fp);

    // Define image metadata (header)
    png_set_IHDR(
        png, info,
        img->width,             // Width of image
        img->height,            // Height of image
        8,                      // Bit depth per channel
        PNG_COLOR_TYPE_RGBA,    // RGBA format
        PNG_INTERLACE_NONE,     // No interlacing
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);

    // Prepare row pointers (each points to the start of one row in image buffer)
    png_bytep *row_pointers = malloc(sizeof(png_bytep) * img->height);
    for (int y = 0; y < img->height; y++)
    {
        row_pointers[y] = img->data + (y * img->width * 4); // 4 bytes per pixel (RGBA)
    }

    // Assign rows to PNG and write image data
    png_set_rows(png, info, row_pointers);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    // Free resources
    free(row_pointers);
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    // Confirmation
    printf("PNG saved: %s (%dx%d)\n", filename, img->width, img->height);
}
