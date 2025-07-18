## Interface Definition.

Team curves came up with 2 options for the interface

```C
#include <stdint.h>
#include "list.h"

#ifndef BEZIER_H
#define BEZIER_H

//? Option 1: Reuse prof. Roth's code
// =================================================================


//? Option 1.1: Brushes do the conversion, have info about tpyes and pints structure
// =================================================================
#define KNOT_MAX 1024

typedef enum {
  knot_endpoint_type  = 0, //* A normal endpoint of a path segment.
  knot_explicit_type  = 1, //* The direction at this point is explicitly specified using a direction vector.
  knot_given_type     = 2, //* The tension is given explicitly.
  knot_curl_type      = 3, //* The curl is specified.
  knot_open_type      = 4  //* An open knot—this point may not be part of a closed path or may have undefined direction.
} knot_type_t;

typedef struct knot_explicit_t knot_explicit_t;
typedef struct knot_given_t knot_given_t;
typedef struct knot_curl_t knot_curl_t;
typedef struct knot_t knot_t;

struct knot_explicit_t {
  double x;
  double y;
};

struct knot_given_t {
  double tension; //* alpha on right/outgoing, beta on left/incoming
  double angle;
};

struct knot_curl_t {
  double tension; //* alpha on right/outgoing, beta on left/incoming
  double curl;
};

typedef union {
  knot_explicit_t e;
  knot_given_t    g;
  knot_curl_t     c;
} knot_info_t;


/* 
 * The values of dx/dy are used differently depending on whether the knot
 * represents a point of a Bezier curve or a pen edge.  In a curve point,
 * dx/dy represent the pen offset of an envelope.  In a pen, dx/dy represent
 * the difference to the next point of the pen in count-clockwise order.
 */
struct knot_t {
  lnod_t ln;
  double x;
  double y;
  knot_info_t li; // left info, going into knot
  knot_info_t ri; // right info, going out of knot
  knot_type_t lt; // left type
  knot_type_t rt; // right type
};
// =================================================================

// doubly linked list variant
void knots_make_choices(knot_t *knots);

/*
Pros:
- A lot of reuse, quicker implementation.
- Curves and Brushes already agreed on using the interface of knots, I think this includes knot_t

Cons:
- Team Brushes or we will have to convert points into this format.
- No support for quadratic curves.

*/
// =================================================================



//? Option 2: Use Jake Low js implementation (Hobby's algorithm)
// =================================================================

/**
 * Fits a Bézier spline to a sequence of points using Hobby's algorithm.
 *
 * @param points pointer to the array of Point points
 * @param omega Curl strength at the endpoints (usually 0 ≤ omega ≤ 1).
 * @param out_points Pointer to an array to store the output points (allocated by the caller).
 *                   Must be at least (3 * num_points - 2) * 2 doubles in size.
 * @return The number of output points written to `out_points`.
 */
int set_controls(const double *points, double omega, double *out_points);

/*
Pros:
- Fits nicely with the way Brushes want to handle points, likely no conversion needed.
- Algorithms easier to understand than what Roth did.

Cons:
- Might use external dependencies for math operations.
- Many math operations must be re-implemented based on the js code, medium amount of reuse.
- Less flexible solution.
- Curl and tension paramteres will need adjustments.

*/
// =================================================================
#endif
```

The first option would be just reusing most of the code given by professor Roth, the second option take a Javascript implementation prof. Roth gave us a link to: https://www.jakelow.com/blog/hobby-curves/hobby.js (08.06.2025). We decided to go for the second option, because this way, we can represent the control points explicitly rather than implicitly, and there would not be any need for a conversion to a new format chosen by other teams.
THe chosen option will be adapted to the needs of the project. I.e., handling of implicit control points should be implemented etc.