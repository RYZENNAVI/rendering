#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include "knots.h"

int are_points_the_same(knot p1, knot p2);

double euclidean_distance(knot p1, knot p2);

knot add_knot(knot p1, knot p2);

knot sub_knot(knot p1, knot p2);

knot scale(knot p1, double mu);

double angle_between(knot p1, knot p2);

double norm(knot p1, knot p2);

knot rotate_vectors(knot p1, knot p2, double theta);

#endif