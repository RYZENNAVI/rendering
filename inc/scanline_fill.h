/**
 * @file scanline_fill.h
 * @brief Scanline polygon fill routine for image_t using color_t.
 *
 * This module provides a function to fill polygons by horizontal scanlines.
 *
 * Preconditions, postconditions and invariants are described for the function.
 *
 * @author Team 3
 * @date 2025-07-01
 */

#ifndef SCANLINE_FILL_H
#define SCANLINE_FILL_H

#include "common.h"

/**
 * @brief Fills a polygon into the image using scanline rasterization.
 *
 * @param pts Array of polygon vertices (must form a closed shape).
 * @param n_pts Number of vertices (must be >= 3).
 * @param img Pointer to target image buffer.
 * @param fill Fill color (RGBA).
 * 
 * @pre
 * - `pts != NULL`
 * - `n_pts >= 3`
 * - `img != NULL`
 *
 * @invariant
 * - Polygon edges form a closed shape.
 * - Scanlines computed from edge intersections.
 * - Memory in `img` must remain valid.
 *
 * @post
 * - Pixels inside the polygon are colored with `fill`.
 * - Image buffer remains allocated.
 */
void fill_polygon_scanline(point_t *pts, int n_pts, image_t *img, color_t fill);

#endif