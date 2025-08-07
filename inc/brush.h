/*
@file brush.h
@authors Prof. Dr.-Ing. Volker Roth, Matthias Grabner (show_segments), Toni
Draßdo
@date 01.07.25
*/

#ifndef BRUSH_H
#define BRUSH_H

#include "common.h"

/* Non-user brush related functions */

/*
 * @brief
 *  Converts trace buffer to cubic Bézier segments in stroke structure.
 *
 * @param
 *  stroke     Brush stroke structure
 *             with allocated `beziers` and `length`.
 *
 * @pre
 *  trace_top is multiple of 4;
 *  stroke->beziers is either NULL or allocated.
 *
 * @post
 *  Appends new cubic segments to stroke->beziers;
 *  updates stroke->length.
 *  Allocates if `beziers == NULL`.
 */
void show_segments(brush_stroke_t *stroke);

/*
 * @brief
 *  Splits all curve segments at tee parameters
 *  (inflections, pen slopes).
 *
 * @param
 *  knots       Cyclic list of knots forming a curve.
 *  brush       Cyclic list of knots forming the pen shape.
 *
 * @pre
 *  knots and brush are valid, cyclic, and have proper control points.
 *
 * @post
 *  Curve is split into segments at calculated tee values.
 */
void split_at_tees(knot_t *knots, knot_t *pen);

/*
 * @brief
 *  Applies the brush convolution to all segments of the curve.
 *
 * @param
 *  knots      Cyclic knot list representing the path.
 *  brush      Cyclic knot list representing the brush shape.
 *
 * @pre
 *  knots and brush are valid, cyclic, and contain control points.
 *
 * @post
 *  All valid convolutions are written to trace buffer.
 */
void convolve_all(knot_t *knots, knot_t *pen);

#endif
