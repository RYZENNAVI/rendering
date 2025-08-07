// #include "image.c" // oder image.h from Team 2 - Rasterization

/*
 * @file test_shapes_to_png.c
 * @brief Test program for PNG image creation (Team 3)
 *
 * This file contains a simple test that creates a small 100x100 image
 * and draws a diagonal green line using the RGBA color model.
 * The image is then saved as a PNG file to verify the PNG export function.
 *
 * @authors Team 3
 * @date 2025-07-04
 *
 * This module demonstrates usage of the common output API:
 * - image creation
 * - pixel coloring
 * - saving to PNG
 */

#include "image_png.h"
#include "common.h"
#include <stdio.h>

/**
 * @brief Entry point for PNG creation test.
 *
 * This test creates a 100x100 image, colors a diagonal with a
 * bright green color, and saves the result to "test_out.png".
 */
int main(void)
{
    // Create test image of size 100x100
    image_t *img = create_image(100, 100);

    // Draw a diagonal line using set_pixel
    for (int i = 0; i < 100; ++i)
        set_pixel(img, i, i, (color_t){105, 255, 0, 255});

    // Save image as PNG
    save_png(img, "test_out.png");

    // Cleanup
    free_image(img);
    return 0;
}