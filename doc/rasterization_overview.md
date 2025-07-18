# Rasterization.c - High Level Overview

## Overview

The `rasterization.c` file implements optimized rasterization functions for brush strokes. It converts Bézier curves into span lists for efficient rendering operations.

## Main Functions and Call Hierarchy

### 1. Brush Stroke Rasterization

#### `rasterize(brush_stroke_t *stroke, double resolution)`

- **Purpose**: Converts a brush stroke into a list of spans
- **Parameters**:
  - `stroke`: The brush stroke to be rasterized
  - `resolution`: Resolution in pixels per unit
- **Returns**: `span_list_t*` - List of rasterized spans
- **Usage**: This is the main function for rasterization

**Call Sequence:**

```text
rasterize()
  ├── span_list_create()     // Creates empty span list
  ├── rasterize_bezier()     // For each Bézier curve in the stroke
  │   ├── bezier_x()         // Calculates X coordinate at parameter t
  │   ├── bezier_y()         // Calculates Y coordinate at parameter t
  │   └── add_span_if_valid() // Adds valid spans
  └── merge_spans()          // Merges overlapping spans
      └── compare_spans()    // Sorting function for spans
```

### 2. Boolean Operations on Brush Strokes

#### `union_brush_stroke(brush_stroke_list_t *strokes)`

- **Purpose**: Unites multiple brush strokes into a single one
- **Parameters**: `strokes` - List of brush strokes
- **Returns**: `brush_stroke_t*` - United brush stroke
- **Method**: Simple concatenation of all Bézier curves

**Call Sequence:**

```text
union_brush_stroke()
  ├── malloc()               // Memory for result stroke
  ├── malloc()               // Memory for Bézier array
  └── memcpy()               // Copies all Bézier curves
```

#### `intersection_brush_stroke(brush_stroke_list_t *strokes)`

- **Purpose**: Calculates the intersection of multiple brush strokes
- **Parameters**: `strokes` - List of brush strokes
- **Returns**: `brush_stroke_t*` - Intersection as new brush stroke
- **Method**: Direct geometric intersection using pairwise bounding box tests

**Call Sequence:**

```text
intersection_brush_stroke()
  ├── intersect_two_brush_strokes()  // Pairwise intersection (repeated)
  │   ├── bounding_box()             // For each stroke
  │   ├── bounding_boxes_intersect() // Quick intersection test
  │   ├── bezier_bounding_box()      // For each Bézier curve
  │   └── beziers_might_intersect()  // Conservative intersection test
  └── [Memory cleanup]               // Free intermediate results
```

### 3. Helper Functions

#### `bounding_box(brush_stroke_t *stroke)`

- **Purpose**: Calculates the bounding box of a brush stroke
- **Parameters**: `stroke` - The brush stroke
- **Returns**: `bounding_box_t` - Bounding rectangle
- **Method**: Iterates over all control points of all Bézier curves

#### `intersect_two_brush_strokes(brush_stroke_t *stroke1, brush_stroke_t *stroke2)`

- **Purpose**: Finds intersection between two brush strokes
- **Parameters**: Two brush strokes to intersect
- **Returns**: `brush_stroke_t*` - New brush stroke containing intersection
- **Method**: Uses bounding box tests to identify potentially intersecting Bézier curves

#### `bounding_boxes_intersect(bounding_box_t box1, bounding_box_t box2)`

- **Purpose**: Tests if two bounding boxes overlap
- **Parameters**: Two bounding boxes
- **Returns**: `bool` - True if boxes intersect
- **Method**: Fast geometric overlap test

#### `bezier_bounding_box(return_cubic_t *bezier)`

- **Purpose**: Calculates bounding box for a single Bézier curve
- **Parameters**: Pointer to Bézier curve
- **Returns**: `bounding_box_t` - Minimal bounding rectangle
- **Method**: Checks all four control points

#### `beziers_might_intersect(return_cubic_t *bezier1, return_cubic_t *bezier2)`

- **Purpose**: Conservative test if two Bézier curves might intersect
- **Parameters**: Two Bézier curves
- **Returns**: `bool` - True if intersection is possible
- **Method**: Compares bounding boxes of the curves

## Typical Usage Scenarios

### Scenario 1: Simple Rasterization

```c
1. brush_stroke_t *stroke = // ... create
2. span_list_t *spans = rasterize(stroke, 50.0)
3. // Use spans for rendering
4. span_list_free(spans)
```

### Scenario 2: Union of Strokes

```c
1. brush_stroke_list_t *strokes = // ... multiple strokes
2. brush_stroke_t *united = union_brush_stroke(strokes)
3. span_list_t *spans = rasterize(united, 50.0)
4. // Cleanup
```

### Scenario 3: Intersection of Strokes

```c
1. brush_stroke_list_t *strokes = // ... multiple strokes
2. brush_stroke_t *intersection = intersection_brush_stroke(strokes)
3. if (intersection && intersection->length > 0) {
4.     span_list_t *spans = rasterize(intersection, 50.0)
5.     // Use result
6.     span_list_free(spans)
7.     // Free intersection stroke
8.     free(intersection->beziers)
9.     free(intersection)
10. }
```

### Scenario 4: Bounding Box Calculation

```c
1. brush_stroke_t *stroke = // ... create
2. bounding_box_t bbox = bounding_box(stroke)
3. // Use bbox.min_x, bbox.max_x, etc.
```

## Important Implementation Details

### Resolution

- **Recommended value**: 50.0 pixels per unit
- **Impact**: Higher values = finer details, but more memory usage
- **Usage**: Use consistently in all rasterize() calls

### Memory Management

- All functions return dynamically allocated structures
- **Important**: Always call `span_list_free()` and corresponding cleanup functions
- NULL pointers are returned on errors
- **Intersection results**: May return empty strokes (length = 0) instead of NULL

### Performance Considerations

- `merge_spans()` optimizes span lists by merging overlapping areas
- Fixed tessellation with 100 steps per Bézier curve for consistent results
- Bresenham-like algorithm for efficient line rasterization
- **Intersection optimization**: Uses bounding box tests to avoid expensive calculations
- **Pairwise intersection**: O(n) complexity for n strokes instead of O(n²)

### Intersection Algorithm Details

- **Conservative approach**: Uses bounding box overlap as intersection indicator
- **No rasterization overhead**: Direct geometric operations on Bézier curves
- **Early termination**: Stops when intersection becomes empty
- **Memory efficient**: Processes strokes pairwise to minimize memory usage

## Algorithm Improvements (Version 2.0)

### Previous vs. Current Intersection Implementation

#### Old Approach (Span-based):
1. Rasterize each brush stroke to spans
2. Find overlapping spans across all lists
3. Convert intersection spans back to Bézier curves
4. **Disadvantages**: Expensive rasterization, loss of precision, complex span merging

#### New Approach (Direct Geometric):
1. Use pairwise intersection of brush strokes
2. Quick bounding box tests for efficiency
3. Keep original Bézier curves that might intersect
4. **Advantages**: No rasterization overhead, maintains precision, simpler logic

### Performance Benefits

- **Speed**: Eliminates rasterization step (major bottleneck)
- **Memory**: No intermediate span lists required
- **Scalability**: Linear complexity O(n) instead of quadratic
- **Precision**: Works directly with original curve data
- **Simplicity**: Clearer code structure and easier maintenance

## Dependencies

- Requires implementation of `span_list_*` functions
- Uses standard C libraries (`math.h`, `string.h`, `stdlib.h`)
- Based on data structures from `common.h`
