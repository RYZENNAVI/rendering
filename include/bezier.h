// Authors: Iaroslav Tretiakov, Evis Bregu
// Date: 06.06.2025

#ifndef BEZIER_H
#define BEZIER_H

#include <knots.h>
#include <stdint.h>

// Based on Jake's Low js implementation:
// https://www.jakelow.com/blog/hobby-curves/hobby.js

/**
 * @param knots pointer to the list of knots
 * @return The number of control points replaced in `knots`.
 * @brief Defines a cubic Bézier curve through 2 regular points and 2 implicit
 * control points.
 */
int set_controls_implicit(knot *knots);

#endif