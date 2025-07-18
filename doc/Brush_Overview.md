**Overview**

This program builds a **vector drawing engine** using **cubic Bézier paths** and **shape-based brushes**. The system splits curves precisely at critical points (inflections, slope matches), then **convolves** a pen shape with the path geometry. This approach allows for detailed and expressive strokes suitable for **typographic**, **calligraphic**, or **illustrative rendering**.

The design is:

- **Modular**: separation between geometry in brush.c, data structures in common.c, and the usage in draw.c.
- **Mathematically robust**: ensures convexity, handles edge cases in curve splitting.
- **Efficient**: uses memory pooling and cyclic linked lists.

\---------------------------------------------------------------------------------------------------------------

**brush.c — Brush Geometry and Convolution Engine**

**Purpose:**

This module handles **geometric processing of brushes** (shapes used for drawing paths) and their **convolution** with Bézier paths. The core idea is to sweep a brush along a curve, generating Bézier segments named stroke.

**Key Concepts:**

- The brush is represented as a **closed convex path** - knot_t ring.
- The curve is divided at **inflection points** and **slope-matching points**.
- The trace\[\] buffer accumulates resulting **Bézier segments** for the stroke.
- **Mathematical operations** include solving quadratic equations and computing Bezier roots.

**Major Components:**

| **Function** | **Description** |
| --- | --- |
| brush_make() | Validates convexity and orientation of a brush shape. |
| inflection_tees() | Detects inflection points of a cubic Bézier segment. |
| pen_tees() | Detects points where curve slope matches a pen side. |
| cubic_split() | Splits a Bézier curve segment at a given parameter t. |
| split_at_tees() | Splits curve at the calculated t values (tees). |
| convolve() | Applies a brush position to the path if angle conditions are met. |
| convolve_all() | Performs convolution of entire brush with the curve. |
| show_segments() | Converts trace\[\] into Bézier stroke output. |

Note:

- The brush must be centered at (0,0) before convolution.
- Splitting increases curve resolution to ensure correct rendering.
- A global stack tee_stack\[\] and buffer trace\[\] manage computation state.

\---------------------------------------------------------------------------------------------------------------

**common.c — Linked List & Knot Management**

**Purpose:**

Provides the **infrastructure and memory management** for knots and paths. Supports operations on **doubly linked cyclic lists** and handles allocation of knot_t objects from a pool.

**Key Concepts:**

- Knots are linked via embedded lnod_t nodes.
- Knots are preallocated in blocks (KNOTS_INCREMENT) and reused via a pool.
- Paths are circular lists (rings), allowing efficient insertions and reversals.

**Major Components:**

| **Function** | **Description** |
| --- | --- |
| list_insert_after/before() | Linked list insertion. |
| list_add_head/tail() | Adds knot at beginning (head) / end (tail) of list. |
| list_remove() | Removes a knot from its list. |
| knots_get() | Gets a free knot from the pool (allocates block if needed). |
| knots_split() | Duplicates a knot and inserts it before the original. |
| knots_ring_clone() | Deep copies an entire ring of knots. |
| knots_ring_reverse() | Reverses a cyclic path and swaps control points. |
| reduce_angle() | Normalizes angles to \[-Pi, Pi\] range. |

Notes:

- All memory is reused unless freed explicitly with knots_pool_free().
- Reversal of a ring is essential for drawing return paths (backward strokes)

\---------------------------------------------------------------------------------------------------------------

**draw.c — Path Construction and Drawing Control**

**Purpose:**

Implements **user-facing drawing operations** by combining curve creation (moveto, lineto, etc.) with brush convolution logic from brush.c.

**Key Concepts:**

- A path is incrementally built by placing and linking knot_t objects.
- Control points are auto-assigned during lineto and curveto.
- The draw_shape() function handles the complete drawing process.

**Major Components:**

| **Function** | **Description** |
| --- | --- |
| moveto() | Starts a new path at absolute coordinates. |
| rmoveto() | Starts a new path at a relative offset. |
| lineto() | Appends a straight line segment. |
| rlineto() | reverse version of lineto(). |
| curveto() | Appends a cubic Bézier segment with explicit handles. |
| pathclose() | Connects end to start of path. |
| draw_shape() | **Master function: aligns brush, splits path, convolves, generates stroke.** |

**Notes:**

- Drawing involves **forward and backward** convolution to create a **double-sided stroke**.
- draw_shape() verifies and centers the brush, splits path curves, and collects stroke data.
- Memory is freed after drawing via knots_ring_free().

\---------------------------------------------------------------------------------------------------------------

**common.h – header file**

**Purpose:**

This header file defines **data structures** and **operations** used for representing, transforming, and rendering vector-based **brush strokes** and **paths** using **cubic Bézier curves**. It is part of a drawing or vector graphics system that allows creation and manipulation of shapes through chained "knot" points and geometric transformations.

**Core Components**

**1\. Data Structures**

| **Function** | **Description** |
| --- | --- |
| **color_t** | Represents an RGBA color using 8-bit components (0-255) |
| **point_t** | A 2D point with floating-point values |
| **return_cubic_t**: | Represents a cubic Bézier segment, including start/end points and two control points. |
| **brush_stroke_t**: | Represents a full stroke consisting of one or more cubic Bézier curves and a color. |

**2\. Path Construction Functions**

These functions operate on a circular doubly-linked list of knot_t:

| **Function** | **Description** |
| --- | --- |
| moveto(point) | Start a new path at a specific point |
| rmoveto(old, shift) | Move the brush relative to a previous knot. |
| lineto(knot, point) | Add a straight line to the path. |
| rlineto(knot, shift) | Add a line segment with relative positioning. |
| curveto(knot, ctrl1, ctrl2, end) | Append a Bézier curve. |
| pathclose(knot) | Close the path with a straight segment if needed. |

**3\. Brush Operations**

A "brush" is a closed path of knots representing the stroke contour:

| **Function** | **Description** |
| --- | --- |
| brush_make(knots) | Validates and converts a closed path into a usable brush contour. |
| brush_make_square(color) | Creates a square-shaped brush centered at the origin |
| draw_shape(path, brush, color) | Applies the brush to a shape, producing a Bézier-stroke rendering |

**4\. Transformations**

Brush transformations are applied using 3×3 matrices. The following utility functions wrap matrix operations for ease of use:

| **Function** | **Description** |
| --- | --- |
| brush_tf(brush, matrix) | General transformation (rotate, scale, translate...). |
| brush_tf_resize(brush, scale, axis) | Scale brush on selected axis. |
| brush_tf_rotate(brush, theta, axis) | Rotate brush around origin. |
| brush_tf_reflect(brush, axis) | Mirror brush along X or Y axis. |
| brush_tf_translate(brush, point) | Move brush to a specific coordinate. |
| brush_tf_shear(brush, factor, axis) | Apply a shearing distortion for effects. |

\---------------------------------------------------------------------------------------------------------------

**Architectural Notes**

- The code uses **circular, doubly-linked lists** (knot_t) to represent paths, allowing seamless path construction and manipulation.
- **Transformations** (scale, rotation, shear) are all applied using homogeneous 3×3 matrices.
- Paths can be **open or closed**, and Bézier segments are handled explicitly through control point interpolation.
- Bézier handles (li, ri) are automatically computed for transitions unless specified manually.

**Pros**

- Clean separation between **geometry creation** and **rendering**.
- Modular and reusable API for shape manipulation.
- Well-suited for **digital drawing applications**, **vector graphics editors**, or **path-based brush simulations** (e.g., in animation or design tools).

\---------------------------------------------------------------------------------------------------------------

**brush.h – heasder file**

**Purpose:**

The brush.h header file defines internal functions that are responsible for applying **geometric brush strokes** to paths defined by **cubic Bézier curves**. These routines are not intended to be used directly by end-users, but serve as key components in rendering strokes with variable-width brush shapes.

It extends the functionality defined in common.h by handling:

- Brush-to-path convolution
- Inflection-aware segmentation
- Bézier curve generation for rendering

**Key Components and Functions**

**1\. show_segments(brush_stroke_t \*stroke)**

- **Role**: Converts the internal trace buffer (result of convolutions) into explicit cubic Bézier segments.
- **Input**: A brush_stroke_t structure (may be partially filled).
- **Output**: Fills or appends to stroke->beziers and updates the segment count (length).
- **Use Case**: After the convolution process, this function converts sampled results into a usable geometric format for drawing.

**2\. split_at_tees(knot_t \*knots, knot_t \*pen)**

- **Role**: Enhances curve segmentation by splitting at _tee parameters_.
- **Tee Parameters**:
  - Inflection points (where curvature changes sign)
  - Slopes of the pen (brush shape) that affect convolution
- **Input**: Path (knots) and brush shape (pen), both as cyclic lists of Bézier knots.
- **Output**: Alters the curve to include new points at detected tee values.
- **Use Case**: Increases visual fidelity and correctness during later convolution by ensuring strokes align with key curve characteristics.

**3\. convolve_all(knot_t \*knots, knot_t \*pen)**

- **Role**: Performs **geometric convolution** between the path and brush.
- **Input**:
  - A path (as cyclic Bézier knot list)
  - A brush shape (also as a cyclic Bézier knot list)
- **Output**: Writes all resulting stroke geometry into a **trace buffer**, from which Bézier segments can be generated (via show_segments).
- **Use Case**: Central to rendering—this is where the brush is "dragged" along the curve to generate its appearance.

\---------------------------------------------------------------------------------------------------------------

**Work flow**

1. **User creates a path** (e.g., using moveto, curveto, etc. exposed in common.h).
2. **Brush is defined** as a closed path (also using knot_t).
3. **split_at_tees** analyzes the path and modifies it at important inflection or slope changes.
4. **convolve_all** applies the brush geometry to the modified path using convolution.
5. **show_segments** converts the internal trace into renderable Bézier segments.

**Pros:**

- **Non-user API**: These are **internal routines** not meant for direct public use, but used by higher-level drawing commands.
- **Performance-sensitive**: Especially convolve_all() - convolution is computationally intensive and tightly coupled to the geometric shape of the brush.
- **Precision rendering**: By splitting at tee parameters and analyzing curve geometry, the system ensures high-quality and mathematically consistent brush strokes.