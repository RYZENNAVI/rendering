/*
 * @file user_examples.c
 * @authors Team 4 - Li
 * @date 18.07.2025
 *
 * Demo program showcasing the high-level drawing API exposed by common.h.
 * It validates twelve representative user scenarios:
 *   1. sharp      — polyline with a 90° sharp corner
 *   2. color      — same geometry but with a different color
 *   3. letter     — blue letter L (two perpendicular strokes)
 *   4. relative   — right triangle built via rlineto (relative coordinates)
 *   5. quadratic  — single quadratic Bézier curve
 *   6. transform  — right triangle with brush transformations (resize / rotate / translate / reflect / shear)
 *   7. misc       — miscellaneous APIs (`rmoveto`, `brush_tf`, `color_mixer`, `rasterize`)
 *   8. letters    — word TEAM drawn with simple strokes
 *   9. boolean    — boolean union / intersection of rectangles + bounding box
 *  10. spans      — manual span list rendering (pixel-art letter I)
 *  11. abcd       — coloured letters A B C D
 *  12. xspan      — blue & yellow X rendered via spans with blended overlap
 *
 * Build:
 *   cd examples && make
 *
 * Run:
 *   ./user_examples <scenario>
 *   where <scenario> = sharp | color | letter | relative | quadratic | transform | misc | letters | boolean | spans | abcd | xspan | all
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/common.h"
#include <stddef.h>

/* Forward declaration from color_mixing.c */
color_t *color_mixer(const color_t *color1, const color_t *color2, double mix);

/* Forward declare quadratic_curve_t */
typedef struct {
    point_t control;
    point_t end;
} quadratic_curve_t;
#include "../inc/image_png.h"

// Global image for rendering
static image_t *g_image = NULL;

// Initialize rendering system
static void init_rendering(int width, int height) {
    knots_init();
    g_image = create_image(width, height);
    if (!g_image) {
        fprintf(stderr, "Failed to create image\n");
        exit(1);
    }

    /* Fill background with opaque white instead of default transparent zeros */
    int total_pixels = width * height;
    for (int i = 0; i < total_pixels; ++i) {
        g_image->data[i * 4 + 0] = 255; /* R */
        g_image->data[i * 4 + 1] = 255; /* G */
        g_image->data[i * 4 + 2] = 255; /* B */
        g_image->data[i * 4 + 3] = 255; /* A */
    }
}

// Clean up rendering system
static void cleanup_rendering() {
    if (g_image) {
        free_image(g_image);
        g_image = NULL;
    }
}

// Helper function to render brush stroke to image
static void render_stroke_to_image(brush_stroke_t *stroke) {
    if (!stroke || !g_image) return;
    
    // Rasterize the stroke
    span_list_t *spans = rasterize(stroke, 1.0);
    if (spans) {
        render_spans_to_image(spans, g_image);
        span_list_free(spans);
    }
}

// Helper function to implement quadraticto using curveto
static knot_t *quadraticto(knot_t *path, quadratic_curve_t curve) {
    // Convert quadratic to cubic Bézier curve
    // For quadratic curve with control point C and end point E,
    // starting from current point P:
    // Cubic control points: C1 = P + 2/3 * (C - P), C2 = E + 2/3 * (C - E)
    
    knot_t *current = (knot_t *)path->list_node.pred;
    point_t start = {current->x, current->y};
    
    point_t c1 = {
        start.x + (2.0/3.0) * (curve.control.x - start.x),
        start.y + (2.0/3.0) * (curve.control.y - start.y)
    };
    
    point_t c2 = {
        curve.end.x + (2.0/3.0) * (curve.control.x - curve.end.x),
        curve.end.y + (2.0/3.0) * (curve.control.y - curve.end.y)
    };
    
    return curveto(path, c1, c2, curve.end);
}

/*
 * Scenario 1 — Sharp Corner
 * Uses common.h API: moveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 1: Sharp Corner ****************************/
static void example_sharp(void) {
    init_rendering(800, 600);

    /* Construct a closed polyline with a 90° sharp corner */
    point_t start = {100.0, 300.0};
    knot_t *path = moveto(start);
    path = lineto(path, (point_t){400.0, 300.0});
    path = lineto(path, (point_t){400.0, 100.0});
    path = pathclose(path);

    color_t red = {255, 0, 0, 255};
    knot_t *brush = brush_make_square();
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);

    brush_stroke_t *stroke = draw_shape(path, brush, red);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/sharp_corner.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 2 — Color Change
 * Uses common.h API: moveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 2: Color Change ****************************/
static void example_color(void) {
    init_rendering(800, 600);

    /* Same geometry as scenario 1 but color changed to green */
    point_t start = {100.0, 300.0};
    knot_t *path = moveto(start);
    path = lineto(path, (point_t){400.0, 300.0});
    path = lineto(path, (point_t){400.0, 100.0});
    path = pathclose(path);

    color_t green = {0, 200, 0, 255};
    knot_t *brush = brush_make_square();
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);

    brush_stroke_t *stroke = draw_shape(path, brush, green);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/color_change.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 3 — Letter L (Blue)
 * Uses common.h API: moveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 3: Letter L (Blue) ****************************/
static void example_letter(void) {
    init_rendering(800, 600);

    /* Vertical bar of letter L */
    knot_t *pathV = moveto((point_t){250.0, 150.0});
    pathV = lineto(pathV, (point_t){250.0, 450.0});

    color_t blue = {0, 0, 255, 255};
    knot_t *brushV = brush_make_square();
    brush_tf_resize(brushV, 8.0, Z);
    brush_stroke_t *strokeV = draw_shape(pathV, brushV, blue);
    render_stroke_to_image(strokeV);

    /* Horizontal base of letter L */
    knot_t *pathH = moveto((point_t){250.0, 450.0});
    pathH = lineto(pathH, (point_t){500.0, 450.0});

    knot_t *brushH = brush_make_square();
    brush_tf_resize(brushH, 8.0, Z);
    brush_stroke_t *strokeH = draw_shape(pathH, brushH, blue);
    render_stroke_to_image(strokeH);

    /* No green dot anymore */

    save_png(g_image, "output/letter_l.png");

    // Cleanup original resources
    brush_stroke_free(strokeV);
    brush_stroke_free(strokeH);
    knots_ring_free(brushV);
    knots_ring_free(brushH);
    cleanup_rendering();
}

/*
 * Scenario 4 — Relative Moves
 * Uses common.h API: moveto, rlineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 4: Relative Moves ****************************/
/* Relative moves: draws a right triangle using rlineto and pathclose */
static void example_relative(void) {
    init_rendering(800, 600);

    /* Start at (200,200), then move relatively forming an L-shape */
    point_t start = {200.0, 200.0};
    knot_t *path = moveto(start);
    path = rlineto(path, (point_t){150.0, 0.0});     /* relative horizontal line */
    path = rlineto(path, (point_t){0.0, 150.0});     /* relative vertical line */
    path = pathclose(path);

    /* Use a basic circular/rounded brush */
    color_t purple = {160, 32, 240, 255};
    knot_t *brush = brush_make_square();  // Using square brush as we don't have circular
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);

    brush_stroke_t *stroke = draw_shape(path, brush, purple);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/relative.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 5 — Quadratic Bézier
 * Uses common.h API: moveto, curveto (via quadraticto),
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 5: Quadratic Bézier ****************************/
static void example_quadratic(void) {
    init_rendering(800, 600);

    /* Draw a quadratic curve from (100,500) to (700,100) with control (400,600) */
    point_t start = {100.0, 500.0};
    quadratic_curve_t curve = {
        .control = {400.0, 600.0},
        .end     = {700.0, 100.0}
    };

    knot_t *path = moveto(start);
    path = quadraticto(path, curve);

    /* Green brush */
    color_t green = {0, 200, 0, 255};
    knot_t *brush = brush_make_square();
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);

    brush_stroke_t *stroke = draw_shape(path, brush, green);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/quadratic.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 6 — Brush Transformations
 * Uses common.h API: moveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   brush_tf_rotate, brush_tf_translate, brush_tf_reflect, brush_tf_shear,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 6: Brush Transformations ****************************/
static void example_transform(void) {
    init_rendering(800, 600);

    /* Basic right triangle path (3 vertices forming a right angle) */
    point_t start = {300.0, 250.0};
    knot_t *path = moveto(start);
    path = lineto(path, (point_t){500.0, 250.0});  /* horizontal line */
    path = lineto(path, (point_t){500.0, 450.0});  /* vertical line */
    path = pathclose(path);                        /* hypotenuse back to start */

    color_t orange = {255, 128, 0, 255};
    knot_t *brush = brush_make_square();
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);  // Thicker strokes

    /* Apply assorted transforms */
    brush_tf_rotate(brush, 30.0, Z);
    brush_tf_translate(brush, (point_t){-50.0, -50.0});
    brush_tf_reflect(brush, Y);
    brush_tf_shear(brush, 0.25, X);

    brush_stroke_t *stroke = draw_shape(path, brush, orange);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/transform.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 7 — Miscellaneous API
 * Uses common.h API: moveto, rmoveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize, brush_tf,
 *   color_mixer, draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 7: Miscellaneous API ****************************/
static void example_misc(void) {
    init_rendering(800, 600);

    /* Demonstrate rmoveto: start at (100,100) then move relatively by (50,50) */
    point_t first = {100.0, 100.0};
    knot_t *path = moveto(first);                /* A */
    path = rmoveto(path, (point_t){50.0, 50.0}); /* B (150,150) */
    path = lineto(path, (point_t){300.0, 150.0});/* C */
    path = lineto(path, first);                  /* back to A -> triangle */
    path = pathclose(path);

    color_t red = {255, 0, 0, 255};
    color_t blue = {0, 0, 255, 255};

    /* Mix colors using color_mixer (additive, 0.5) */
    color_t *purple_ptr = color_mixer(&red, &blue, 0.5);
    color_t purple = purple_ptr ? *purple_ptr : (color_t){128, 0, 128, 255};
    if (purple_ptr) free(purple_ptr);

    /* Create brush with mixed color */
    knot_t *brush = brush_make_square();
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);

    /* Apply a generic 3×3 identity transform using brush_tf */
    double identity[3][3] = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    brush_tf(brush, identity);

    brush_stroke_t *stroke = draw_shape(path, brush, purple);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/misc.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 8 — Draw Letters
 * Uses common.h API: moveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 8: Draw Letters ****************************/
static void example_letters(void) {
    /* Render the word "TEAM" using simple line paths */
    init_rendering(1000, 400);

    color_t black = {0, 0, 0, 255};
    knot_t *brush = brush_make_square();
    /* Thicken stroke */
    brush_tf_resize(brush, 4.0, Z);

    /********** Letter T **********/
    /* Top bar */
    knot_t *pathT1 = moveto((point_t){50.0, 100.0});
    pathT1 = lineto(pathT1, (point_t){130.0, 100.0});
    brush_stroke_t *strokeT1 = draw_shape(pathT1, brush, black);
    render_stroke_to_image(strokeT1);
    brush_stroke_free(strokeT1);

    /* Vertical stem */
    knot_t *pathT2 = moveto((point_t){90.0, 100.0});
    pathT2 = lineto(pathT2, (point_t){90.0, 250.0});
    brush_stroke_t *strokeT2 = draw_shape(pathT2, brush, black);
    render_stroke_to_image(strokeT2);
    brush_stroke_free(strokeT2);

    /********** Letter E **********/
    double ex = 200.0; // starting x offset for E
    /* Vertical spine */
    knot_t *pathE1 = moveto((point_t){ex, 100.0});
    pathE1 = lineto(pathE1, (point_t){ex, 250.0});
    brush_stroke_t *strokeE1 = draw_shape(pathE1, brush, black);
    render_stroke_to_image(strokeE1);
    brush_stroke_free(strokeE1);

    /* Top bar */
    knot_t *pathE2 = moveto((point_t){ex, 100.0});
    pathE2 = lineto(pathE2, (point_t){ex + 80.0, 100.0});
    brush_stroke_t *strokeE2 = draw_shape(pathE2, brush, black);
    render_stroke_to_image(strokeE2);
    brush_stroke_free(strokeE2);

    /* Middle bar */
    knot_t *pathE3 = moveto((point_t){ex, 175.0});
    pathE3 = lineto(pathE3, (point_t){ex + 60.0, 175.0});
    brush_stroke_t *strokeE3 = draw_shape(pathE3, brush, black);
    render_stroke_to_image(strokeE3);
    brush_stroke_free(strokeE3);

    /* Bottom bar */
    knot_t *pathE4 = moveto((point_t){ex, 250.0});
    pathE4 = lineto(pathE4, (point_t){ex + 80.0, 250.0});
    brush_stroke_t *strokeE4 = draw_shape(pathE4, brush, black);
    render_stroke_to_image(strokeE4);
    brush_stroke_free(strokeE4);

    /********** Letter A **********/
    double ax = 350.0; // starting x offset for A
    /* Left diagonal */
    knot_t *pathA1 = moveto((point_t){ax, 250.0});
    pathA1 = lineto(pathA1, (point_t){ax + 40.0, 100.0});
    brush_stroke_t *strokeA1 = draw_shape(pathA1, brush, black);
    render_stroke_to_image(strokeA1);
    brush_stroke_free(strokeA1);

    /* Right diagonal */
    knot_t *pathA2 = moveto((point_t){ax + 40.0, 100.0});
    pathA2 = lineto(pathA2, (point_t){ax + 80.0, 250.0});
    brush_stroke_t *strokeA2 = draw_shape(pathA2, brush, black);
    render_stroke_to_image(strokeA2);
    brush_stroke_free(strokeA2);

    /* Cross bar */
    knot_t *pathA3 = moveto((point_t){ax + 20.0, 175.0});
    pathA3 = lineto(pathA3, (point_t){ax + 60.0, 175.0});
    brush_stroke_t *strokeA3 = draw_shape(pathA3, brush, black);
    render_stroke_to_image(strokeA3);
    brush_stroke_free(strokeA3);

    /********** Letter M **********/
    double mx = 500.0; // starting x offset for M
    /* Left vertical */
    knot_t *pathM1 = moveto((point_t){mx, 250.0});
    pathM1 = lineto(pathM1, (point_t){mx, 100.0});
    brush_stroke_t *strokeM1 = draw_shape(pathM1, brush, black);
    render_stroke_to_image(strokeM1);
    brush_stroke_free(strokeM1);

    /* Left diagonal */
    knot_t *pathM2 = moveto((point_t){mx, 100.0});
    pathM2 = lineto(pathM2, (point_t){mx + 45.0, 175.0});
    brush_stroke_t *strokeM2 = draw_shape(pathM2, brush, black);
    render_stroke_to_image(strokeM2);
    brush_stroke_free(strokeM2);

    /* Right diagonal */
    knot_t *pathM3 = moveto((point_t){mx + 45.0, 175.0});
    pathM3 = lineto(pathM3, (point_t){mx + 90.0, 100.0});
    brush_stroke_t *strokeM3 = draw_shape(pathM3, brush, black);
    render_stroke_to_image(strokeM3);
    brush_stroke_free(strokeM3);

    /* Right vertical */
    knot_t *pathM4 = moveto((point_t){mx + 90.0, 100.0});
    pathM4 = lineto(pathM4, (point_t){mx + 90.0, 250.0});
    brush_stroke_t *strokeM4 = draw_shape(pathM4, brush, black);
    render_stroke_to_image(strokeM4);
    brush_stroke_free(strokeM4);

    /* Save output */
    save_png(g_image, "output/letters.png");

    /* Cleanup */
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 9  — Boolean Ops (Union & Intersection)
 * Uses common.h API: moveto, lineto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, brush_stroke_list_create/add/free,
 *   union_brush_stroke, intersection_brush_stroke,
 *   rasterize, render_spans_to_image,
 *   bounding_box, set_pixel, save_png, save_ppm,
 *   brush_stroke_free, span_list_free
 */
/**************************** Scenario 9: Boolean Ops (Union & Intersection) ****************************/
static void example_boolean(void) {
    init_rendering(800, 600);

    /* Build two overlapping rectangles */
    knot_t *path1 = moveto((point_t){200.0, 200.0});
    path1 = lineto(path1, (point_t){400.0, 200.0});
    path1 = lineto(path1, (point_t){400.0, 400.0});
    path1 = pathclose(path1);

    knot_t *path2 = moveto((point_t){300.0, 150.0});
    path2 = lineto(path2, (point_t){550.0, 150.0});
    path2 = lineto(path2, (point_t){550.0, 350.0});
    path2 = pathclose(path2);

    color_t red = {255, 0, 0, 255};
    color_t blue = {0, 0, 255, 180};
    knot_t *brush = brush_make_square();
    brush_tf_resize(brush, 4.0, Z);

    /* Individual strokes */
    brush_stroke_t *stroke1 = draw_shape(path1, brush, red);
    brush_stroke_t *stroke2 = draw_shape(path2, brush, blue);

    /* List and union / intersection */
    brush_stroke_list_t *list = brush_stroke_list_create(2);
    brush_stroke_list_add(list, stroke1);
    brush_stroke_list_add(list, stroke2);

    brush_stroke_t *u_stroke = union_brush_stroke(list);
    brush_stroke_t *i_stroke = intersection_brush_stroke(list);

    /* Rasterize union */
    span_list_t *spans_u = rasterize(u_stroke, 1.0);
    render_spans_to_image(spans_u, g_image);
    span_list_free(spans_u);

    /* Draw bounding box in green pixels */
    if (u_stroke) {
        bounding_box_t bb = bounding_box(u_stroke);
        color_t green = {0, 255, 0, 255};
        for (int x = (int)bb.min_x; x <= (int)bb.max_x; ++x) {
            set_pixel(g_image, x, (int)bb.min_y, green);
            set_pixel(g_image, x, (int)bb.max_y, green);
        }
        for (int y = (int)bb.min_y; y <= (int)bb.max_y; ++y) {
            set_pixel(g_image, (int)bb.min_x, y, green);
            set_pixel(g_image, (int)bb.max_x, y, green);
        }
    }

    save_png(g_image, "output/boolean_union.png");

    /* Optionally save PPM demonstrating save_ppm */
    save_ppm(g_image, "output/boolean_union.ppm");

    /* Cleanup */
    brush_stroke_free(stroke1);
    brush_stroke_free(stroke2);
    if (u_stroke) brush_stroke_free(u_stroke);
    if (i_stroke) brush_stroke_free(i_stroke);
    brush_stroke_list_free(list);
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 10 — Manual Spans
 * Uses common.h API: knots_init, create_image, span_list_create,
 *   span_list_add, render_spans_to_image, save_bmp,
 *   span_list_free, free_image, set_pixel (background)
 */
/**************************** Scenario 10: Manual Spans ****************************/
static void example_spans(void) {
    /* Demonstrates span_list_create / add / free & save_bmp */
    int W = 400, H = 200;
    knots_init();
    g_image = create_image(W, H);
    if (!g_image) return;
    /* white background */
    for (int i = 0; i < W * H; ++i) {
        g_image->data[i * 4 + 0] = 255;
        g_image->data[i * 4 + 1] = 255;
        g_image->data[i * 4 + 2] = 255;
        g_image->data[i * 4 + 3] = 255;
    }

    span_list_t *spans = span_list_create(100);
    color_t black = {0, 0, 0, 255};

    /* Build a simple pixel-art letter 'I' using horizontal spans */
    for (int y = 20; y < 40; ++y) {
        span_t s = {50, 150, y, black};
        span_list_add(spans, s);
    }
    for (int y = 40; y < 160; ++y) {
        span_t s = {90, 110, y, black};
        span_list_add(spans, s);
    }
    for (int y = 160; y < 180; ++y) {
        span_t s = {50, 150, y, black};
        span_list_add(spans, s);
    }

    render_spans_to_image(spans, g_image);

    save_bmp(g_image, "output/spans.bmp");

    span_list_free(spans);
    free_image(g_image);
    g_image = NULL;
}

/*
 * Scenario 11 — Alphabet ABCD
 * Uses common.h API: moveto, lineto, curveto, pathclose,
 *   brush_make_square, brush_tf_resize,
 *   draw_shape, rasterize, render_spans_to_image,
 *   save_png, brush_stroke_free, span_list_free
 */
/**************************** Scenario 11: Alphabet ABCD ****************************/
static void example_abcd(void) {
    init_rendering(1000, 400);
    knot_t *brush = brush_make_square();
    brush_tf_resize(brush, 4.0, Z);

    /* Letter A in Red with closed triangle and crossbar */
    color_t red = {255, 0, 0, 255};
    knot_t *pathA = moveto((point_t){80, 300});
    pathA = lineto(pathA, (point_t){120, 100});
    pathA = lineto(pathA, (point_t){160, 300});
    // pathclose removed to keep bottom open

    // Crossbar in the middle of A
    knot_t *barA = moveto((point_t){100, 200});
    barA = lineto(barA, (point_t){140, 200});

    brush_stroke_t *sA1 = draw_shape(pathA, brush, red);
    render_stroke_to_image(sA1);
    brush_stroke_free(sA1);

    brush_stroke_t *sA2 = draw_shape(barA, brush, red);
    render_stroke_to_image(sA2);
    brush_stroke_free(sA2);

    /* Letter B in Green with curved bulges */
    color_t green = {0, 200, 0, 255};
    // Vertical spine
    knot_t *spine = moveto((point_t){240.0, 100.0});
    spine = lineto(spine, (point_t){240.0, 300.0});
    brush_stroke_t *stroke_spine = draw_shape(spine, brush, green);
    render_stroke_to_image(stroke_spine);
    brush_stroke_free(stroke_spine);

    // Upper bulge (cubic curve)
    knot_t *top = moveto((point_t){240.0, 100.0});
    point_t top_c1 = {340.0, 100.0};
    point_t top_c2 = {340.0, 200.0};
    point_t top_end = {240.0, 200.0};
    top = curveto(top, top_c1, top_c2, top_end);
    brush_stroke_t *stroke_top = draw_shape(top, brush, green);
    render_stroke_to_image(stroke_top);
    brush_stroke_free(stroke_top);

    // Lower bulge (cubic curve)
    knot_t *bot = moveto((point_t){240.0, 200.0});
    point_t bot_c1 = {340.0, 200.0};
    point_t bot_c2 = {340.0, 300.0};
    point_t bot_end = {240.0, 300.0};
    bot = curveto(bot, bot_c1, bot_c2, bot_end);
    brush_stroke_t *stroke_bot = draw_shape(bot, brush, green);
    render_stroke_to_image(stroke_bot);
    brush_stroke_free(stroke_bot);

    /* Letter C in Blue (shifted right to avoid B) */
    color_t blue = {0, 0, 255, 255};
    knot_t *pathC = moveto((point_t){460, 120});
    pathC = lineto(pathC, (point_t){400, 120});
    pathC = lineto(pathC, (point_t){380, 180});
    pathC = lineto(pathC, (point_t){400, 260});
    pathC = lineto(pathC, (point_t){460, 260});
    brush_stroke_t *sC = draw_shape(pathC, brush, blue);
    render_stroke_to_image(sC);
    brush_stroke_free(sC);

    /* Letter D in Yellow (shifted right to follow C) */
    color_t yellow = {255, 255, 0, 255};
    knot_t *pathD = moveto((point_t){540, 100});
    pathD = lineto(pathD, (point_t){540, 300});
    pathD = lineto(pathD, (point_t){620, 260});
    pathD = lineto(pathD, (point_t){620, 140});
    pathD = pathclose(pathD);
    brush_stroke_t *sD = draw_shape(pathD, brush, yellow);
    render_stroke_to_image(sD);
    brush_stroke_free(sD);

    save_png(g_image, "output/abcd.png");
    knots_ring_free(brush);
    cleanup_rendering();
}

/*
 * Scenario 12 — Letter X via Spans
 * Uses common.h API: knots_init, create_image, span_list_create,
 *   span_list_add, render_spans_to_image, save_png,
 *   span_list_free, free_image, color_mixer
 */
/**************************** Scenario 12: Letter X via Spans ****************************/
static void example_xspan(void) {
    int W = 600, H = 600;
    knots_init();
    g_image = create_image(W, H);

    // Fill white background
    for (int i = 0; i < W * H; ++i) {
        g_image->data[i * 4 + 0] = 255;
        g_image->data[i * 4 + 1] = 255;
        g_image->data[i * 4 + 2] = 255;
        g_image->data[i * 4 + 3] = 255;
    }

    span_list_t *spans = span_list_create(2000);
    int thick = 10;

    color_t blue = {0, 0, 255, 255};
    color_t yellow = {255, 255, 0, 255};

    for (int y = 100; y <= 500; y++) {
        int x_blue = y;         // blue: /
        int x_yellow = W - y;   /* yellow: \\ diagonal */

        /* Add blue span */
        span_t sB = {x_blue - thick / 2, x_blue + thick / 2, y, blue};
        span_list_add(spans, sB);

        /* Add yellow span */
        span_t sY = {x_yellow - thick / 2, x_yellow + thick / 2, y, yellow};
        span_list_add(spans, sY);

        /* Check overlap exactly */
        int leftO = (x_blue - thick / 2) > (x_yellow - thick / 2) ? (x_blue - thick / 2) : (x_yellow - thick / 2);
        int rightO = (x_blue + thick / 2) < (x_yellow + thick / 2) ? (x_blue + thick / 2) : (x_yellow + thick / 2);
        if (leftO <= rightO) {
            /* Overlap range exists */
            color_t *mixed = color_mixer(&blue, &yellow, 0.5);
            color_t green = mixed ? *mixed : (color_t){0, 255, 0, 255};
            if (mixed) free(mixed);
            span_t sG = {leftO, rightO, y, green};
            span_list_add(spans, sG); /* Added last so it overrides */
        }
    }

    render_spans_to_image(spans, g_image);
    save_png(g_image, "output/xspan.png");
    span_list_free(spans);
    free_image(g_image);
    g_image = NULL;
}
/**************************** Main ****************************/
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s sharp|color|letter|relative|quadratic|transform|misc|letters|boolean|spans|abcd|xspan|all\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "sharp") == 0) {
        example_sharp();
    } else if (strcmp(argv[1], "color") == 0) {
        example_color();
    } else if (strcmp(argv[1], "letter") == 0) {
        example_letter();
    } else if (strcmp(argv[1], "relative") == 0) {
        example_relative();
    } else if (strcmp(argv[1], "quadratic") == 0) {
        example_quadratic();
    } else if (strcmp(argv[1], "transform") == 0) {
        example_transform();
    } else if (strcmp(argv[1], "misc") == 0) {
        example_misc();
    } else if (strcmp(argv[1], "boolean") == 0) {
        example_boolean();
    } else if (strcmp(argv[1], "spans") == 0) {
        example_spans();
    } else if (strcmp(argv[1], "abcd") == 0) {
        example_abcd();
    } else if (strcmp(argv[1], "xspan") == 0) {
        example_xspan();
    } else if (strcmp(argv[1], "letters") == 0) {
        example_letters();
    } else if (strcmp(argv[1], "all") == 0) {
        example_sharp();
        example_color();
        example_letter();
        example_relative();
        example_quadratic();
        example_transform();
        example_misc();
        example_boolean();
        example_spans();
        example_abcd();
        example_xspan();
        example_letters();
    } else {
        fprintf(stderr, "Unknown scenario '%s'\n", argv[1]);
        return 1;
    }
    return 0;
} 