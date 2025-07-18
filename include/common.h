/*
@file common.h
@authors Matthias, Toni
@date 01.07.25
*/

#ifndef COMMON_H
#define COMMON_H

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "knots.h"

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

/* One brush stroke consists of an array of cubic Bézier curves,
 * the length of the array and a color
 */
typedef struct {
    return_cubic_t *beziers;
    color_t color;
    size_t length; // length of the array
} brush_stroke_t;

/*
 *@brief
 * Initialize knots and move brush to specified point
 *
 *@param
 * point       point at which to move
 *
 *@return
 * knot_t *    pointer to created knot_t
 *
 *Preconditions
 * knots_get() must return valid memory.
 *   i.e. knots_get() must not return NULL.
 * point must contain valid coordinates.
 *   point.x and point.y should contain meaningful
 *   (e.g. no NaN (NotaNumber)) values.
 * The return value of knots_get() must be a valid,
 * unused knot_t pointer.
 *
 *Invaraints
 * The knot always refers to itself in its embedded list
 * knot structure (ln):
 *    knot->ln.pred == &knot->ln
 *    knot->ln.succ == &knot->ln
 * The knot_t structure is only changed via its own fields -
 * no influence on other knot_t objects.
 * There is no memory release or relocation -
 * the knot remains stable in memory.
 *
 *Postconditions
 * The function returns a non-NULL pointer to a valid knot_t.
 * The coordinates of the knot are passed as in the argument:
 *    knot->x == point.x
 *    knot->y == point.y
 * The embedded double list points to itself:
 *    knot->ln.pred == &knot->ln
 *    knot->ln.succ == &knot->ln
 * The knot types are set correctly:
 *    knot->lt == kt_open
 *    knot->rt == kt_endpoint
 * The generated knot is ready for further path processing.
 */
knot_t *moveto(point_t point);

/*
 *@brief
 * Initialize knots and move brush relative to specified point
 * The function uses old->ln.pred to access the “last” knot r
 * from which the move takes place.
 *
 *@param
 * old         Pointer to an existing knot in a linked list
 * p           shft relative to previous point
 *
 *@return
 * knot_t *    A pointer to a new knot_t that was created relative
 *             to the previous one.
 *
 *Preconditions
 * old != NULL
 *    - The pointer old must point to a valid knot_t.
 * old->ln.pred must point to a valid knot_t element
 *    - I.e. old must be part of a correctly constructed linked list
 *      (e.g. circular or linear).
 * The knots_get() function must return a valid knot (not NULL).
 * The values x and y must be valid floating point numbers
 * (e.g. no NaN (NotaNumber) or infinity).
 *
 *Invaraints
 * The original state of old and r is not changed.
 *    - The function reads r = old->ln.pred, but does not change
 *      anything in old or r.
 * The list to which old belongs remains structurally unchanged.
 *    - No knotss are added or removed.
 * knot->ln.pred and knot->ln.succ point to &knot->ln,
 * i.e. the knot is linked on its own (cyclic one-element list).
 *
 *Postconditions
 * The function returns a non-NULL pointer to a new knot_t object.
 * The coordinates of the new knot are calculated relative
 * to r = old->ln.pred:
 *    knot->x == r->x + x
 *    knot->y == r->y + y
 * The new knot is not included in an external list:
 *    knot->ln.pred == &knot->ln
 *    knot->ln.succ == &knot->ln
 * The knot types are set correctly:
 *    knot->lt == kt_open
 *    knot->rt == kt_endpoint
 * The old state (old, r, list) has not been changed.
 */
knot_t *rmoveto(knot_t *old, point_t p);

/*
 *@brief
 * Append a line made of one additional point to a collection of knots
 *
 *@param
 * p           pointer to an existing knot that is already part of
 *             a linked list.
 *             It marks the end of the previous path.
 *
 * point       Structure with target coordinates (x, y)
 *             to which a new line is drawn.
 *
 *@return
 * knot_t *    The function returns the same pointer p that was
 *             passed as an argument.
 *             It does not return the new knot,
 *             but continues to point to the current “last” knot.
 *
 *Preconditions
 * p is a valid, already included knot_t *.
 *    - p must be part of a double linked list.
 *    - p->ln.pred must point to a valid previous knot.
 * knots_get() returns a valid, non-NULL knots.
 *    - Otherwise there is a risk of a crash during
 *      the initialization of q.
 * point contains valid coordinate values (x, y).
 * The list in which p is located must not be empty.
 *    - Otherwise p->ln.pred is undefined.
 *
 *Invaraints
 * The double linked list remains consistently structured.
 *    - New knots are inserted correctly with list_insert_before(...).
 *    - No broken links are created.
 * p is not changed during the function, except by inserting
 * a new predecessor.
 * All knot pointers (p, r, q) point to valid knot_t structures
 * during the function.
 * The list remains circular or consistently linked
 * if it was previously correct.
 *
 *Postconditions
 * A new knot q was inserted before p in the list.
 * Coordinates of q:
 *    q->x == point.x
 *    q->y == point.y
 * Bézier control points were set explicitly:
 * On the right edge of r = p->ln.pred:
 *    r->rt == kt_explicit
 *    r->ri.e == 30% along the distance [r, q]
 * At the left edge of q:
 *    q->lt == kt_explicit
 *    q->li.e == 70% along the distance [r, q]
 * The return value is p, unchanged.
 * The list now contains an additional knot (q),
 * correctly integrated before p.
 */
knot_t *lineto(knot_t *knots, point_t point);

/*
 *@brief
 *Append a line, with an endpoint specified relative to the
 *last endpoint in the path
 *
 *@param
 * t           Pointer to a knot at the end of a chained knot
 *             sequence (path).
 * p           Relative X and y-displacement from the previous knot
 *             (not knots, but knots->ln.pred)
 *
 *@return
 * knot_t *    knots remains unchanged, but is supplemented
 *             by a new preceding knot q,
 *             which is inserted into the list.
 *
 *Preconditions
 * t != NULL
 *    - The transferred knot p must be valid (not a null pointer).
 * t->ln.pred points to a valid knot r
 *    - p must be part of a correctly linked knot path
 *      (e.g. a cyclic or linear).
 * The knots_get() function returns a valid knot q.
 *    - This means that memory is available and knot management
 *      has been initialized.
 * x and y are well-defined floating point numbers.
 *    - e.g. no NaN, no infinite values.
 *
 *Invaraints
 * The existing knot list remains correctly linked.
 *    - Exactly one new knot q is inserted correctly.
 *    - No list knot (p, r) are unintentionally changed or removed.
 * The link in t remains valid.
 *    - t continues to point correctly to successors and predecessors.
 * t, r and q point to valid knot_t structures during
 * the entire execution
 *
 *Postconditions
 * A new knot q has been added to the list - directly in front of t.
 * The coordinates of q are set relative to r:
 *    q->x == r->x + x
 *    q->y == r->y + y
 * The Bézier handles between r and q have been set explicitly:
 *    r gets a right handle:
 *       r->rt == kt_explicit
 *       r->ri.e.x == r->x + 0.3 * (q->x - r->x)
 *       r->ri.e.y == r->y + 0.3 * (q->y - r->y)
 *    q gets a left handle:
 *       q->lt == kt_explicit
 *       q->li.e.x == r->x + 0.7 * (q->x - r->x)
 *       q->li.e.y == r->y + 0.7 * (q->y - r->y)
 * The return value is exactly the input value t.
 * The knot path (r -> q -> t) is consistent and completely linked.
 */
knot_t *rlineto(knot_t *knots, point_t p);

/*
 *@brief
 * Append a cubic Bézier made of four points to a collection of knots.
 * If the start point of the cubic curve does not match
 * the coordinates of the last knot,
 * an empty knot list will be returned.
 *
 *@param
 * p           Pointer to the current end knot of an existing path.
 *
 * curve       Structure that describes a cubic Bézier curve
 *             (with two control points and one endpoint).
 *
 *@return
 * knot_t * p  The function returns the same pointer p that it
 *             received as an argument.
 *             p is not changed, but only extended
 *             by a new knot q in front of it.
 *
 *Preconditions
 * p != NULL
 *    - The passed pointer p must point to a valid knot_t.
 * p->ln.pred != NULL
 *    - The knot p must be part of a correctly linked double list;
 *      r = p->ln.pred must be valid.
 * knots_get() must return a valid, initialized knot.
 * The curve structure contains well-defined values:
 *    - curve.c1, curve.c2, curve.end are valid point_t values
 *      (no NaNs, Infs).
 * curve.end.y
 *    - accesses the y-value of the end point of the Bézier curve.
 *      This value is used to set the y-coordinate of the new knot q:
 *
 *Invaraints
 * The knot path remains consistent:
 *    - The double-linked list remains intact
 *    - a new knot is inserted correctly.
 * p remains unchanged:
 *    - The function does not change p directly,
 *      except that it adds a predecessor (q) to it.
 * The memory structure of all knot remains valid:
 *    - No free, no invalid access, all pointers remain valid.
 * The knot types (kt_explicit, kt_endpoint) are set explicitly.
 *
 *Postconditions
 * A new knot q was inserted directly before p.
 * The coordinates of q match curve.end:
 *    q->x == curve.end.x
 *    q->y == curve.end.y
 * The control points (Bézier handles) were set explicitly:
 * For the previous knot r = p->ln.pred:
 *    r->rt == kt_explicit
 *    r->ri.e == curve.c1
 * For the new knot q:
 *    q->lt == kt_explicit
 *    q->li.e == curve.c2
 * The return value is the same pointer p.
 * The knot list now contains r → q → p with
 * correctly set Bézier data.
 */
knot_t *curveto(knot_t *knots, point_t control1, point_t control2,
                point_t end);

/*
 *@brief
 * Closes the current path (if not already closed) with a straight
 * line segment.
 *
 *@param
 * p           Pointer to the start knot of a path segment
 *             that is to be closed cyclically.
 *
 *@return
 * knot_t *    The unchanged pointer p is returned.
 *             If p == NULL, NULL is returned.
 *
 *Preconditions
 * p == NULL is permitted, then NULL is simply returned.
 * If p != NULL, p->ln.pred must be a valid, concatenated knot
 *    - p must be part of a correctly constructed double
 *      concatenated list.
 *    - p->ln.pred must not be invalid or free memory.
 * If the path is already cyclic (p->ln.pred == p),
 * no additional connection is created.
 *
 *Invaraints
 * The knot list remains consistent.
 *    - No knots are created or deleted.
 *    - Only handles (ri.e, li.e) and types (rt, lt) are changed.
 * p and r = p->ln.pred remain valid and correctly concatenated.
 * The position data (x, y) of the knots remain unchanged.
 *    - Only the Bézier handle fields are changed.
 * If r == p, nothing is changed at all.
 *    - This means that the function is also neutral
 *      for paths with only one knot or closed paths.
 *
 *Postconditions
 * If p == NULL:
 *    Return value is NULL. No changes are made.
 * If r != p (path has several knots and is still open):
 *    A straight line transition with explicit Bézier handles
 *    is defined between the last knot r and
 * the start knot p. r->rt == kt_explicit and
 * p->lt == kt_explicit The handles lie on the line from r
 * to p: r->ri.e = 30 % of the distance from
 * r to p p->li.e = 70 % of the distance from r to p The
 * return value is always p (except when p == NULL).
 */
knot_t *pathclose(knot_t *knots);

/*
 *@brief
 * Verifies that knots can be used as brush
 *
 *@param
 * knots *     Pointer to the start knot of a circularly
 *             linked path (a closed path)
 *             that is to be used as a brush contour.
 *             The knot path is a closed loop:
 *             SUCC and PRED move back and forth in a
 *             circular fashion.
 *             Each knot_t contains position data (x, y)
 *             and Bézier handle fields (li, ri).
 *
 *@return
 * int         1 Successful: The knot path is valid
 *                and has been processed.
 *            -1 Error: Two consecutive knots are at the same
 *                      point (duplicate point).
 *            -2 Error: A segment makes a non-left-handed
 *                      (straight or right-handed) curve
 *                      - brush must be convex and counterclockwise.
 *            -3 Error: More than one full rotation (alpha > 2Pi) -
 *                      i.e. too many rotations, not permitted.
 *
 *Preconditions
 * knots is not NULL.
 *     There must be a valid, circularly linked knot ring.
 * The knot path contains at least 3 different points.
 *     Otherwise a convex closed form is not possible.
 * SUCC(p) and PRED(p) must be defined and correctly
 * concatenated for all p.
 *     The function assumes that p->ln.succ and p->ln.pred are set
 *     correctly.
 * All knotss have valid coordinates (x, y).
 * The knot_t data structure contains fields for
 * Bézier control points,
 *     e.g. ri.e, li.e, to set handle points.
 *
 *Invaraints
 * p runs over each knotof the ring list exactly once.
 *     do { ... } while (p != knotss); guarantees complete iteration
 *     without duplication.
 * The knot path remains continuously linked and unchanged.
 *     No knots are added or removed.
 * The angle theta between two consecutive segments is calculated
 * correctly for each transition.
 *     Angles are reduced (via reduce_angle) and
 *     positive (> 0) if correct.
 * The current Bézier handles are set:
 *     For each segment p -> q:
 *         p->ri.e = p + ⅓ * (q - p)
 *         q->li.e = q - ⅓ * (q - p)
 * Angle sum alpha increases with each iteration.
 *
 *Postconditions
 * All knots have correctly initialized Bézier control points:
 *     Explicit handles on 1/3 or 2/3 of the path to
 *     the neighboring knot.
 * The entire figure is oriented counterclockwise.
 *     Each local angle is > 0 (no right or negative angle).
 * The entire rotation alpha is fulfilled:
 *     0 < alpha ≤ 2Pi (max. one rotation).
 * The return value is 1 -> success.
 *
 */
int brush_make(knot_t *knots);

/*
 *@brief
 * Creates square-shaped brush from knots and verifies it
 *
 *@param
 * color       color value that is assigned to the created brush.
 *
 *@return
 * brush_t *   brush_make_square() returns a pointer to a
 *             newly created brush object (of type *brush_t).
 *             The shape of this brush is a closed square path.
 *             The brush is therefore a square with
 *             center (0,0) and color.
 *
 *Preconditions
 * The passed parameter color of type color_t must be valid
 * and well-defined.
 *    - e.g. no undefined colour values,
 *      no uninitialized memory areas.
 * Memory management (e.g. knots_get()) must be initialized.
 * point_t constructs such as (point_t){{-0.5, 0.5}}
 * must be permitted in the C dialect used *(possibly GNU C).
 *
 *Invaraints
 *The brush consists of a consistently concatenated list of knots
 (knot_t) at all times.
 *    - After each lineto etc.,
 *      the knot path is correctly concatenated and
 *      consistent in itself.
 * The geometric structure grows step by step,
 * but always remains a valid open or closed path.
 * The coordinates are in the range [-0.5, 0.5]
 *    - The square is centered around the origin and symmetrical.
 * The color information color is passed on unchanged
 * until brush_make is called
 *
 *Postconditions
 * A valid brush_t * was returned.
 * The brush represents a closed square with the
 * following points (in this order):
 *    (0.5, 0.5) → starting point
 *    (-0.5, 0.5)
 *    (-0.5, -0.5)
 *    (0.5, -0.5)
 *    -> automatically return to (0.5, 0.5) by pathclose
 * The brush contains correct line connections
 * (no Bézier, but lineto).
 * The path structure was passed to brush_make(...),
 * which internally presumably
 *     converts the knots,
 *     and assigns the color color.
 * The return value is a fully usable brush object.
 */
knot_t *brush_make_square();

/*
 *@brief
 * Should support all operations that can be done with matrix,
 * such as resize, rotate, translate, slant, etc.
 * At least translation requires a 3x3 matrix,
 * even for 2D coordinates, thus we use a 3x3 matrix.
 * Is chainable with other transforms.
 *
 *@param
 * brush_t            the brush that is to be transformed
 *                    (consits of knots)
 * transform [3][3]   Transformation matrix (rotation, scaling, etc.)
 *
 *@return
 * void               no return value -
 *                    the transformation is applied directly to brush
 *
 *Preconditions
 * brush ≠ NULL
 *  -> A valid pointer to a brush_t structure must be passed.
 * brush->knots ≠ NULL
 *  -> The brush must have at least one valid start knot.
 * The knot path is correctly cyclically concatenated
 *  -> The list of knot_t elements must form a valid
 *     cyclic double-list compound.
 * transform is a valid 3×3 matrix
 *  -> All values of the matrix are defined
 *     (not NaN or undefined) and contain a
 *     valid affine *transformation.
 *
 *Invaraints
 * The knot path remains cyclical (cycle == 1)
 *  -> The double list of knot_t elements remains intact
 *     (no loss of connections).
 * Number of knots remains the same
 *  -> No new knots are created or deleted.
 * Only coordinates are changed
 *  -> Only k->x, k->y and, if applicable,
 *     li.e and ri.e of neighboring knots are changed - no other
 *knot data structure is damaged. The order of the knots is retained
 *  -> The order (starting point, line direction) remains unchanged
 *
 *Postconditions
 * Each knot was transformed
 *  -> The following applies for each knot k:
 *  [k->x, k->y] was multiplied by transform[][]
 * Explicit control points were also transformed
 *  -> For all explicit control points (kt_explicit) of the
 *     neighbors, the values were set to the
 * new point (identical to k->x, k->y).
 * The brush represents the same topology
 *  -> The shape is geometrically transformed,
 *     but it remains a closed (or open) path with the same
 * segment structure. No memory loss / no invalid pointers
 * -> No memory leaks were generated and all pointers within
 *    the knot structure are still valid.
 */
void brush_tf(knot_t *brush, double matrix[3][3]);

/*
 *@brief
 * Scales the brush by independent
 * factors along x and y. Calls brush_transform,
 * but makes it easier for user to interface with matrices.
 * Both are exposed though.
 * Is chainable with other transforms.
 *
 *@param
 * brush       brsuh to transform
 * scale       scalefactor
 * axis        Which axis(es) are going to be scaled: X, Y or Z
 *
 *@return
 * void        no return value -
 *             the transformation resize is applied directly to brush
 *
 *Preconditions
 * brush ≠ NULL
 *  -> A valid pointer to a brush_t object must be passed.
 * brush->knots ≠ NULL
 *  -> The brush must contain at least
 *     one valid knot_t start point (not empty).
 * scale > 0
 *  -> The scaling has to be other than zero or negative,
 *     otherwise it makes no geometric sense.
 * axis ∈ {X, Y, Z}
 *  -> The value of the axis parameter must be one of the
 *     valid enum values. (Z is for both axes,
 * analogous to 2D “Zoom”). The knot list is circular and valid
 *  -> The ln.pred/ln.succ pointers of the knot_t structures
 *     must form an intact, circular, double-linked list.
 *
 *Invaraints
 * Number of knots remains the same
 *  -> No knots are added or deleted.
 * Knots order remains the same
 *  -> The order of the knots remains the same as
 *     before the transformation.
 * The structure of the brush remains consistent
 *  -> brush->knots remains pointing to the
 *     original first knot (even after transformation).
 * The transformation is linear-affine
 *  -> The scaling is performed correctly along the
 *     desired axes using a 3×3 matrix.
 * The brush remains cyclic (cycle == 1)
 *  -> The knots list remains circularly linked.
 *
 *Postconditions
 * Brush was scaled by scalevalue
 *  -> In the specified axis (X, Y or Z),
 *     all knots were moved by the specified factor from the
 * center point. Geometric center is retained
 *  -> The brush was first moved to the origin, then scaled,
 *     and then moved back to the original
 *     center. Coordinates of all knots are changed (if scale ≠ 1)
 *  -> All k->x, k->y values were changed accordingly
 *     by the affine transformation.
 * Explicit control points may also have been scaled correctly
 *  -> Since brush_transform() also transforms
 *     control points internally, ri.e and li.e are also
 *     affected. The topology of the brush is retained
 *  -> The number and connection of the segments remains the same -
 *     it is the same figure, only scaled.
 */
void brush_tf_resize(knot_t *brush, double scale, enum axis_t axis);

/*
 *@brief
 * Flip the brush on the specified axis. Calls brush_transform,
 * but makes it easier for
 * user to interface with matrices. Both are exposed though.
 * Is chainable with other transforms.
 *
 *@param
 * brush       pointer to brush which is going to be refelcted
 * axis        axis of refelction (X, Y, Z)
 *
 *@return
 * void        refelcdtion is applied directly to brush
 *
 *Preconditions
 * brush != NULL
 *  -> A valid pointer to a brush structure must be transferred.
 * brush->knots != NULL
 *  -> The brush must contain at least one knot.
 * axis is X, Y
 *  -> The axis type must be meaningfully defined
 *     (other values do not lead to an effective (transformation).
 *     The knot list of the brush is correctly circularly linked
 *  -> ln.pred and ln.succ of each knot form a consistent ring list.
 *
 *Invaraints
 * The number of knots remains the same
 *  -> No knots are added or removed.
 * The concatenation of the knots is retained
 *  -> The order and linking of the knots does not change.
 * The structure of the brush remains valid
 *  -> No pointers are damaged or uninitialized
 *     data structures are used.
 *
 *Postconditions
 * All knots of the brush were mirrored along the selected axis
 *    If axis == X: Y coordinates of all points are negated
 *    -> mirroring on X axis.
 *    If axis == Y: X coordinates are negated → mirrored on Y axis.
 * Explicit tangents were also mirrored
 *    Control points (if available,
 *    lt == kt_explicit or rt == kt_explicit) are handled correctly by
 *    brush_transform().
 *    The topology and structure of the brush is retained
 *    -> The sequence, concatenation and knot properties except
 *    the coordinates remain the same.
 */
void brush_tf_rotate(knot_t *brush, double theta, enum axis_t axis);

/*
 *@brief
 * Flip the brush on the specified axis. Calls brush_transform,
 * but makes it easier for
 * user to interface with matrices. Both are exposed though.
 * Is chainable with other transforms.
 *
 *@param
 * brush       pointer to brush which is going to be refelcted
 * axis        axis of refelction (X, Y, Z)
 *
 *@return
 * void        refelcdtion is applied directly to brush
 *
 *Preconditions
 * brush != NULL
 *  -> A valid pointer to a brush structure must be transferred.
 * brush->knots != NULL
 *  -> The brush must contain at least one knot.
 * axis is X, Y
 *  -> The axis type must be meaningfully defined
 *     (other values do not lead to an effective transformation).
 *     The knot list of the brush is correctly circularly linked
 *  -> ln.pred and ln.succ of each knot form a consistent ring list.
 *
 *Invaraints
 * The number of knots remains the same
 *  -> No knots are added or removed.
 * The concatenation of the knots is retained
 *  -> The order and linking of the knots does not change.
 * The structure of the brush remains valid
 *  -> No pointers are damaged or uninitialized data structures
 *     are used.
 *
 *Postconditions
 * All knots of the brush were mirrored along the selected axis
 *    If axis == X: Y coordinates of all points are negated
 *    -> mirroring on X axis.
 *       If axis == Y: X coordinates are negated → mirrored on Y axis.
 * Explicit tangents were also mirrored
 *    Control points (if available,
 *    lt == kt_explicit or rt == kt_explicit) are handled correctly by
 *    brush_transform().
 *    The topology and structure of the brush is retained
 *    -> The sequence, concatenation and knot properties except
 *       the coordinates remain the same.
 */
void brush_tf_reflect(knot_t *brush, enum axis_t axis);

/*
 *@brief
 * Translate the brush midpoint to the coordinate specified.
 * Calls brush_transform, but
 * makes it easier for user to interface with matrices.
 * Both are exposed though.
 * -> Is chainable with other transforms.
 *
 *@param
 * knot * first  Pointer to the first knot of a circularly linked path
 *               (i.e. a closed brush).
 *
 * point_t dest  Target point (x, y) to which the center of the brush
 *               is to be moved.
 *
 *@return
 * void        The function does not return anything.
 *             It changes the brush in-place by moving it
 *             in the plane (translation).
 *
 *Preconditions
 * first not NULL
 *  - It must be a valid pointer to a knot_t knot.
 * first belongs to a circularly concatenated knot ring
 *  - This means that the concatenation
 *    via first->ln.succ must return to first at some point,
 *    without an infinite loop or segfault.
 * All knots in the ring have valid x and y values.
 *  - No NaN, inf or undefined memory areas.
 * The brush_tf(...) function is implemented correctly.
 *  - It expects a 3×3 matrix (homogeneous coordinates)
 *    and modifies all knot accordingly.
 *
 *Invaraints
 * All processed knots belong to the same ring.
 *  - The loop do { ... } while (k != first)
 *    no knot is processed twice.
 * The min/max values (xmin, xmax, ymin, ymax)
 * define a bounding box rectangle over all knots.
 * The variable center describes the exact geometric
 * center of the bounding box.
 *  - (xmin + xmax)/2, (ymin + ymax)/2
 * The calculation dest - center results in the correct translation
 * vector to move the center to dest.
 *
 *Postconditions
 * All knotss in the path have been moved by the same vector
 * (trans_x, trans_y).
 * The geometric center of the bounding box of the brush
 * is now at dest.
 * The shape and structure of the path remain unchanged.
 *  - Only a translation was performed, no scaling,
 *    rotation or topology change.
 * The circular concatenation of the knots remains intact.
 *  - ln.succ pointers have not been changed.
 * The transformation is purely geometric (affine),
 * without side effects on other data structures.
 */
void brush_tf_translate(knot_t *brush, point_t dest);

/*
 *@brief
 * Shears the brush by the specified factor on the specified axis.
 * Calls brush_transform, but makes it easier for user to interface
 * with matrices. Both are exposed though. Is chainable with
 * other transforms.
 *
 *@param
 * brush       pointer to brush which is going to be refelcted
 * shear       factor, how much the points are shifted along
 *             an axis depending on the other coordinate.
 * axis        axis of refelction (X, Y, Z)
 *
 *@return
 * void        shear is applied directly to brush
 *
 *Preconditions
 * brush != NULL
 *  -> A valid pointer to a brush structure must be transferred.
 * brush->knots != NULL
 *  -> The brush must contain at least one valid knot.
 * axis is X, Y
 *  -> Only X or Y shear direction is permitted. .
 * The knot list of the brush is consistently circularly linked
 *  -> ln.pred and ln.succ of each knot point correctly
 *     to the neighbors.
 *
 *Invaraints
 * The number of knots remains the same
 *  -> No knots are created or deleted.
 * The concatenation of the knots remains the same
 *  -> ln.pred and ln.succ of all knotss remain correctly linked.
 * Non-coordinate-related knot properties remain the same
 *  -> Type information (lt, rt) or pointers to other data
 *     remain unchanged.
 *
 *Postconditions
 * All knots were transformed shear geometrically
 *     For X: x' = x + shear * y
 *     For Y: y' = y + shear * x
 * Explicit tangents were adjusted correctly
 *     If a neighboring knot had lt == kt_explicit
 *     or rt == kt_explicit, the tangent points (li.e, ri.e)
 *     were also transformed. The shape of the brush is distorted,
 *     but structurally unchanged
 *  -> The brush is still valid and fully linked.
 *
 */
void brush_tf_shear(knot_t *brush, double shear, enum axis_t axis);

/*
 *@brief
 * Moves the brush to the first knot of the path and draws the shape.
 *
 *@param
 * path        A path of linked knot_t knot, usually
 *             organized circularly.
 *             Represents the shape that is to be drawn.
 *
 * brush       A brush object that contains the shape of the stroke
 *             (e.g. square, line) and a color.
 *
 *@return
 * brush_stroke_t    The return value is the finished stroke.
 *                   It is allocated using malloc in the function,
 *                   and must be freed by the user!
 *
 *Preconditions
 * path != NULL
 *    - path must point to a valid, circularly linked knot list
 *      (knot_t ring), i.e. typically a
 *closed path object. brush != NULL
 *    - brush must be a valid pointer to an
 *      initialized brush_t structure, including defined color and
 *      shape (e.g. square).
 *      The contained functions must be
 *      defined correctly:
 *      brush_tf_translate
 *      split_at_tees
 *      convolve_all
 *      show_segments
 *      knots_ring_clone,
 *      knots_ring_reverse,
 *      knots_ring_free
 * The brush transformation (brush_tf_translate) and
 * convolution (convolve_all) must not cause any
 * memory errors or side effects.
 * The x and y fields in the path start knot must be valid coordinate
 * values.
 *
 *Invaraints
 * The memory areas for stroke and all internal structures
 * (before releasing) remain valid.
 * The brush remains consistently transformed during convolution -
 * translation is relative to the starting point.
 * The path structure (path) remains valid until it is released.
 * The color information is retained throughout the entire execution
 * and is transferred unchanged in stroke->color.
 * The forward and return path logic is symmetrical:
 * The original path is processed first,
 * then a cloned and reversed path.
 * A memory leak is never generated, as everything that is
 * duplicated is also released.
 *
 *Postconditions
 * The brush stroke contains new drawn segments
 * (at least from the forward and backward paths).
 * stroke->color corresponds to the brush color value brush->color.
 * stroke->length has been recalculated and is ≥ 0.
 * The following temporary data structures used have been released
 * correctly: path, return_path
 * The return value is the same pointer as the input parameter stroke,
 * but with updated content.
 */
brush_stroke_t *draw_shape(knot_t *path, knot_t *brush, color_t color);

#endif