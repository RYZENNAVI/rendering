# Team 3 function Documentation

## Overview
Team 3 is responsible for the final output of rasterized images, including:
- Filling raster spans with color
- Creating and managing RGBA image buffers
- Saving final images to PNG format using libpng

**Note:** The image buffer structure `image_t` and basic pixel operations like `set_pixel_rgba` are provided by **Team 2**. The RGBA color structure (`color_t`) used in spans and pixels is defined by **Team 4**. 
Team 3 builds on this infrastructure and focuses on rendering spans and exporting images as PNGs.

Our module is called after rasterization and performs scanline rendering using span_t data provided by Team 2.

## Table of Contents
1. [Scanline Fill Function](#scanline-fill-function)
2. [PNG Export Function](#png-export-function)
3. [Test Program](#test-program)
4. [Data Structures](#data-structures)
5. [Implementation Notes](#implementation-notes)

---

## Scanline Fill Function

### `fill_polygon_scanline()`

**Purpose**: Fills a closed polygon into an image using scanline rasterization.
**Signature**:
```c
void fill_polygon_scanline(point_t *pts, int n_pts, image_t *img, color_t fill);;
```

**Preconditions**:
- `pts != NULL`
- `n_pts >= 3` (polygon must have ≥ 3 points)
- `img != NULL` 
- `img->data != NULL`

**Invariants**:
- Polygon is closed: first and last point must connect
- Scanlines iterate top-down (y-axis increasing)
- x-intersections per scanline are sorted before filling
- Only pixels inside polygon are filled

**Postconditions**:
- The filled polygon appears in the image
- Only valid pixels in the image buffer are modified

---

## PNG Export Function

### `save_png()`

**Purpose**: Writes the RGBA image buffer to a .png file using libpng.

**Signature**:
```c
void save_png(image_t *img, const char *filename);
```

**Preconditions**:
- `img != NULL`
- `img->data != NULL`
- `filename != NULL`

**Invariants**:
- Format is 8-bit RGBA per pixel
- `libpng` s correctly initialized and destroyed
- File is opened and closed safely
- Rows are assigned correctly from image buffer

**Postconditions**:
- A valid `.png` file is written to disk
- Resources are freed properly (row pointers, file handle, PNG structs)

---

## Test Program

### `test_png_export.c`

**Purpose**: Demonstrates how to generate an image and export it as a PNG.

**Signature**:
```bash
./test_png_export
```

**Preconditions**:
- No external input needed (test is hardcoded)
- libpng must be installed

**Postconditions**:
- The image is saved as `test_out.png` in the current directory
---

## Data Structures

`image_t` (Provided by Team 2)
```c
typedef struct {
    uint8_t *data;  // Pointer to RGBA buffer (4 bytes per pixel)
    int width;
    int height;
} image_t;
```

`point_t`
```c
typedef struct {
    int x;
    int y;
} point_t;
```

`color_t` (Provided by Team 4)
```c
typedef struct {
    uint8_t r, g, b, a;
} color_t;
```

---

## Implementation Notes

- Uses libpng for PNG encoding (`-lpng` required)
- Fill algorithm works for convex and simple concave polygons
- Output image origin is top-left (0,0) and uses row-major memory layout
- Install libpng on Debian/Ubuntu:
```bash
sudo apt update
sudo apt install libpng-dev
```
This will install the required development headers and libraries for linking against `libpng`