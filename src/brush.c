/*
@file brush.c
@authors Prof. Dr.-Ing. Volker Roth, Matthias Grabner (show_segments)
         Toni Draßdo
@date 01.07.25
*/

#include "brush.h"
#include "common.h"

#define PEN_MAX 1024
#define TRACE_MAX 1024
#define SMALL 0.000000000001

double tee_stack[2 * PEN_MAX];
uint32_t tee_top;

point_t trace[TRACE_MAX];
uint32_t trace_top;

/* Rotate (u,v) right by the angle of (x,y). */
#define ROR_X(x, y, u, v) ((u) * (x) + (v) * (y))
#define ROR_Y(x, y, u, v) ((v) * (x) - (u) * (y))

/* Shortcuts to save typing. */
#define SUCC(p) ((knot_t *) (p)->list_node.succ)
#define PRED(p) ((knot_t *) (p)->list_node.pred)

/* PRE: knots is a cyclic path.
 * POST: The path is convex.  There are only
 * left turns in the edges of the path.
 */

/*
 * @brief
 *  Makes control points and verifies convexity and
 *  orientation of a brush path.
 *
 * @param
 * knots A     cyclic list of knots forming the pen shape.
 *
 * @pre
 *  knots != NULL, path is cyclic (SUCC and PRED form a cycle),
 *  has at least 3 distinct points.
 *
 * @invariant
 *  Each segment is a left turn;
 *  direction angles are accumulated positively.
 *
 * @post Control points (ri.e and li.e) are initialized; returns:
 *       -1: duplicate points,
 *       -2: non-left turn (not convex CCW),
 *       -3: winding number too high,
 *        1: success.
 */
int brush_make(knot_t *knots) {
    knot_t *p;
    knot_t *q;
    double theta;
    double alpha;
    double dx;
    double dy;
    double du;
    double dv;

    p = knots;

    /* Initialize our loop invariant and running variables.
     * dx and dy is the
     * direction vector that ends in our current knot.
     * We add up the angles
     * between this vector and subsequent vectors,
     * checking that each angle is positive.
     */
    alpha = 0.0;
    q = PRED(p);
    dx = p->x - q->x;
    dy = p->y - q->y;

    /* Check for duplicate point.
     */
    if ((0.0 == dx) && (0.0 == dy)) {
        return -1;
    }
    do {
        /* invariant:
         * - alpha accumulates total angle of left turns
         * - dx, dy = previous segment direction vector
         * - p points to current knot
         * - control points for p and q are updated
         * - each angle theta is strictly > 0 (left turn)
         */
        q = SUCC(p);
        du = q->x - p->x;
        dv = q->y - p->y;

        /* Initialize control points.
         */
        p->right_info.e.x = p->x + du / 3.0;
        p->right_info.e.y = p->y + dv / 3.0;
        q->left_info.e.x = q->x - du / 3.0;
        q->left_info.e.y = q->y - dv / 3.0;

        /* Check for duplicate point.
         */
        if ((0.0 == du) && (0.0 == dv)) {
            return -1;
        }
        theta = reduce_angle(atan2(dv, du) - atan2(dy, dx));

        /* Right turns and straight segments are illegal,
         * we require that pens
         * are convex and oriented counter-clockwise.
         */
        if (0.0 >= theta) {
            return -2;
        }
        alpha += theta;
        dx = du;
        dy = dv;
        p = q;
    } while (p != knots);

    /* Check the winding number.  We allow only one revolution.
     */
    if (2 * M_PI < alpha) {
        return -3;
    }

    return 1;
}
/*
 * @brief
 *  Constructs a 1x1 square brush (convex polygon).
 *
 * @return
 *  Pointer to a cyclic list of 4 knots forming a square.
 *
 * @post
 *  The returned path is a closed convex shape (Brush);
 *  control points are implicitly valid.
 */

knot_t *brush_make_square() {
    knot_t *knots;

    knots = moveto((point_t){0.5, 0.5});
    knots = lineto(knots, (point_t){-0.5, 0.5});
    knots = lineto(knots, (point_t){-0.5, -0.5});
    knots = lineto(knots, (point_t){0.5, -0.5});
    pathclose(knots);

    /* The brush is verified from the way it is constructed, so
       we don't call brush_make */
    return knots;
}

/*
 * @brief
 *  Partition function for quicksort of tee_stack.
 *  Quick sort the times at which we split the curve.
 *
 * @pre
 *  lo <= hi; tee_stack[lo..hi] contains valid doubles.
 *
 * @post
 *  Elements <= pivot moved left of pivot index;
 *  returns final pivot position.
 */
static int partition_tees(uint32_t lo, uint32_t hi) {
    uint32_t i;
    uint32_t j;
    double pivot;
    double tee;

    pivot = tee_stack[lo];
    i = lo - 1;
    j = hi + 1;

    for (;;) {
        do {
            i++;
        } while (tee_stack[i] < pivot);

        do {
            j--;
        } while (tee_stack[j] > pivot);

        if (i >= j) {
            return j;
        }
        tee = tee_stack[i];
        tee_stack[i] = tee_stack[j];
        tee_stack[j] = tee;
    }
}
/*
 * @brief
 *  Sorts a range of the tee_stack using quicksort.
 *  Quick sort the times at which we split the curve.
 *
 * @pre
 *  tee_stack[lo..hi] must contain valid numeric entries.
 *
 * @post
 *  tee_stack[lo..hi] is sorted in ascending order.
 */

static void sort_tees(uint32_t lo, uint32_t hi) {
    uint32_t i;

    if (lo < hi) {
        i = partition_tees(lo, hi);
        sort_tees(lo, i);
        sort_tees(i + 1, hi);
    }
}

/*
 * @brief
 *  Solves a quadratic equation a*x^2 + b*x + c = 0
 *  using numerically stable method.
 *
 * @pre
 *  tee_top < sizeof(tee_stack) / sizeof(double) - 2;
 *  pass b as (u - v) form.
 *
 * @post Up to 2 real roots written to tee_stack[tee_top++];
 * returns number of roots (0–2).
 */

/* Solving quadratic equations aX^2+bX+c = 0
 *
 * Let B = -b/2 = u - v
 *  => b = -2B  = 2 * (v - u)
 *
 * If a == 0:
 *   x = -c/b = c/2B
 *
 * If c == 0 and a != 0:
 *   x = 0 and x = -b/a = 2B/a
 *
 * If c != 0:
 *   x = (-b +/- sqrt(b^2 - 4ac))     / 2a
 *   x = (2B +/- sqrt(B^2 -  ac) * 2) / 2a
 *     = ( B +/- sqrt(B^2 -  ac))     /  a
 *
 * If c != 0 then alternatively:
 *   x = 2c / (-b +/- sqrt(b^2 - 4ac))
 *   x =  c / ( B +/- sqrt(B^2 -  ac))
 *
 * Pass B instead of b as the second argument!
 */
static int solve_quadratic(double a, double b, double c) {
    double d;

    if (0.0 == a) {
        if (0.0 != b) {
            tee_stack[tee_top++] = c / (2.0 * b);
            return 1;
        }
        return 0;
    }
    if (0.0 == c) {
        tee_stack[tee_top++] = 0.0;

        if (0.0 != b) {
            tee_stack[tee_top++] = 2.0 * b / a;
            return 2;
        }
        return 1;
    }
    d = (b * b) - (a * c);

    if (0.0 > d) {
        return 0;
    }
    d = sqrt(d);

    if (0.0 == d) {
        tee_stack[tee_top++] = b / a;
        return 1;
    }
    /* Avoid subtracting two numbers with the same sign to achieve better
     * numerical stability.  See also:
     * http://people.csail.mit.edu/bkph/articles/Quadratics.pdf
     */
    if (0.0 > b) {
        tee_stack[tee_top++] = c / (b - d);
        tee_stack[tee_top++] = (b - d) / a;
    } else {
        tee_stack[tee_top++] = c / (b + d);
        tee_stack[tee_top++] = (b + d) / a;
    }
    return 2;
}

/*
 * @brief
 *  Solves Bezier root polynomial using specialized form of
 *  quadratic equation.
 *
 * @pre
 *  Values represent sampled Bezier points u, v, w in 1D.
 *
 * @post
 *  Roots stored in tee_stack[] via solve_quadratic.
 */

/* Solving Bezier roots:
 *
 * B(t) * 1/3
 *   = u * (1-t)^2 + 2v * (1-t)t + w * t^2
 *   = u * (t^2 - 2t + 1) - 2v * (t^2 - t) + w * t^2
 *   = t^2 * (u - 2v + w) + t * 2 * (v - u) + u
 *
 * Pass B = v - u instead of 2 * (v - u) as the second argument.
 */
static int solve_bezier(double u, double v, double w) {
    return solve_quadratic(u - v - v + w, u - v, u);
}

/*
 * @brief
 *  Computes inflection points of a cubic Bezier segment
 *  and stores them in tee_stack.
 *
 * @param
 * knots       Pointer to the start of a cubic curve segment.
 *
 * @pre
 *  knots and SUCC(knots) are valid and
 *  have explicit control points (ri.e, li.e).
 *
 * @post
 *  tee_stack is updated with inflection point parameters
 *  in (0,1) domain.
 */

/* We split the curve at inflection points and wherever the slope equals the
 * slope of a pen's side.  The formula for computing inflection points comes
 * from Pomax, "A Primer on Bezier Curves."
 */
void inflection_tees(knot_t *knots) {
    knot_t *p;
    knot_t *q;
    double x0, y0, x1, y1, x2, y2, x3, y3;
    double t0, t1, t2;
    double a, b, c, d;
    double m;

    p = knots;
    q = SUCC(p);

    /* Translate the curve to (0,0). x0 and y0 become 0. */
    x1 = p->right_info.e.x - p->x;
    y1 = p->right_info.e.y - p->y;
    x2 = q->left_info.e.x - p->x;
    y2 = q->left_info.e.y - p->y;
    x3 = q->x - p->x;
    y3 = q->y - p->y;

    /* Rotate the curve right so that y3 = 0.  This maps the base vector (1,0)
     * to (x3 -y3) and (0,1) to (y3 x3).  We write the results to the points
     * starting with x0 and y0 to save temporary variables.
     */
    m = hypot(x3, y3);
    t0 = x3 / m;
    t1 = y3 / m;
    x0 = ROR_X(t0, t1, x1, y1);
    y0 = ROR_Y(t0, t1, x1, y1);
    x1 = ROR_X(t0, t1, x2, y2);
    y1 = ROR_Y(t0, t1, x2, y2);
    x2 = ROR_X(t0, t1, x3, y3);
    // y2 = ROR_Y(t0, t1, x3, y3); ???

    /* Precompute values we need to compute
     * the time of inflection points.
     */
    a = x1 * y0;
    b = x2 * y0;
    c = x0 * y1;
    d = x2 * y1;

    /* Actually, y = 18(3a-b-3c) but we must pass B = -b/2.
     */
    t0 = 18 * (-3 * a + 2 * b + 3 * c - d);
    t1 = 9 * (-3 * a + b + 3 * c);
    t2 = 18 * (c - a);

    solve_quadratic(t0, t1, t2);
}

/* Computes the tees at which the curve must be split to insert the
 * moves of pens.
 *
 * Also computes the tees at diagonal, horizontal and vertical slopes
 * (unless commented out below).  The latter assures that the curve
 * segments are monotonic within each octant so that we can use
 * approaches similar to METAFONT, for example, rounding,
 * smoothing and computing discrete moves.
 *
 * For the sake of simplicity we accept some inefficiency
 * because several curve segments are convoluted with pen
 * turns unnecessarily.
 */
void pen_tees(knot_t *knots, knot_t *pen) {
    knot_t *p;
    knot_t *q;
    double x0, y0, x1, y1, x2, y2;
    double t0, t1, t2;
    double dx, dy;

    p = knots;
    q = SUCC(p);

    /* Compute first derivative B'(t) */
    x0 = p->right_info.e.x - p->x;
    y0 = p->right_info.e.y - p->y;
    x1 = q->left_info.e.x - p->right_info.e.x;
    y1 = q->left_info.e.y - p->right_info.e.y;
    x2 = q->x - q->left_info.e.x;
    y2 = q->y - q->left_info.e.y;

    /* Split at diagonal angles. */
    // solve_bezier(y0 - x0, y1 - x1, y2 - x2);
    // solve_bezier(y0 + x0, y1 + x1, y2 + x2);

    /* Split at horizontal and vertical angles. */
    // solve_bezier(x0, x1, x2);
    // solve_bezier(y0, y1, y2);

    p = pen;

    do {
        /* invariant:
         * - for each r in the pen path, compute a root of the difference
         *   between curve slope and segment slope
         * - tee_stack grows with valid t values (0 <= t <= 1)
         */
        q = SUCC(p);

        /* Compute a Bezier curve with roots at the desired slope. */
        dx = q->x - p->x;
        dy = q->y - p->y;
        t0 = y0 * dx - x0 * dy;
        t1 = y1 * dx - x1 * dy;
        t2 = y2 * dx - x2 * dy;

        solve_bezier(t0, t1, t2);

        p = q;
    } while (p != pen);
}

/*
 * @brief
 *  Linearly interpolates between a and b at parameter t.
 *
 * @pre
 *  0 <= t <= 1.
 *
 * @return
 *  Interpolated value a + t*(b - a).
 */

static inline double t_of_the_way(double t, double a, double b) {
    return a + t * (b - a);
}

/*
 * @brief
 *  Splits the Bezier curve between p and SUCC(p)
 *  at parameter t into two curves.
 *
 * @param
 * p           Knot with explicit control points,
 *             followed by SUCC(p) with explicit points.
 *
 * t           Split point in (0,1).
 *
 * @pre
 *  p and SUCC(p) are valid;
 *  control points are initialized; 0 < t < 1.
 *
 * @invariant (conceptual):
 * - u0..u2, v0..v1, w0 are results of recursive de Casteljau splitting
 * - resulting p, r, q have continuous curve representation
 *
 * @post
 *  Inserts a new knot r between p and SUCC(p),
 *  adjusting control points appropriately.
 *
 * @return
 *  Newly inserted knot.
 */

/* Pre: the knot p and its successor q
 * must have explicit control points; t
 * is between 0 and 1, exclusively.
 * Post: in between p and q is a new knot.
 * Return: the newly created knot.
 */
static knot_t *cubic_split(knot_t *p, double t) {
    double u0, u1, u2;
    double v0, v1;
    double w0;
    double a0, a1, a2;
    double b0, b1;
    double c0;
    knot_t *r;
    knot_t *q;

    q = SUCC(p);

    /* Compute (but not set) the control points of the split curve.
     * While not needed in this version of the code anymore,
     * this allows us to check the
     * outcome before changing and adding a knot to the curve.
     */
    u0 = t_of_the_way(t, p->x, p->right_info.e.x);
    u1 = t_of_the_way(t, p->right_info.e.x, q->left_info.e.x);
    u2 = t_of_the_way(t, q->left_info.e.x, q->x);
    v0 = t_of_the_way(t, u0, u1);
    v1 = t_of_the_way(t, u1, u2);
    w0 = t_of_the_way(t, v0, v1);

    a0 = t_of_the_way(t, p->y, p->right_info.e.y);
    a1 = t_of_the_way(t, p->right_info.e.y, q->left_info.e.y);
    a2 = t_of_the_way(t, q->left_info.e.y, q->y);
    b0 = t_of_the_way(t, a0, a1);
    b1 = t_of_the_way(t, a1, a2);
    c0 = t_of_the_way(t, b0, b1);

    r = knots_get();

    list_insert_after(&p->list_node, &r->list_node);

    /* Adjust the control points of the split curve.
     */
    r->x = w0;
    p->right_info.e.x = u0;
    r->left_info.e.x = v0;
    r->right_info.e.x = v1;
    q->left_info.e.x = u2;

    r->y = c0;
    p->right_info.e.y = a0;
    r->left_info.e.y = b0;
    r->right_info.e.y = b1;
    q->left_info.e.y = a2;

    r->left_type = kt_explicit;
    r->right_type = kt_explicit;
    return r;
}

/*
 * @brief
 *  Splits all curve segments at tee parameters
 *  (inflections, pen slopes).
 *
 * @param
 * knots       Cyclic list of knots forming a curve.
 * brush       Cyclic list of knots forming the pen shape.
 *
 * @pre
 *  knots and brush are valid, cyclic, and have proper control points.
 *
 * @post
 *  Curve is split into segments at calculated tee values.
 */

void split_at_tees(knot_t *knots, knot_t *brush) {
    uint32_t i;
    knot_t *p;
    double s;
    double t;
    double x;
    double m;

    p = knots;

    do {
        tee_top = 0;
        i = 0;

        /* invariant before filtering:
         * - tee_stack[0..tee_top-1] contains raw t-values
         *   from inflections + slope matches
         */

        inflection_tees(p);
        pen_tees(p, brush);

        /* Filter out tees outside the Bezier interval. */
        while (i < tee_top) {
            /* invariant:
             * - 0 < tee_stack[0..i-1] < 1
             * - tee_stack[0..tee_top-1] remains valid after filtering
             */
            m = tee_stack[i];

            if ((0 < m) && (1 > m)) {
                i++;
                continue;
            }
            tee_top--;
            tee_stack[i] = tee_stack[tee_top];
        }
        if (1 < tee_top) {
            sort_tees(0, tee_top - 1);
        }
        s = 0.0;

        for (i = 0; i < tee_top; i++) {
            /* invariant:
             * - p points to the current knot before the i-th split
             * - s is the last split location in [0,1)
             * - t is a valid normalized t-value between s and 1
             */
            x = tee_stack[i];

            // fprintf(stderr, "tee[%d] = %f\n", i, x);

            if (s != x) {
                t = (x - s) / (1.0 - s);
                p = cubic_split(p, t);
                s = x;
            }
        }
        p = SUCC(p);
    } while ((p != knots) && (kt_regular != p->right_type));
}

/*
 * @brief
 *  Creates a stroke segment by offsetting knot p
 *  with r and writing to trace buffer.
 *
 * @pre
 *  trace_top + 4 < TRACE_MAX; r and p are valid knots;
 *  SUCC(p) exists.
 *
 * @post
 *  4 control points of a new curve written
 *  to trace[trace_top..trace_top+3].
 */
static void make_move(knot_t *r, knot_t *p) {
    uint32_t i;
    knot_t *q;

    q = SUCC(p);
    i = trace_top;

    trace[i].x = r->x + p->x;
    trace[i].y = r->y + p->y;
    trace[i + 3].x = r->x + q->x;
    trace[i + 3].y = r->y + q->y;
    trace[i + 1].x = r->x + p->right_info.e.x;
    trace[i + 1].y = r->y + p->right_info.e.y;
    trace[i + 2].x = r->x + q->left_info.e.x;
    trace[i + 2].y = r->y + q->left_info.e.y;
    trace_top += 4;
}

/*
 * @brief
 *  Checks if vector (du,dv) is to the right (clockwise) of (dx,dy).
 *
 * @return
 *  1          if clockwise or nearly colinear,
 *  0          otherwise.
 */

static int clockwise(double dx, double dy, double du, double dv) {
    double d;

    d = dx * dv - dy * du;

    if (fabs(d) < SMALL) {
        return 1;
    }
    return (0.0 <= d);
}

/*
 * @brief
 *  Determines whether the sequence of vectors
 *  forms a proper turn (within convex arc).
 *
 * @pre
 *  Inputs are vectors forming two consecutive turns.
 *
 * @return
 * 1           if within convex angle, 0 otherwise.
 */
static int within_turn(double x1, double y1, double x2, double y2, double x3,
                       double y3) {
    if (!clockwise(x1, y1, x2, y2)) { /* strictly right turn */
        return clockwise(x2, y2, x3, y3) && clockwise(x3, y3, x1, y1);
    }
    return clockwise(x1, y1, x3, y3) && clockwise(x3, y3, x2, y2);
}

/*
 * @brief
 *  Applies brush r to curve p if directional match permits.
 *
 * @param
 * p           Curve knot
 * x1,y1       Incoming vector,
 * x2,y2       direction vector,
 * x3,y3       outgoing vector
 * r           Current brush knot
 *
 * @pre
 *  p and r are valid; vectors computed from control points.
 *
 * @post
 *  Writes one or two new moves to trace buffer
 *  if turn conditions are met.
 */

// p is curve/knots and r is brush
void convolve(knot_t *p, double x1, double y1, double x2, double y2,
              double x3, double y3, knot_t *r) {
    knot_t *s;
    double x4, y4;
    double x5, y5;

    s = PRED(r);
    x4 = r->x - s->x;
    y4 = r->y - s->y;

    s = SUCC(r);
    x5 = s->x - r->x;
    y5 = s->y - r->y;

    if (within_turn(x1, y1, x2, y2, x5, y5)) {
        make_move(p, r);
    }
    if (within_turn(x4, y4, x5, y5, x3, y3)) {
        make_move(r, p);
    }
}

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

void convolve_all(knot_t *knots, knot_t *brush) {
    knot_t *p;
    knot_t *q;
    knot_t *r;
    double x1, x2, x3;
    double y1, y2, y3;

    trace_top = 0;
    p = knots;

    do {
        /* invariant:
         * - p points to the current knot before the i-th split
         * - s is the last split location in [0,1)
         * - t is a valid normalized t-value between s and 1
         */
        q = SUCC(p);
        r = brush;

        /* Incoming and outgoing angle at ends of curve. */
        x2 = p->right_info.e.x - p->x;
        y2 = p->right_info.e.y - p->y;

        if (kt_explicit == p->left_type) {
            x1 = p->x - p->left_info.e.x;
            y1 = p->y - p->left_info.e.y;
        } else {
            x1 = -x2;
            y1 = -y2;
        }
        x3 = q->x - p->x;
        y3 = q->y - p->y;

        do {
            convolve(p, x1, y1, x2, y2, x3, y3, r);

            r = SUCC(r);
        } while (r != brush);

        p = q;
    } while ((p != knots) && (kt_regular != p->right_type));
}

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

void show_segments(brush_stroke_t *stroke) {
    uint32_t i;
    size_t beziers_top;

    beziers_top = stroke->length;

  /* Allocate for forward and return path on first call
     amount of entries: stroke->beziers = 1/4 * trace_top
     trace_top needs to be put into stroke->beziers 2x (forward+return)
     => amount of entries: stroke->beziers = 1/2 * trace_top
     However, we need a margin of safety */
  if (stroke->beziers == NULL) {
    stroke->beziers = malloc(trace_top* 2/3 * sizeof *trace_top);
  }

    // If not allocated at this point, there was a malloc error
    if (!stroke->beziers) {
        exit(-1);
    }

    for (i = 0; i < trace_top; i += 4) {
        /* invariant:
         * - i is divisible by 4
         * - each group trace[i]..trace[i+3] forms a valid cubic segment
         * - stroke->beziers[beziers_top + i/4] is correctly written
         */
        stroke->beziers[beziers_top + i / 4] = (return_cubic_t){
            .start = {trace[i].x, trace[i].y},
            .c1 = {trace[i + 1].x, trace[i + 1].y},
            .c2 = {trace[i + 2].x, trace[i + 2].y},
            .end = {trace[i + 3].x, trace[i + 3].y},
        };
        stroke->length += 1;
    }
}