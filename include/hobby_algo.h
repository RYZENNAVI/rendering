#ifndef SWP_TRY1_H
#define SWP_TRY1_H

#include "math_functions.h"

typedef struct {
    knot_t *points;
    int length;
} knot_array;

int count_segments(knot_array *arr);

void segment_lengths(knot_array *arr, double *distance);

#endif