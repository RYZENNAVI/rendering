#include "math_functions.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Tests if two points are the same.
 * @param p1 is a 2D point
 * @param p2 is the point we are comparing p1 to
 * @return if p1 and p2 have the exact same coordinates
 */
int are_points_the_same(knot_t p1, knot_t p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

/**
 * Computes the 2D euclidean distance between two points.
 * @param p1 first point
 * @param p2 second point
 * @return the distance between two points
 */

double euclidean_distance(knot_t p1, knot_t p2) {

    double delta_x_squared = pow(p1.x - p2.x, 2);
    double delta_y_squared = pow(p1.y - p2.y, 2);

    double squared_sum = delta_x_squared + delta_y_squared;
    assert(squared_sum >= 0);
    return sqrt(squared_sum);
}

/**
 * Adds two knots.
 * @param p1 first knot
 * @param p2 second knot
 * @return a new knot that is the sum of p1 and p2
 */
knot_t add_knot(knot_t p1, knot_t p2) {
    knot_t result;

    result.x = p1.x + p2.x;
    result.y = p1.y + p2.y;

    return result;
}

/**
 * Subtracts two knots.
 * @param p1 first knot
 * @param p2 second knot
 * @return a new knot that is the difference of p1 and p2
 */
knot_t sub_knot(knot_t p1, knot_t p2) {
    knot_t result;

    result.x = p1.x - p2.x;
    result.y = p1.y - p2.y;

    return result;
}

/**
 * Computes the angle between two knots.
 * @param p1 first knot
 * @param p2 second knot
 * @return the angle in radians between the two knots
 */

double angle_between(knot_t p1, knot_t p2) {
    double dot_prod = (p1.x * p2.x) + (p1.y * p2.y);
    double p1_magn = sqrt((p1.x * p1.x) + (p1.y * p1.y));
    double p2_magn = sqrt((p2.x * p2.x) + (p2.y * p2.y));

    if (p1_magn == 0 || p2_magn == 0) {
        dprintf(2, "p1 magnitude or p2 magnitude is 0!Quiting...");
        exit(1);
    }

    double dot_over_length = dot_prod / (p1_magn * p2_magn);

    if (dot_over_length > 1.0)
        dot_over_length = 1.0;
    else if (dot_over_length < -1.0)
        dot_over_length = -1.0;

    double angle = acos(dot_over_length);
    return angle;
}

/**
 * Scales a knot by a given factor.
 * @param p1 is the knot to be scaled
 * @param mu is the scaling factor
 * @return a new knot that is the scaled version of p1
 */

knot_t scale(knot_t p1, double mu) {
    knot_t result;

    result.x = p1.x * mu;
    result.y = p1.y * mu;

    return result;
}

/**
 * Computes the norm of the vector between two knots.
 * @param p1 first knot
 * @param p2 second knot
 * @return the length of the vector from p1 to p2
 */

double norm(knot_t p1, knot_t p2) {
    double x_diff = p2.x - p1.x;
    double y_diff = p2.y - p1.y;

    return sqrt((x_diff * x_diff) + (y_diff * y_diff));
}

/**
 * Rotates the vector from p1 to p2 by a given angle theta.
 * @param p1 is the origin of the vector
 * @param p2 is the destination of the vector
 * @param theta is the angle in radians by which to rotate the vector
 * @return a new knot that represents the rotated vector
 */

knot_t rotate_vectors(knot_t p1, knot_t p2, double theta) {

    double x_diff = p2.x - p1.x;
    double y_diff = p2.y - p1.y;

    double rotated_x = x_diff * cos(theta) - y_diff * sin(theta);
    double rotated_y = x_diff * sin(theta) + y_diff * cos(theta);

    knot_t p3;

    p3.x = p1.x + rotated_x;
    p3.y = p1.y + rotated_y;

    return p3;
}