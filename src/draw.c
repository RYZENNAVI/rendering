/*
@file draw.c
@authors Matthias, Toni
@date 01.07.25
*/

#include "brush.h"
#include "common.h"

knot_t *moveto(point_t point) {
    knot_t *knot;

    knot = knots_get(); /* pointer to an unused or new knot */

    /* self-link the embedded list node */
    knot->list_node.pred = &knot->list_node;
    knot->list_node.succ = &knot->list_node;

    /* scalar members */
    knot->x = point.x;
    knot->y = point.y;
    knot->left_type = kt_open;
    knot->right_type = kt_regular;

    return knot;
}

knot_t *rmoveto(knot_t *old, point_t p) {
    knot_t *r;

    r = (knot_t *) old->list_node.pred;

    knot_t *knot = knots_get(); /* pointer to an unused or new knot */

    /* self-link the embedded list node */
    knot->list_node.pred = &knot->list_node;
    knot->list_node.succ = &knot->list_node;

    /* scalar members */
    knot->x = r->x + p.x;
    knot->y = r->y + p.y;
    knot->left_type = kt_open;
    knot->right_type = kt_regular;

    return knot;
}

knot_t *lineto(knot_t *p, point_t point) {
    knot_t *q;
    knot_t *r;

    r = (knot_t *) p->list_node.pred;

    q = knots_get();
    q->x = point.x;
    q->y = point.y;
    q->right_type = kt_regular;

    list_insert_before(&q->list_node, &p->list_node);

    r->right_type = kt_explicit;
    r->right_info.e.x = r->x + 0.3 * (q->x - r->x);
    r->right_info.e.y = r->y + 0.3 * (q->y - r->y);
    q->left_type = kt_explicit;
    q->left_info.e.x = r->x + 0.7 * (q->x - r->x);
    q->left_info.e.y = r->y + 0.7 * (q->y - r->y);
    return p;
}

knot_t *rlineto(knot_t *t, point_t p) {
    knot_t *q;
    knot_t *r;

    r = (knot_t *) t->list_node.pred;

    q = knots_get();
    q->x = r->x + p.x;
    q->y = r->y + p.y;
    q->right_type = kt_regular;

    list_insert_before(&q->list_node, &t->list_node);

    r->right_type = kt_explicit;
    r->right_info.e.x = r->x + 0.3 * (q->x - r->x);
    r->right_info.e.y = r->y + 0.3 * (q->y - r->y);
    q->left_type = kt_explicit;
    q->left_info.e.x = r->x + 0.7 * (q->x - r->x);
    q->left_info.e.y = r->y + 0.7 * (q->y - r->y);
    return t;
}

knot_t *curveto(knot_t *p, point_t control1, point_t control2, point_t end) {
    knot_t *q;
    knot_t *r;

    r = (knot_t *) p->list_node.pred;

    q = knots_get();
    q->x = end.x;
    q->y = end.y;
    q->right_type = kt_regular;

    list_insert_before(&q->list_node, &p->list_node);

    r->right_type = kt_explicit;
    r->right_info.e.x = control1.x;
    r->right_info.e.y = control1.y;
    q->left_type = kt_explicit;
    q->left_info.e.x = control2.x;
    q->left_info.e.y = control2.y;
    return p;
}

knot_t *pathclose(knot_t *p) {
    knot_t *q;
    knot_t *r;

    if (!p) return NULL;

    r = (knot_t *) p->list_node.pred;
    q = p;

    if (r != p) { /* Add straight line */
        r->right_type = kt_explicit;
        r->right_info.e.x = r->x + 0.3 * (q->x - r->x);
        r->right_info.e.y = r->y + 0.3 * (q->y - r->y);
        q->left_type = kt_explicit;
        q->left_info.e.x = r->x + 0.7 * (q->x - r->x);
        q->left_info.e.y = r->y + 0.7 * (q->y - r->y);
    }

    return p;
}

brush_stroke_t *draw_shape(knot_t *path, knot_t *brush, color_t color) {
    knot_t *return_path;

    brush_stroke_t *stroke = malloc(sizeof *stroke);

    if (stroke == NULL) {
        exit(-1); // malloc error
    }

    stroke->color = color;
    stroke->length = 0;
    stroke->beziers = NULL; // Not yet initialized

    /* Verify brush */
    brush_make(brush);

    /* Align brush with path. As the logic in convolve_all expects
       the brush to have the center 0,0, it is translated to be safe */
    brush_tf_translate(brush, (point_t){0.0, 0.0});

    /* Draw path */
    split_at_tees(path, brush);
    convolve_all(path, brush);
    show_segments(stroke);

    /* Draw return path */
    return_path = knots_ring_reverse(knots_ring_clone(path));
    convolve_all(return_path, brush);
    show_segments(stroke);

    /* Free memory */
    knots_ring_free(path);
    knots_ring_free(return_path);

    return stroke;
}