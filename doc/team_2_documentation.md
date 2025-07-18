# Team 2 Function Documentation

## Overview
Team 2 is responsible for **Rasterization** in the rendering pipeline. This module converts vector-based brush strokes (defined by Bézier curves) into pixel-based representations (spans) that can be rendered to images.

## Table of Contents
1. [Core Rasterization Functions](#core-rasterization-functions)
2. [Span List Management](#span-list-management)
3. [Brush Stroke Operations](#brush-stroke-operations)
4. [Image Output Functions](#image-output-functions)
5. [Data Structures](#data-structures)

---

## Core Rasterization Functions

### `rasterize()`
**Purpose**: Main rasterization function that converts a brush stroke into a list of spans.

**Signature**: 
```c
span_list_t *rasterize(brush_stroke_t *stroke, double resolution);
```

**Preconditions**:
- `stroke != NULL`
- `stroke->beziers != NULL` if `stroke->length > 0`
- `resolution > 0.0`
- `stroke->length` represents valid number of Bézier curves
- All Bézier curves in stroke have valid coordinates (no NaN, no infinity)

**Invariants**:
- Memory allocation is consistent (no memory leaks)
- Span generation maintains geometric accuracy
- Color information is preserved throughout processing
- Resolution scaling is applied uniformly to all curves

**Postconditions**:
- Returns valid `span_list_t*` pointer or `NULL` on failure
- Generated spans cover all pixels touched by the stroke
- Spans are sorted by y-coordinate, then by x-coordinate
- Overlapping spans on same y-line are merged
- All spans contain the stroke's color information

---

### `union_brush_stroke()`
**Purpose**: Combines multiple brush strokes into a single brush stroke.

**Signature**: 
```c
brush_stroke_t *union_brush_stroke(brush_stroke_list_t *strokes);
```

**Preconditions**:
- `strokes != NULL`
- `strokes->count > 0`
- All strokes in list are valid (non-NULL)
- Each stroke has valid Bézier curve data

**Invariants**:
- Total number of Bézier curves = sum of all input strokes
- Memory is allocated correctly for combined stroke
- No Bézier curves are lost or duplicated
- First stroke's color is used as result color

**Postconditions**:
- Returns new `brush_stroke_t*` with all input curves combined
- Result length equals sum of all input stroke lengths
- All Bézier curves from input strokes are preserved
- Memory is properly allocated and must be freed by caller

---

### `intersection_brush_stroke()`
**Purpose**: Computes geometric intersection of multiple brush strokes using pairwise bounding box tests.

**Signature**: 
```c
brush_stroke_t *intersection_brush_stroke(brush_stroke_list_t *strokes);
```

**Preconditions**:
- `strokes != NULL`
- `strokes->count > 0`
- All strokes in list are valid (non-NULL)
- Each stroke has valid Bézier curve data and coordinates

**Invariants**:
- Uses pairwise intersection algorithm with O(n) complexity
- Bounding box tests provide conservative intersection estimates
- Memory allocation follows doubling strategy during processing
- Original input strokes remain unmodified
- Color from first stroke is preserved in result

**Postconditions**:
- Returns new `brush_stroke_t*` containing intersection geometry
- Result may be empty (length = 0) if no intersection exists
- Only Bézier curves with overlapping bounding boxes are included
- Memory is properly allocated and must be freed by caller
- NULL returned only on allocation failure, not empty intersection

---

### `bounding_box()`
**Purpose**: Calculates the bounding box of a brush stroke.

**Signature**: 
```c
bounding_box_t bounding_box(brush_stroke_t *stroke);
```

**Preconditions**:
- `stroke` may be NULL (returns zero-initialized bbox)
- If `stroke != NULL`, then `stroke->beziers != NULL` when `stroke->length > 0`
- All control points have valid coordinates

**Invariants**:
- Bounding box calculation considers all control points
- Min/max values are updated correctly during iteration
- Coordinate system consistency maintained

**Postconditions**:
- Returns `bounding_box_t` with correct min/max coordinates
- If stroke is NULL or empty, returns zero-initialized bounding box
- Bounding box encompasses all control points of all Bézier curves
- `min_x ≤ max_x` and `min_y ≤ max_y` (unless empty)

---

## Span List Management

### `span_list_create()`
**Purpose**: Creates a new span list with specified initial capacity.

**Signature**: 
```c
span_list_t *span_list_create(size_t initial_capacity);
```

**Preconditions**:
- `initial_capacity > 0`
- Sufficient memory available for allocation

**Invariants**:
- `count` always ≤ `capacity`
- `spans` pointer is valid when `capacity > 0`
- Memory allocation is consistent

**Postconditions**:
- Returns valid `span_list_t*` or `NULL` on failure
- `count = 0`, `capacity = initial_capacity`
- Memory for spans array is allocated
- List is ready to accept span additions

---

### `span_list_add()`
**Purpose**: Adds a span to the list, expanding capacity if needed.

**Signature**: 
```c
void span_list_add(span_list_t *list, span_t span);
```

**Preconditions**:
- `list != NULL`
- `list->spans != NULL` 
- Span contains valid coordinates and color data

**Invariants**:
- Capacity expansion follows doubling strategy
- Existing spans are preserved during reallocation
- `count` is incremented only on successful addition
- Memory consistency maintained

**Postconditions**:
- Span is added to list (if memory allows)
- `list->count` incremented by 1 on success
- Capacity may be doubled if expansion was needed
- Order of existing spans is preserved

---

### `span_list_free()`
**Purpose**: Frees memory allocated for a span list.

**Signature**: 
```c
void span_list_free(span_list_t *list);
```

**Preconditions**:
- `list` may be NULL (function handles gracefully)
- If `list != NULL`, it must be validly allocated

**Invariants**:
- All allocated memory is freed
- No memory leaks occur
- Function is safe to call multiple times

**Postconditions**:
- All memory associated with list is freed
- `list` pointer becomes invalid after call
- Function handles NULL input gracefully

---

## Brush Stroke Operations

### `brush_stroke_list_create()`
**Purpose**: Creates a new brush stroke list with specified capacity.

**Signature**: 
```c
brush_stroke_list_t *brush_stroke_list_create(size_t initial_capacity);
```

**Preconditions**:
- `initial_capacity > 0`
- Sufficient memory available

**Invariants**:
- `count ≤ capacity` always maintained
- Memory allocation follows standard patterns
- Pointer array is properly allocated

**Postconditions**:
- Returns valid list pointer or NULL on failure
- List initialized with zero count
- Ready to accept brush stroke pointers

---

### `brush_stroke_list_add()`
**Purpose**: Adds a brush stroke pointer to the list.

**Signature**: 
```c
void brush_stroke_list_add(brush_stroke_list_t *list, brush_stroke_t *stroke);
```

**Preconditions**:
- `list != NULL`
- `stroke != NULL`
- List has valid internal structure

**Invariants**:
- Only pointers are stored (no deep copying)
- Capacity expansion follows doubling strategy
- Existing pointers preserved during reallocation

**Postconditions**:
- Stroke pointer added to list
- Count incremented on success
- Capacity may increase if needed

---

### `brush_stroke_list_free()`
**Purpose**: Frees brush stroke list (not individual strokes).

**Signature**: 
```c
void brush_stroke_list_free(brush_stroke_list_t *list);
```

**Preconditions**:
- `list` may be NULL

**Invariants**:
- Only list structure is freed
- Individual brush strokes remain valid
- No memory leaks in list management

**Postconditions**:
- List structure memory freed
- Individual brush strokes unchanged
- Caller retains responsibility for stroke memory

---

### `brush_stroke_free()`
**Purpose**: Frees memory allocated for a brush stroke.

**Signature**: 
```c
void brush_stroke_free(brush_stroke_t *stroke);
```

**Preconditions**:
- `stroke` may be NULL

**Invariants**:
- All components of stroke are freed
- Bézier curve array is properly deallocated
- Function handles NULL gracefully

**Postconditions**:
- All memory associated with stroke is freed
- Bézier curves array and stroke structure deallocated
- Safe to call with NULL pointer

---

## Image Output Functions

### `create_image()`
**Purpose**: Creates a new image with specified dimensions.

**Signature**: 
```c
image_t *create_image(int width, int height);
```

**Preconditions**:
- `width > 0`
- `height > 0`
- Sufficient memory for image data (width × height × 4 bytes)

**Invariants**:
- Image data uses RGBA format (4 bytes per pixel)
- Memory allocation is contiguous
- Image initialized to transparent black

**Postconditions**:
- Returns valid `image_t*` or NULL on failure
- Image data allocated and initialized to zeros
- Image ready for pixel operations

---

### `set_pixel()`
**Purpose**: Sets a pixel color at specified coordinates.

**Signature**: 
```c
void set_pixel(image_t *img, int x, int y, color_t color);
```

**Preconditions**:
- `img != NULL`
- `img->data != NULL`
- `0 ≤ x < img->width`
- `0 ≤ y < img->height`

**Invariants**:
- Pixel data stored in RGBA format
- Array bounds checking prevents buffer overruns
- Color components stored in correct order

**Postconditions**:
- Pixel at (x,y) set to specified color
- Out-of-bounds requests are ignored safely
- Image data integrity maintained

---

### `save_bmp()`
**Purpose**: Saves image data to BMP file format.

**Signature**: 
```c
void save_bmp(image_t *img, const char *filename);
```

**Preconditions**:
- `img != NULL`
- `img->data != NULL`
- `filename != NULL`
- Valid file system path
- Write permissions for target location

**Invariants**:
- BMP format specifications followed
- File I/O handled with proper error checking
- Image data converted to BMP RGB format
- Endianness handled correctly

**Postconditions**:
- BMP file created with image data
- File contains valid BMP header and pixel data
- Function handles errors gracefully
- Original image data unchanged

---

### `render_spans_to_image()`
**Purpose**: Renders a list of spans to an image.

**Signature**: 
```c
void render_spans_to_image(span_list_t *spans, image_t *img);
```

**Preconditions**:
- `spans != NULL`
- `img != NULL`
- `img->data != NULL`
- All spans contain valid coordinates and colors

**Invariants**:
- Spans are processed in order
- Only pixels within image bounds are modified
- Color blending or overwriting follows consistent rules
- Span coordinates are properly clipped

**Postconditions**:
- Image contains rendered spans
- Pixels within span ranges set to span colors
- Out-of-bounds spans handled safely
- Image ready for output or further processing

---

### `free_image()`
**Purpose**: Frees memory allocated for an image.

**Signature**: 
```c
void free_image(image_t *img);
```

**Preconditions**:
- `img` may be NULL

**Invariants**:
- Both image structure and data are freed
- Function handles partial initialization
- No memory leaks occur

**Postconditions**:
- All image memory deallocated
- Function safe to call with NULL
- Image pointer becomes invalid after call

---

## Data Structures

### `span_t`
Represents a horizontal line segment for rasterization:
```c
typedef struct {
    int x_start;   /* Starting x coordinate */
    int x_end;     /* Ending x coordinate */
    int y;         /* Y coordinate of the span */
    color_t color; /* Color of the span */
} span_t;
```

### `span_list_t`
Dynamic array of spans with capacity management:
```c
typedef struct {
    span_t *spans;
    size_t count;
    size_t capacity;
} span_list_t;
```

### `image_t`
Image representation with RGBA pixel data:
```c
typedef struct {
    uint8_t *data;
    int width;
    int height;
} image_t;
```

### `bounding_box_t`
2D bounding box representation:
```c
typedef struct {
    double min_x;
    double min_y;
    double max_x;
    double max_y;
} bounding_box_t;
```

---

## Implementation Notes

### Performance Characteristics
- **Rasterization**: O(n·k) where n = number of curves, k = tessellation steps
- **Span Merging**: O(m log m) where m = number of spans
- **Union Operation**: O(n) where n = total number of curves
- **Intersection Operation**: O(n·m) where n = curves in stroke1, m = curves in stroke2 (pairwise)
- **Bounding Box Tests**: O(1) per comparison
- **Memory Management**: Dynamic allocation with capacity doubling

### Memory Management
- All functions follow consistent allocation/deallocation patterns
- Caller responsible for freeing returned pointers
- Functions handle NULL inputs gracefully
- No global state dependencies

### Coordinate System
- Image coordinates: (0,0) at top-left
- Span coordinates: integer pixel positions
- Bézier coordinates: floating-point world coordinates
- Resolution parameter scales world to pixel coordinates

### Error Handling
- Functions return NULL on allocation failures
- Bounds checking prevents buffer overruns
- Invalid parameters handled gracefully
- No undefined behavior on edge cases

---

### `bounding_boxes_intersect()` (Internal Helper)
**Purpose**: Fast geometric test for bounding box overlap.

**Signature**: 
```c
static bool bounding_boxes_intersect(bounding_box_t box1, bounding_box_t box2);
```

**Preconditions**:
- Both bounding boxes contain valid coordinate ranges
- `min_x ≤ max_x` and `min_y ≤ max_y` for both boxes

**Invariants**:
- Uses standard 2D rectangle intersection algorithm
- No side effects or memory allocation
- Symmetric operation (order of boxes doesn't matter)

**Postconditions**:
- Returns true if boxes overlap or touch
- Returns false if boxes are completely separate
- Handles edge cases (touching boundaries count as intersection)

---

### `bezier_bounding_box()` (Internal Helper)
**Purpose**: Calculates tight bounding box for a single Bézier curve.

**Signature**: 
```c
static bounding_box_t bezier_bounding_box(return_cubic_t *bezier);
```

**Preconditions**:
- `bezier != NULL`
- All control points (start, c1, c2, end) have valid coordinates
- No NaN or infinity values in coordinates

**Invariants**:
- Examines all four control points of cubic Bézier
- Conservative bounding box (may be larger than actual curve bounds)
- Fast computation using only control point coordinates

**Postconditions**:
- Returns bounding box that completely contains the curve
- `min_x ≤ max_x` and `min_y ≤ max_y` guaranteed
- Box may extend beyond actual curve due to control point positions

---

### `beziers_might_intersect()` (Internal Helper)
**Purpose**: Conservative test for potential intersection between two Bézier curves.

**Signature**: 
```c
static bool beziers_might_intersect(return_cubic_t *bezier1, return_cubic_t *bezier2);
```

**Preconditions**:
- `bezier1 != NULL` and `bezier2 != NULL`
- Both curves have valid control point coordinates

**Invariants**:
- Uses bounding box comparison for fast screening
- Conservative approach (no false negatives, may have false positives)
- No geometric curve intersection calculation performed

**Postconditions**:
- Returns true if curves might intersect (bounding boxes overlap)
- Returns false only if curves definitely cannot intersect
- Used for efficient filtering in intersection algorithms

---

### Intersection Algorithm Details
- **Approach**: Direct geometric intersection without rasterization
- **Strategy**: Pairwise comparison using bounding box tests
- **Conservative**: May include curves that don't actually intersect (false positives)
- **Performance**: Avoids expensive rasterization and de-rasterization steps
- **Memory Efficiency**: Processes strokes pairwise to minimize peak memory usage
- **Early Termination**: Stops processing if intersection becomes empty
