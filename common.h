/*
@file common.h
@authors Matthias, Toni, Fatima, Samuel, André, Jannis, Li
@date 09.06.25
*/

#ifndef COMMON_H
#define COMMON_H

#include "bezier.h"
#include <png.h>
#include <stdint.h>

enum axis_t { X, Y, Z };

/* Color represented as rgba */
typedef struct {
    uint8_t red;   /* red   component (0-255) */
    uint8_t green; /* green component (0-255) */
    uint8_t blue;  /* blue  component (0-255) */
    uint8_t alpha; /* alpha component (0-255) */
} color_t;

/* x and y are coordinates of a point */
typedef struct {
    double x;
    double y;
} point_t;

/* Quadratic bezier curve consists of control point, end */
typedef struct {
    point_t control;
    point_t end;
} quadratic_curve_t;

/* Cubic bezier curve consists of end, first (c1) and second (c2) control
 * point */
typedef struct {
    point_t end;
    point_t c1;
    point_t c2;
} cubic_curve_t;

/* Bezier curves that are returned have start explicitly set */
typedef struct {
    point_t start;
    point_t end;
    point_t c1;
    point_t c2;
} return_cubic_t;

/* One brush stroke consists of an array of cubic bezier curves, the length of
 * the array and a color */
typedef struct {
    return_cubic_t beziers[TRACE_MAX];
    color_t color;
    size_t length; // length =< TRACE_MAX
} brush_stroke_t;

/* Should only be used for verified Brushes (constructed with brush_make) */
typedef struct {
    knot_t *knots;
    color_t color;
} brush_t;

/* Initialize knots and move brush to specified point */
knot_t *moveto(point_t point);

/* Initialize knots and move brush relative to specified point */
knot_t *rmoveto(knot_t *old, double x, double y);

/* Append a line made of one additional point to a collection of knots */
knot_t *lineto(knot_t *knots, point_t point);

/* Append a line, with an endpoint specified relative to the last endpoint in
 * the path */
knot_t *rlineto(knot_t *knots, double x, double y);

/* Append a quadratic bezier made of three points to a collection of knots.
   If the start point of the quadratic curve does not match the coordinates
   of the last knot, an empty knot list will be returned. */
knot_t *quadraticto(knot_t *knots, quadratic_curve_t curve);

/* Closes the current path (if not already closed) with a straight
   line segment. */
knot_t *closepath(knot_t *knots);

/* Creates brush from knots and verifies it */
brush_t *brush_make(color_t color);

/* Creates square-shaped brush from knots and verifies it */
brush_t *brush_make_square(color_t color);

/* Should support all operations that can be done with matrix, such as resize,
   rotate, translate, slant, etc. At least translation requires a 3x3 matrix,
   even for 2D coordinates, thus we use a 3x3 matrix. Is chainable with other
   transforms. */
void brush_tf(brush_t *brush, double matrix[3][3]);

/* Scales the brush by independent factors along x and y. Calls
   brush_transform, but makes it easier for user to interface with matrices.
   Both are exposed though. Is chainable with other transforms. */
void brush_tf_resize(brush_t *brush, double scale[2][2], int rotation);

/* Rotates the brush by theta degrees. Calls brush_transform, but makes it
   easier for user to interface with matrices. Both are exposed though. Is
   chainable with other transforms. */
void brush_tf_rotate(brush_t *brush, double theta, enum axis_t axis);

/* Flip the brush on the specified axis. Calls brush_transform, but makes it
   easier for user to interface with matrices. Both are exposed though. Is
   chainable with other transforms. */
void brush_tf_reflect(brush_t *brush, enum axis_t axis);

/* Translate the brush midpoint to the coordinate specified. Calls
   brush_transform, but makes it easier for user to interface with matrices.
   Both are exposed though. Is chainable with other transforms. */
void brush_tf_translate(brush_t *brush, point_t dest);

/* Shears the brush by the specified factor on the specified axis. Calls
   brush_transform, but makes it easier for user to interface with matrices.
   Both are exposed though. Is chainable with other transforms. */
void brush_tf_shear(brush_t *brush, double shear, enum axis_t axis);

/* Moves the brush to the first knot of the path and draws the shape. The
   brush stroke has to be initialized first and given as a parameter. */
brush_stroke_t *draw_shape(knot_t *path, brush_t *brush,
                           brush_stroke_t *stroke);

/**
 * @brief Initializes internal RGBA pixel buffer.
 *
 * @param width  Image width in pixels
 * @param height Image height in pixels
 */
void init_buffer(int width, int height);

/**
 * @brief Writes buffer contents into a PNG image file.
 *
 * @param filename Output filename (e.g., "output.png")
 */
void write_png(const char *filename);

/**
 * @brief Frees the internal buffer after writing.
 */
void free_buffer(void);

/**
 * @brief Draws an array of spans into the pixel buffer.
 *
 * This function must be finalized in agreement with Team 2 to match
 * their span data structure and format.
 *
 * @param spans Array of span structures
 * @param n_spans Number of spans in the array
 */
void draw_spans(span spans[], int n_spans);

/*
 * Mixes two colors using additive or subtractive blending.
 *
 * @param color1 Pointer to the first color.
 * @param color2 Pointer to the second color.
 * @param mix A double value:
 *        - Range 0.0 to 1.0: for additive mixing (weighted toward color1)
 *        - Range 0.0 to -1.0: for subtractive mixing (weighted toward color2)
 *
 * @return A newly allocated Color that must be freed by the caller using
 * free().
 *
 * @pre  - `color1 != NULL`
 *       - `color2 != NULL`
 *       - `mix >= -1.0 && mix <= 1.0`
 *
 * @post - Returns a valid pointer to a color_t struct.
 *       - The values of the returned object (red, green, blue, alpha) are in
 * the range [0, 255].
 *
 * @invariant - The input values `color1` and `color2` were not changed.
 *            - The resulting color lies in valid RGBA-colorspace.
 *            - The function does not produce side-effects on global states.
 */
color_t *color_mixer(const color_t *color1, const color_t *color2,
                     const double mix);

#endif // COMMON_H
