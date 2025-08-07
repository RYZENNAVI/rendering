/*
 * @file rasterization.c
 * @authors Team 2 - Rasterization (Optimized Version)
 * @date 01.07.25
 *
 * Optimized implementation of rasterization functions for brush strokes
 * Cleaned up version with only essential functions
 */

#include "../inc/common.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

/* Internal helper functions */
static double bezier_x(return_cubic_t *bezier, double t);
static double bezier_y(return_cubic_t *bezier, double t);
static void add_span_if_valid(span_list_t *spans, int x1, int x2, int y,
                              color_t color);
static void rasterize_bezier(return_cubic_t *bezier, color_t color,
                             span_list_t *spans, double resolution);
static void merge_spans(span_list_t *spans);
static int compare_spans(const void *a, const void *b);
static brush_stroke_t *intersect_two_brush_strokes(brush_stroke_t *stroke1,
                                                   brush_stroke_t *stroke2);

/*
 * Helper function to evaluate Bézier curve x-coordinate at parameter t
 */
static double bezier_x(return_cubic_t *bezier, double t) {
    double u = 1.0 - t;
    return u * u * u * bezier->start.x + 3 * u * u * t * bezier->c1.x +
           3 * u * t * t * bezier->c2.x + t * t * t * bezier->end.x;
}

/*
 * Helper function to evaluate Bézier curve y-coordinate at parameter t
 */
static double bezier_y(return_cubic_t *bezier, double t) {
    double u = 1.0 - t;
    return u * u * u * bezier->start.y + 3 * u * u * t * bezier->c1.y +
           3 * u * t * t * bezier->c2.y + t * t * t * bezier->end.y;
}

/*
 * Helper function to add a span if it's valid (x1 <= x2)
 */
static void add_span_if_valid(span_list_t *spans, int x1, int x2, int y,
                              color_t color) {
    if (x1 <= x2) {
        span_t span = {x1, x2, y, color};
        span_list_add(spans, span);
    }
}

/*
 * Compare function for sorting spans by y, then by x_start
 */
static int compare_spans(const void *a, const void *b) {
    const span_t *span_a = (const span_t *) a;
    const span_t *span_b = (const span_t *) b;

    if (span_a->y != span_b->y) {
        return span_a->y - span_b->y;
    }
    return span_a->x_start - span_b->x_start;
}

/*
 * Merge overlapping spans on the same y coordinate
 */
static void merge_spans(span_list_t *spans) {
    if (spans->count <= 1) return;

    qsort(spans->spans, spans->count, sizeof(span_t), compare_spans);

    size_t write_idx = 0;
    for (size_t read_idx = 1; read_idx < spans->count; read_idx++) {
        span_t *current = &spans->spans[write_idx];
        span_t *next = &spans->spans[read_idx];

        // If spans are on same y and overlap or touch, merge them
        if (current->y == next->y && current->x_end >= next->x_start - 1) {
            current->x_end =
                (current->x_end > next->x_end) ? current->x_end : next->x_end;
        } else {
            write_idx++;
            spans->spans[write_idx] = *next;
        }
    }
    spans->count = write_idx + 1;
}

/*
 * Rasterize a single Bézier curve using optimized scanline conversion
 */
static void rasterize_bezier(return_cubic_t *bezier, color_t color,
                             span_list_t *spans, double resolution) {
    const int steps = 100; // Fixed tessellation steps for consistent results
    const double step_size = 1.0 / steps;

    // Sample points along the curve
    point_t prev_point = {bezier_x(bezier, 0) * resolution,
                          bezier_y(bezier, 0) * resolution};

    for (int i = 1; i <= steps; i++) {
        double t = i * step_size;
        point_t curr_point = {bezier_x(bezier, t) * resolution,
                              bezier_y(bezier, t) * resolution};

        // Rasterize line segment from prev_point to curr_point
        int x0 = (int) round(prev_point.x);
        int y0 = (int) round(prev_point.y);
        int x1 = (int) round(curr_point.x);
        int y1 = (int) round(curr_point.y);

        // Bresenham-like algorithm for rasterizing the line
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        int x = x0, y = y0;
        while (1) {
            add_span_if_valid(spans, x, x, y, color);

            if (x == x1 && y == y1) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }

        prev_point = curr_point;
    }
}

/*
 * Rasterizes a brush stroke into a list of spans
 */
span_list_t *rasterize(brush_stroke_t *stroke, double resolution) {
    if (!stroke || resolution <= 0) {
        return NULL;
    }

    span_list_t *spans = span_list_create(1000);
    if (!spans) {
        return NULL;
    }

    // Rasterize each Bézier curve in the stroke
    for (size_t i = 0; i < stroke->length; i++) {
        rasterize_bezier(&stroke->beziers[i], stroke->color, spans,
                         resolution);
    }

    // Merge overlapping spans
    merge_spans(spans);

    return spans;
}

/*
 * Unions multiple brush strokes into a single brush stroke
 */
brush_stroke_t *union_brush_stroke(brush_stroke_list_t *strokes) {
    if (!strokes || strokes->count == 0) {
        return NULL;
    }

    // Calculate total number of beziers needed
    size_t total_length = 0;
    for (size_t i = 0; i < strokes->count; i++) {
        total_length += strokes->strokes[i]->length;
    }

    // Create a new stroke that contains all beziers
    brush_stroke_t *result = malloc(sizeof(brush_stroke_t));
    if (!result) {
        return NULL;
    }

    // Allocate memory for beziers array
    result->beziers = malloc(total_length * sizeof(return_cubic_t));
    if (!result->beziers) {
        free(result);
        return NULL;
    }

    result->length = 0;
    result->color = strokes->strokes[0]->color; // Use color from first stroke

    // Combine all beziers from all strokes
    for (size_t i = 0; i < strokes->count; i++) {
        brush_stroke_t *stroke = strokes->strokes[i];
        for (size_t j = 0;
             j < stroke->length && result->length < total_length; j++) {
            result->beziers[result->length] = stroke->beziers[j];
            result->length++;
        }
    }

    return result;
}

/*
 * Helper function to check if two bounding boxes intersect
 */
static bool bounding_boxes_intersect(bounding_box_t box1,
                                     bounding_box_t box2) {
    return !(box1.max_x < box2.min_x || box2.max_x < box1.min_x ||
             box1.max_y < box2.min_y || box2.max_y < box1.min_y);
}

/*
 * Helper function to get bounding box of a single bezier curve
 */
static bounding_box_t bezier_bounding_box(return_cubic_t *bezier) {
    bounding_box_t bbox;

    // Initialize with start point
    bbox.min_x = bbox.max_x = bezier->start.x;
    bbox.min_y = bbox.max_y = bezier->start.y;

    // Check all control points
    point_t points[4] = {bezier->start, bezier->c1, bezier->c2, bezier->end};

    for (int i = 0; i < 4; i++) {
        if (points[i].x < bbox.min_x) bbox.min_x = points[i].x;
        if (points[i].x > bbox.max_x) bbox.max_x = points[i].x;
        if (points[i].y < bbox.min_y) bbox.min_y = points[i].y;
        if (points[i].y > bbox.max_y) bbox.max_y = points[i].y;
    }

    return bbox;
}

/*
 * Helper function to estimate if two bezier curves might intersect
 * This is a conservative estimate using bounding boxes
 */
static bool beziers_might_intersect(return_cubic_t *bezier1,
                                    return_cubic_t *bezier2) {
    bounding_box_t box1 = bezier_bounding_box(bezier1);
    bounding_box_t box2 = bezier_bounding_box(bezier2);

    return bounding_boxes_intersect(box1, box2);
}

/*
 * Intersect two brush strokes and return the intersection
 * This is a simplified implementation that keeps beziers from both strokes
 * that have overlapping bounding boxes
 */
static brush_stroke_t *intersect_two_brush_strokes(brush_stroke_t *stroke1,
                                                   brush_stroke_t *stroke2) {
    if (!stroke1 || !stroke2) {
        return NULL;
    }

    // Quick bounding box check
    bounding_box_t bbox1 = bounding_box(stroke1);
    bounding_box_t bbox2 = bounding_box(stroke2);

    if (!bounding_boxes_intersect(bbox1, bbox2)) {
        return NULL; // No intersection possible
    }

    // Create result stroke with estimated capacity
    brush_stroke_t *result = malloc(sizeof(brush_stroke_t));
    if (!result) return NULL;

    // Start with capacity for smaller stroke
    size_t initial_capacity = (stroke1->length < stroke2->length)
                                  ? stroke1->length
                                  : stroke2->length;
    result->beziers = malloc(initial_capacity * sizeof(return_cubic_t));
    if (!result->beziers) {
        free(result);
        return NULL;
    }

    result->length = 0;
    result->color = stroke1->color; // Use color from first stroke
    size_t capacity = initial_capacity;

    // Find beziers that might intersect
    for (size_t i = 0; i < stroke1->length; i++) {
        for (size_t j = 0; j < stroke2->length; j++) {
            if (beziers_might_intersect(&stroke1->beziers[i],
                                        &stroke2->beziers[j])) {
                // Expand capacity if needed
                if (result->length >= capacity) {
                    capacity *= 2;
                    return_cubic_t *new_beziers = realloc(
                        result->beziers, capacity * sizeof(return_cubic_t));
                    if (!new_beziers) {
                        free(result->beziers);
                        free(result);
                        return NULL;
                    }
                    result->beziers = new_beziers;
                }

                // Add the bezier from stroke1 (could be refined to actual
                // intersection)
                result->beziers[result->length] = stroke1->beziers[i];
                result->length++;
                break; // Only add each bezier once
            }
        }
    }

    // If no intersections found, return empty stroke instead of NULL
    if (result->length == 0) {
        // Keep the empty stroke structure
    }

    return result;
}

/*
 * Intersects multiple brush strokes into a single brush stroke
 * Uses pairwise intersection to avoid rasterization overhead
 */
brush_stroke_t *intersection_brush_stroke(brush_stroke_list_t *strokes) {
    if (!strokes || strokes->count == 0) {
        return NULL;
    }

    if (strokes->count == 1) {
        // Single stroke - return a copy
        brush_stroke_t *first = strokes->strokes[0];
        brush_stroke_t *result = malloc(sizeof(brush_stroke_t));
        if (!result) return NULL;

        result->beziers = malloc(first->length * sizeof(return_cubic_t));
        if (!result->beziers) {
            free(result);
            return NULL;
        }

        result->length = first->length;
        result->color = first->color;
        for (size_t i = 0; i < first->length; i++) {
            result->beziers[i] = first->beziers[i];
        }
        return result;
    }

    // Start with the first stroke
    brush_stroke_t *result = NULL;

    // Copy first stroke
    brush_stroke_t *first = strokes->strokes[0];
    result = malloc(sizeof(brush_stroke_t));
    if (!result) return NULL;

    result->beziers = malloc(first->length * sizeof(return_cubic_t));
    if (!result->beziers) {
        free(result);
        return NULL;
    }

    result->length = first->length;
    result->color = first->color;
    for (size_t i = 0; i < first->length; i++) {
        result->beziers[i] = first->beziers[i];
    }

    // Intersect with each subsequent stroke
    for (size_t i = 1; i < strokes->count; i++) {
        brush_stroke_t *temp =
            intersect_two_brush_strokes(result, strokes->strokes[i]);

        // Free the old result
        if (result) {
            free(result->beziers);
            free(result);
        }

        result = temp;

        // If intersection is empty, no need to continue
        if (!result || result->length == 0) {
            break;
        }
    }

    return result;
}

/*
 * Calculates the bounding box of a brush stroke
 */
bounding_box_t bounding_box(brush_stroke_t *stroke) {
    bounding_box_t bbox = {0, 0, 0, 0};

    if (!stroke || stroke->length == 0) {
        return bbox;
    }

    // Initialize with first point
    bbox.min_x = bbox.max_x = stroke->beziers[0].start.x;
    bbox.min_y = bbox.max_y = stroke->beziers[0].start.y;

    // Check all control points and end points
    for (size_t i = 0; i < stroke->length; i++) {
        return_cubic_t *bezier = &stroke->beziers[i];

        // Check all four points of the bezier curve
        point_t points[4] = {bezier->start, bezier->c1, bezier->c2,
                             bezier->end};

        for (int j = 0; j < 4; j++) {
            if (points[j].x < bbox.min_x) bbox.min_x = points[j].x;
            if (points[j].x > bbox.max_x) bbox.max_x = points[j].x;
            if (points[j].y < bbox.min_y) bbox.min_y = points[j].y;
            if (points[j].y > bbox.max_y) bbox.max_y = points[j].y;
        }
    }

    return bbox;
}
