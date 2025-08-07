/*
@file transforms.c
@authors Matthias, Toni
@date 01.07.25
*/

#include "../inc/common.h"

void brush_tf(knot_t *first, double transform[3][3]) {
    knot_t *k = first;
    knot_t *prev = (knot_t *) k->list_node.pred;

    do { /* Iterate over all knots */
        knot_t *next = (knot_t *) k->list_node.succ;
        double brush_out[3] = {0.0, 0.0, 0.0};
        double brush_in[3] = {k->x, k->y, 1.0};

        for (int i = 0; i < 3; ++i) /* Transform knot */
            brush_out[i] = transform[i][0] * brush_in[0] +
                           transform[i][1] * brush_in[1] +
                           transform[i][2] * brush_in[2];

        k->x = brush_out[0];
        k->y = brush_out[1];
        // printf("brush x: %f, brush y: %f\n", k->x, k->y);

        if (next->left_type ==
            kt_explicit) { /* left neighbour of next knot = current knot */
            next->left_info.e.x = k->x;
            next->left_info.e.y = k->y;
        }

        if (prev->right_type ==
            kt_explicit) { /* right neighbour of prev knot = current knot */
            prev->right_info.e.x = k->x;
            prev->right_info.e.y = k->y;
        }

        prev = k;
        k = next;
    } while (k != first);
}

void brush_tf_translate(knot_t *first, point_t dest) {
    knot_t *k = first;

    double xmin = k->x;
    double xmax = k->x;
    double ymin = k->y;
    double ymax = k->y;

    do { /* Find maximums and minimums */
        if (k->x < xmin) xmin = k->x;
        if (k->x > xmax) xmax = k->x;
        if (k->y < ymin) ymin = k->y;
        if (k->y > ymax) ymax = k->y;

        k = (knot_t *) k->list_node.succ;
    } while (k != first);

    point_t center = {(xmin + xmax) / 2.0,
                      (ymin + ymax) / 2.0}; /* Find center */

    const double trans_x =
        dest.x - center.x; /* Calculate translation vector */
    const double trans_y = dest.y - center.y;

    double transform[3][3] = {
        {1.0, 0.0, trans_x}, {0.0, 1.0, trans_y}, {0.0, 0.0, 1.0}};

    brush_tf(first, transform); /* Apply translation vector */
}

void brush_tf_shear(knot_t *first, double shear, enum axis_t axis) {
    double transform[3][3] = {
        {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

    if (axis == X) transform[0][1] = shear;
    if (axis == Y) transform[1][0] = shear;

    brush_tf(first, transform);
}

void brush_tf_rotate(knot_t *first, double theta, enum axis_t axis) {

    /* Conversion to radians */
    theta = theta * M_PI / 180.0;

    if (axis == X) { /* Pitch */
        double transform[3][3] = {{1.0, 0.0, 0.0},
                                  {0.0, cos(theta), -sin(theta)},
                                  {0.0, sin(theta), cos(theta)}};

        brush_tf(first, transform);
    }
    if (axis == Y) { /* Yaw */
        double transform[3][3] = {{cos(theta), 0.0, sin(theta)},
                                  {0.0, 1.0, 0.0},
                                  {-sin(theta), 0.0, cos(theta)}};

        brush_tf(first, transform);
    }
    if (axis == Z) { /* Roll */
        double transform[3][3] = {{cos(theta), -sin(theta), 0.0},
                                  {sin(theta), cos(theta), 0.0},
                                  {0.0, 0.0, 1.0}};

        brush_tf(first, transform);
    }
}

void brush_tf_reflect(knot_t *first, enum axis_t axis) {
    if (axis == X) { /* Flip on x-axis */
        double transform[3][3] = {
            {1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}};

        /* We need to reverse the order to keep the
           order of knots counterclockwise */
        first = knots_ring_reverse(first);
        brush_tf(first, transform);
    }
    if (axis == Y) { /* Flip on y-axis */
        double transform[3][3] = {
            {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

        /* We need to reverse the order to keep the
           order of knots counterclockwise */
        first = knots_ring_reverse(first);
        brush_tf(first, transform);
    }
}

void brush_tf_resize(knot_t *first, double scale, enum axis_t axis) {
    /* set X and Y scaling */
    double sx = (axis == X || axis == Z) ? scale : 1.0;
    double sy = (axis == Y || axis == Z) ? scale : 1.0;

    double transform[3][3] = {
        {sx, 0.0, 0.0}, {0.0, sy, 0.0}, {0.0, 0.0, 1.0}};

    /* Scaling the brush */
    brush_tf(first, transform);
}
