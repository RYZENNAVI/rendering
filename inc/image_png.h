/**
 * @file image_png.h
 * @brief Declares PNG export function using libpng for saving RGBA image buffers.
 *
 * This module is responsible for writing image data in image_t to a .png file.
 *
 * Preconditions, postconditions and invariants are described for the function.
 *
 * @author Team 3
 * @date 2025-07-01
 */

#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#include "common.h"

/**
 * @brief Saves an image buffer as a PNG file using libpng.
 *
 * @param img Pointer to the image structure containing RGBA data.
 * @param filename Name (and path) of the PNG file to write.
 * 
 * @pre
 * - `img != NULL`
 * - `img->data != NULL`
 * - `filename != NULL`
 *
 * @invariant
 * - Image buffer must remain valid throughout execution.
 * - No change to input buffer contents.
 * - Output format is 8-bit RGBA.
 *
 * @post
 * - File is written at specified path if writable.
 * - libpng structures are properly freed.
 */
void save_png(image_t *img, const char *filename);

#endif