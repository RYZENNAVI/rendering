# Examples – Rendering API Showcase
# Author - Li & Daniel
**Location:** `sose2025_rendering/examples`

This directory contains a compact demo driver (`user_examples.c`) for the **Team-1 Rendering Library**.  After building it you obtain the executable `user_examples`, which renders **ten** representative scenes and stores their outputs in `output/`.

---

## Directory contents

| File | Purpose |
|------|---------|
| `user_examples.c` | Main demo with 10 scenarios |
| `Makefile`        | Build script – compiles library sources directly |
| `output/`         | Rendered PNG files go here (created automatically) |

> Legacy items mentioned in older docs – `adapter.*`, `simple_image.c`, PPM output, etc. – were removed.  The project now relies solely on the real `common.h` API.

---

## Build & run

```bash
cd sose2025_rendering/examples
make                # build `user_examples` plus build/* objects
```

Run a single scenario for quick testing:
```bash
./user_examples sharp        # or color / overlap / relative / quadratic / transform / misc / letters / boolean / spans
```
Run all scenarios:
```bash
./user_examples all
```
PNG files are written to `output/` with an opaque white background.

Clean up:
```bash
make clean          # remove build/ , executable and output/*.png
```

---

## macOS: Fixing the `png.h` not found error

If you're on **macOS** and get this error during `make`:

```
fatal error: 'png.h' file not found
```
It means the compiler can't find the Homebrew-installed PNG headers.

### Solution

1. **Install `libpng`:**
```bash
brew install libpng
```
2. **Build with the correct flags:**
```bash
make CFLAGS="-I/opt/homebrew/opt/libpng/include" \
     LDFLAGS="-L/opt/homebrew/opt/libpng/lib -lpng"
```
3. **(Optional) If you get missing zlib errors, also install and link zlib:**
```bash
brew install zlib
make CFLAGS="-I/opt/homebrew/opt/libpng/include -I/opt/homebrew/opt/zlib/include" \
     LDFLAGS="-L/opt/homebrew/opt/libpng/lib -L/opt/homebrew/opt/zlib/lib -lpng -lz"
```
4. **(Optional) Permanent fix: Modify your Makefile like so:**
```bash
CFLAGS += -I/opt/homebrew/opt/libpng/include
LDFLAGS += -L/opt/homebrew/opt/libpng/lib -lpng
```

---

## The twelve scenarios

| # | Name | Output | Highlights |
|---|------|--------|------------|
| 1 | `sharp`      | `sharp_corner.png`   | Closed poly-line with a 90 ° corner |
| 2 | `color`      | `color_change.png`   | Same geometry, different colour |
| 3 | `letter`     | `letter_l.png`       | Blue letter L |
| 4 | `relative`   | `relative.png`       | Right triangle drawn via relative commands |
| 5 | `quadratic`  | `quadratic.png`      | Quadratic Bézier (converted to cubic) |
| 6 | `transform`  | `transform.png`      | Brush resize / rotate / translate / reflect / shear |
| 7 | `misc`       | `misc.png`           | Purple triangle via rmoveto, brush_tf, color_mixer, … |
| 8 | `letters`    | `letters.png`        | Word **TEAM** drawn with simple strokes |
| 9 | `boolean`    | `boolean_union.png`  | Demonstrates union / intersection, bounding box, PPM output |
|10 | `spans`      | `spans.bmp`          | Manual span list rendering & BMP output |
|11 | `abcd`       | `abcd.png`           | Colored letters A B C D |
|12 | `xspan`      | `xspan.png`          | Blue & yellow solid X – overlap appears gray (RGB average) |

### API usage by scenario (high-level)

| Scenario | Key common.h functions exercised |
|----------|-----------------------------------|
| sharp      | moveto, lineto, pathclose, brush_make_square, brush_tf_resize, draw_shape |
| color      | moveto, lineto, pathclose, brush_make_square, brush_tf_resize, draw_shape |
| letter     | moveto, lineto, pathclose, brush_make_square, brush_tf_resize, draw_shape |
| relative   | moveto, rlineto, pathclose, brush_make_square, draw_shape |
| quadratic  | moveto, curveto (via helper), brush_make_square, draw_shape |
| transform  | moveto, lineto, pathclose, brush_make_square, brush_tf_resize/rotate/translate/reflect/shear, draw_shape |
| misc       | moveto, rmoveto, lineto, pathclose, brush_make_square, brush_tf, color_mixer, draw_shape |
| letters    | moveto, lineto, brush_make_square, brush_tf_resize, draw_shape, save_png |
| boolean    | moveto, lineto, pathclose, brush_make_square, brush_tf_resize, draw_shape, brush_stroke_list_*, union_brush_stroke, intersection_brush_stroke, rasterize, bounding_box, set_pixel |
| spans      | knots_init, create_image, span_list_create/add, render_spans_to_image, save_bmp |
| abcd       | moveto, lineto, pathclose, brush_make_square, brush_tf_resize, draw_shape, save_png |
| xspan      | knots_init, create_image, span_list_create/add, color_mixer, render_spans_to_image, save_png |

---

## API coverage

`user_examples.c` calls functions declared in `inc/common.h`:

* **Path construction** – `moveto`, `rmoveto`, `lineto`, `rlineto`, `curveto`, `pathclose`
* **Brush handling** – `brush_make_square` and the whole `brush_tf_*` family
* **Rendering pipeline** – `draw_shape`, `rasterize`, `render_spans_to_image`, `save_png`
* **Colour utilities** – `color_mixer`
* **Helpers** – `knots_init`, `brush_stroke_free`, …

All functionality now relies directly on the upstream library sources; no local patching or helper C files are required.

---

## Design notes

* **White background** – `init_rendering()` fills the RGBA buffer with `255,255,255,255`, so exported PNGs are opaque.
* **Rasterisation quality** – the library samples each cubic Bézier at 100 steps; you can pass a higher `resolution` to `rasterize()` for smoother lines.
* **Output format** – only PNG is written; PPM support can be re-enabled by calling `save_ppm()` alongside `save_png()` if required.

---

## Coming next

1. Adaptive Bézier tesselation / supersampling for crisper lines.
2. Golden-image regression tests in CI – compare hashes of generated PNGs.

