// Copyright 2025 Prof. Dr.-Ing. Volker Roth
// See accompanying README for license information (wasn't provided?)
// Rewrite author: Iaroslav Tretiakov
// Date: 09.06.2025

#ifndef KNOTS_H
#define KNOTS_H

#define KNOT_MAX 1024

#include "list.h"
#include <stdint.h>

typedef enum {
    /* A normal startpoint / endpoint of a path segment. Used
     * for regular path nodes. */
    kt_regular = 0,
    /* Explicitly given control point. */
    kt_explicit = 1,
    /* The "tension" (how muWch control points affect regular
     * points) is given explicitly.  */
    kt_given = 2,
    /* The "curl" (a kind of smoothness or curvature control) is
     * given explicitly. */
    kt_curl = 3,
    /* An open knot — this point may not be part of a closed path
     * or may have undefined direction. */
    kt_open = 4
} knot_type;

typedef struct knot_explicit knot_explicit;
typedef struct knot_given knot_given;
typedef struct knot_curl knot_curl;
typedef struct knot_t knot_t;

struct knot_explicit {
    double x;
    double y;
};

//? Perhaps it's better to describe how to use it right here.
struct knot_given {
    double tension; // alpha on right/outgoing, beta on left/incoming
    double angle;
};

//? Perhaps it's better to describe how to use it right here.
struct knot_curl {
    double tension; // alpha on right/outgoing, beta on left/incoming
    double curl;
};

typedef union {
    knot_explicit e;
    knot_given g;
    knot_curl c;
} knot_info;

struct knot_t {
    node list_node;
    double x;
    double y;
    knot_info left_info;  // going into knot
    knot_info right_info; // going out of knot
    knot_type left_type;
    knot_type right_type;
};

knot_t *knots_get(void);
knot_t *knots_ring_clone(knot_t *knots);

void knots_init(void);
void knots_free(knot_t *knot);
void knots_copy(knot_t *dst, knot_t *src);
void knots_ring_free(knot_t *knot);

knot_t *knots_ring_reverse(knot_t *knots);
knot_t *knots_split(knot_t *target_knot);

#endif