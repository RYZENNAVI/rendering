/*
@file common.h
@authors Matthias, Toni
@date 01.07.25 (merged)
*/

#ifndef COMMON_H
#define COMMON_H

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// If you need knots_t, include the appropriate header
#include "knots.h"
// If you need additional common definitions, include as needed
// #include "commonadded.h"

#define M_PI 3.14159265358979323846

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

/* Bézier curves that are returned have start explicitly set */
typedef struct {
    point_t start;
    point_t end;
    point_t c1;
    point_t c2;
} return_cubic_t;

/* One brush stroke consists of an array of cubic Bézier curves, the length of
 * the array and a color */
typedef struct {
    return_cubic_t *beziers;
    color_t color;
    size_t length;
} brush_stroke_t;

/* Span represents a horizontal line segment for rasterization */
typedef struct {
    int x_start;   /* Starting x coordinate */
    int x_end;     /* Ending x coordinate */
    int y;         /* Y coordinate of the span */
    color_t color; /* Color of the span */
} span_t;

/* Bounding box structure */
typedef struct {
    double min_x;
    double min_y;
    double max_x;
    double max_y;
} bounding_box_t;

/* List of spans for rasterization output */
typedef struct {
    span_t *spans;
    size_t count;
    size_t capacity;
} span_list_t;

/* List of brush strokes for set operations */
typedef struct {
    brush_stroke_t **strokes;
    size_t count;
    size_t capacity;
} brush_stroke_list_t;

// --- Function declarations (merged, with documentation) ---

// Path and brush construction
knot_t *moveto(point_t point);
knot_t *rmoveto(knot_t *old, point_t p);
knot_t *lineto(knot_t *knots, point_t point);
knot_t *rlineto(knot_t *knots, point_t p);
knot_t *curveto(knot_t *knots, point_t control1, point_t control2,
                point_t end);
knot_t *pathclose(knot_t *knots);

// Brush validation and creation
int brush_make(knot_t *knots);
knot_t *brush_make_square();

// Brush transforms
void brush_tf(knot_t *brush, double matrix[3][3]);
void brush_tf_resize(knot_t *brush, double scale, enum axis_t axis);
void brush_tf_rotate(knot_t *brush, double theta, enum axis_t axis);
void brush_tf_reflect(knot_t *brush, enum axis_t axis);
void brush_tf_translate(knot_t *brush, point_t dest);
void brush_tf_shear(knot_t *brush, double shear, enum axis_t axis);

// Drawing and rasterization
brush_stroke_t *draw_shape(knot_t *path, knot_t *brush, color_t color);
void show_segments(brush_stroke_t *stroke);

/*
 * @brief
 * Rasterizes a brush stroke into a list of spans
 *
 * @param stroke     The brush stroke to rasterize
 * @param resolution The resolution for rasterization (pixels per unit)
 * @return          List of spans representing the rasterized stroke
 * Preconditions:
 * - `stroke != NULL`
 * - `stroke->beziers != NULL` if `stroke->length > 0`
 * - `resolution > 0.0`
 * - `stroke->length` represents valid number of Bézier curves
 * - All Bézier curves in stroke have valid coordinates (no NaN, no infinity)
 * Invariants:
 * - Memory allocation is consistent (no memory leaks)
 * - Span generation maintains geometric accuracy
 * - Color information is preserved throughout processing
 * - Resolution scaling is applied uniformly to all curves
 * Postconditions:
 * - Returns valid `span_list_t*` pointer or `NULL` on failure
 * - Generated spans cover all pixels touched by the stroke
 * - Spans are sorted by y-coordinate, then by x-coordinate
 * - Overlapping spans on same y-line are merged
 * - All spans contain the stroke's color information
 */
span_list_t *rasterize(brush_stroke_t *stroke, double resolution);

/*
 * @brief
 * Unions multiple brush strokes into a single brush stroke
 *
 * @param strokes   List of brush strokes to union
 * @return         Single brush stroke representing the union
 *
 * Preconditions:
 * - `strokes != NULL`
 * - `strokes->count > 0`
 * - All strokes in list are valid (non-NULL)
 * - Each stroke has valid Bézier curve data
 *
 * Invariants:
 * - Total number of Bézier curves = sum of all input strokes
 * - Memory is allocated correctly for combined stroke
 * - No Bézier curves are lost or duplicated
 * - First stroke's color is used as result color
 *
 * Postconditions:
 * - Returns new `brush_stroke_t*` with all input curves combined
 * - Result length equals sum of all input stroke lengths
 * - All Bézier curves from input strokes are preserved
 * - Memory is properly allocated and must be freed by caller
 */
brush_stroke_t *union_brush_stroke(brush_stroke_list_t *strokes);

/*
 * @brief
 * Intersects multiple brush strokes into a single brush stroke
 *
 * @param strokes   List of brush strokes to intersect
 * @return         Single brush stroke representing the intersection
 *
 * Preconditions:
 * - `strokes != NULL`
 * - `strokes->count > 0`
 * - All strokes in list are valid (non-NULL)
 * - Each stroke has valid Bézier curve data and coordinates
 *
 * Invariants:
 * - Uses pairwise intersection algorithm with O(n) complexity
 * - Bounding box tests provide conservative intersection estimates
 * - Memory allocation follows doubling strategy during processing
 * - Original input strokes remain unmodified
 * - Color from first stroke is preserved in result
 *
 * Postconditions:
 * - Returns new `brush_stroke_t*` containing intersection geometry
 * - Result may be empty (length = 0) if no intersection exists
 * - Only Bézier curves with overlapping bounding boxes are included
 * - Memory is properly allocated and must be freed by caller
 * - NULL returned only on allocation failure, not empty intersection
 */
brush_stroke_t *intersection_brush_stroke(brush_stroke_list_t *strokes);

/*
 * @brief
 * Calculates the bounding box of a brush stroke
 *
 * @param stroke   The brush stroke to calculate bounding box for
 * @return        Bounding box of the stroke
 *
 * Preconditions:
 * - `stroke` may be NULL (returns zero-initialized bbox)
 * - If `stroke != NULL`, then `stroke->beziers != NULL` when `stroke->length
 * > 0`
 * - All control points have valid coordinates
 *
 * Invariants:
 * - Bounding box calculation considers all control points
 * - Min/max values are updated correctly during iteration
 * - Coordinate system consistency maintained
 *
 * Postconditions:
 * - Returns `bounding_box_t` with correct min/max coordinates
 * - If stroke is NULL or empty, returns zero-initialized bounding box
 * - Bounding box encompasses all control points of all Bézier curves
 * - `min_x ≤ max_x` and `min_y ≤ max_y` (unless empty)
 */
bounding_box_t bounding_box(brush_stroke_t *stroke);

/* Helper functions for span and brush stroke list management */

/*
 * @brief
 * Creates a new span list with specified initial capacity
 *
 * @param initial_capacity   Initial capacity for the span list
 * @return                  Pointer to new span list or NULL on failure
 *
 * Preconditions:
 * - `initial_capacity > 0`
 * - Sufficient memory available for allocation
 *
 * Invariants:
 * - `count` always ≤ `capacity`
 * - `spans` pointer is valid when `capacity > 0`
 * - Memory allocation is consistent
 *
 * Postconditions:
 * - Returns valid `span_list_t*` or `NULL` on failure
 * - `count = 0`, `capacity = initial_capacity`
 * - Memory for spans array is allocated
 * - List is ready to accept span additions
 */
span_list_t *span_list_create(size_t initial_capacity);

/*
 * @brief
 * Adds a span to the list, expanding capacity if needed
 *
 * @param list   The span list to add to
 * @param span   The span to add
 *
 * Preconditions:
 * - `list != NULL`
 * - `list->spans != NULL`
 * - Span contains valid coordinates and color data
 *
 * Invariants:
 * - Capacity expansion follows doubling strategy
 * - Existing spans are preserved during reallocation
 * - `count` is incremented only on successful addition
 * - Memory consistency maintained
 *
 * Postconditions:
 * - Span is added to list (if memory allows)
 * - `list->count` incremented by 1 on success
 * - Capacity may be doubled if expansion was needed
 * - Order of existing spans is preserved
 */
void span_list_add(span_list_t *list, span_t span);

/*
 * @brief
 * Frees memory allocated for a span list
 *
 * @param list   The span list to free
 *
 * Preconditions:
 * - `list` may be NULL (function handles gracefully)
 * - If `list != NULL`, it must be validly allocated
 *
 * Invariants:
 * - All allocated memory is freed
 * - No memory leaks occur
 * - Function is safe to call multiple times
 *
 * Postconditions:
 * - All memory associated with list is freed
 * - `list` pointer becomes invalid after call
 * - Function handles NULL input gracefully
 */
void span_list_free(span_list_t *list);

/*
 * @brief
 * Creates a new brush stroke list with specified capacity
 *
 * @param initial_capacity   Initial capacity for the brush stroke list
 * @return                  Pointer to new brush stroke list or NULL on
 * failure
 *
 * Preconditions:
 * - `initial_capacity > 0`
 * - Sufficient memory available
 *
 * Invariants:
 * - `count ≤ capacity` always maintained
 * - Memory allocation follows standard patterns
 * - Pointer array is properly allocated
 *
 * Postconditions:
 * - Returns valid list pointer or NULL on failure
 * - List initialized with zero count
 * - Ready to accept brush stroke pointers
 */
brush_stroke_list_t *brush_stroke_list_create(size_t initial_capacity);

/*
 * @brief
 * Adds a brush stroke pointer to the list
 *
 * @param list    The brush stroke list to add to
 * @param stroke  The brush stroke to add
 *
 * Preconditions:
 * - `list != NULL`
 * - `stroke != NULL`
 * - List has valid internal structure
 *
 * Invariants:
 * - Only pointers are stored (no deep copying)
 * - Capacity expansion follows doubling strategy
 * - Existing pointers preserved during reallocation
 *
 * Postconditions:
 * - Stroke pointer added to list
 * - Count incremented on success
 * - Capacity may increase if needed
 */
void brush_stroke_list_add(brush_stroke_list_t *list, brush_stroke_t *stroke);

/*
 * @brief
 * Frees brush stroke list (not individual strokes)
 *
 * @param list   The brush stroke list to free
 *
 * Preconditions:
 * - `list` may be NULL
 *
 * Invariants:
 * - Only list structure is freed
 * - Individual brush strokes remain valid
 * - No memory leaks in list management
 *
 * Postconditions:
 * - List structure memory freed
 * - Individual brush strokes unchanged
 * - Caller retains responsibility for stroke memory
 */
void brush_stroke_list_free(brush_stroke_list_t *list);

/* Brush stroke memory management */

/*
 * @brief
 * Frees memory allocated for a brush stroke
 *
 * @param stroke   The brush stroke to free
 *
 * Preconditions:
 * - `stroke` may be NULL
 *
 * Invariants:
 * - All components of stroke are freed
 * - Bézier curve array is properly deallocated
 * - Function handles NULL gracefully
 *
 * Postconditions:
 * - All memory associated with stroke is freed
 * - Bézier curves array and stroke structure deallocated
 * - Safe to call with NULL pointer
 */
void brush_stroke_free(brush_stroke_t *stroke);

/* Image output functions */
typedef struct {
    uint8_t *data;
    int width;
    int height;
} image_t;

/*
 * @brief
 * Creates a new image with specified dimensions
 *
 * @param width    Width of the image in pixels
 * @param height   Height of the image in pixels
 * @return        Pointer to new image or NULL on failure
 *
 * Preconditions:
 * - `width > 0`
 * - `height > 0`
 * - Sufficient memory for image data (width × height × 4 bytes)
 *
 * Invariants:
 * - Image data uses RGBA format (4 bytes per pixel)
 * - Memory allocation is contiguous
 * - Image initialized to transparent black
 *
 * Postconditions:
 * - Returns valid `image_t*` or NULL on failure
 * - Image data allocated and initialized to zeros
 * - Image ready for pixel operations
 */
image_t *create_image(int width, int height);

/*
 * @brief
 * Sets a pixel color at specified coordinates
 *
 * @param img    The image to modify
 * @param x      X coordinate of the pixel
 * @param y      Y coordinate of the pixel
 * @param color  Color to set the pixel to
 *
 * Preconditions:
 * - `img != NULL`
 * - `img->data != NULL`
 * - `0 ≤ x < img->width`
 * - `0 ≤ y < img->height`
 *
 * Invariants:
 * - Pixel data stored in RGBA format
 * - Array bounds checking prevents buffer overruns
 * - Color components stored in correct order
 *
 * Postconditions:
 * - Pixel at (x,y) set to specified color
 * - Out-of-bounds requests are ignored safely
 * - Image data integrity maintained
 */
void set_pixel(image_t *img, int x, int y, color_t color);

/*
 * @brief
 * Saves image data to BMP file format
 *
 * @param img       The image to save
 * @param filename  Name of the file to save to
 *
 * Preconditions:
 * - `img != NULL`
 * - `img->data != NULL`
 * - `filename != NULL`
 * - Valid file system path
 * - Write permissions for target location
 *
 * Invariants:
 * - BMP format specifications followed
 * - File I/O handled with proper error checking
 * - Image data converted to BMP RGB format
 * - Endianness handled correctly
 *
 * Postconditions:
 * - BMP file created with image data
 * - File contains valid BMP header and pixel data
 * - Function handles errors gracefully
 * - Original image data unchanged
 */
void save_bmp(image_t *img, const char *filename);

void save_ppm(image_t *img, const char *filename);

/*
 * @brief
 * Renders a list of spans to an image
 *
 * @param spans  The list of spans to render
 * @param img    The image to render to
 *
 * Preconditions:
 * - `spans != NULL`
 * - `img != NULL`
 * - `img->data != NULL`
 * - All spans contain valid coordinates and colors
 *
 * Invariants:
 * - Spans are processed in order
 * - Only pixels within image bounds are modified
 * - Color blending or overwriting follows consistent rules
 * - Span coordinates are properly clipped
 *
 * Postconditions:
 * - Image contains rendered spans
 * - Pixels within span ranges set to span colors
 * - Out-of-bounds spans handled safely
 * - Image ready for output or further processing
 */
void render_spans_to_image(span_list_t *spans, image_t *img);

/*
 * @brief
 * Frees memory allocated for an image
 *
 * @param img   The image to free
 *
 * Preconditions:
 * - `img` may be NULL
 *
 * Invariants:
 * - Both image structure and data are freed
 * - Function handles partial initialization
 * - No memory leaks occur
 *
 * Postconditions:
 * - All image memory deallocated
 * - Function safe to call with NULL
 * - Image pointer becomes invalid after call
 */
void free_image(image_t *img);
#endif

#endif