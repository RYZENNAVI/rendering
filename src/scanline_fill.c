/*
 * @file scanline_fill.c
 * @brief Scanline polygon fill implementation.
 *
 * This file provides a simple scanline-based algorithm to fill a closed polygon.
 * It computes horizontal spans for each scanline and fills pixels in between.
 * 
 * @authors Team 3
 * @date 2025-07-04
 */

#include "../inc/scanline_fill.h"
#include <math.h>
#include "../inc/common.h"

// filling a polygon using the scanline algorithm
void fill_polygon_scanline(point_t *pts, int n_pts, image_t *img, color_t fill)
{
    for (int y = 0; y < img->height; ++y)
    {
        double xs[64];
        int n_xs = 0;
        
        // Collect all x-intersections for current scanline
        for (int i = 0; i < n_pts; ++i)
        {
            point_t a = pts[i];
            point_t b = pts[(i + 1) % n_pts];

            if ((a.y <= y && b.y > y) || (b.y <= y && a.y > y))
            {
                double t = (y - a.y) / (b.y - a.y);
                double x_cross = a.x + t * (b.x - a.x);
                xs[n_xs++] = x_cross;
            }
        }

        // Sort intersection x-coordinates (bubble sort)
        for (int i = 0; i < n_xs - 1; ++i)
            for (int j = i + 1; j < n_xs; ++j)
                if (xs[j] < xs[i])
                {
                    double tmp = xs[i];
                    xs[i] = xs[j];
                    xs[j] = tmp;
                }

        // Fill between pairs of intersections
        for (int i = 0; i + 1 < n_xs; i += 2)
        {
            int x0 = (int)ceil(xs[i]);
            int x1 = (int)floor(xs[i + 1]);
            for (int x = x0; x <= x1; ++x)
            {
                if (x >= 0 && x < img->width)
                    if (img != NULL && img->data != NULL)
                        set_pixel(img, x, y, fill);
            }
        }
    }
}
