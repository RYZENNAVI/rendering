/*
 * @file user_examples.c
 * @authors Team 4 - Li
 * @date 09.07.2025
 *
 * Demo program showcasing the high-level drawing API exposed by common.h.
 * It validates seven representative user scenarios:
 *   1. sharp      — polyline with a 90° sharp corner
 *   2. color      — same geometry but with a different color
 *   3. overlap    — two groups of semi-transparent strokes that overlap
 *   4. relative   — polyline built via rlineto (relative coordinates)
 *   5. quadratic  — single quadratic Bézier curve
 *   6. transform  — right triangle with brush using resize / rotate / translate / reflect / shear
 *   7. misc       — miscellaneous APIs (`rmoveto`, `brush_tf`, `color_mixer`, `rasterize`)
 *
 * Build:
 *   cd examples && make
 *
 * Run:
 *   ./user_examples <scenario>
 *   where <scenario> = sharp | color | overlap | relative | quadratic | transform | misc | all
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../include/common.h"
#include "include/common_fixed.h"
#include "../include/image_png.h"

// Global image for rendering
static image_t *g_image = NULL;

// Helper function to reduce angle to [0, 2π)
static double reduce_angle(double angle) {
    while (angle < 0) angle += 2.0 * M_PI;
    while (angle >= 2.0 * M_PI) angle -= 2.0 * M_PI;
    return angle;
}

// Initialize rendering system
static void init_rendering(int width, int height) {
    knots_init();
    /* Ensure output directory exists */
    struct stat st = {0};
    if (stat("output", &st) == -1) {
        mkdir("output", 0755);
    }
    g_image = create_image(width, height);
    if (!g_image) {
        fprintf(stderr, "Failed to create image\n");
        exit(1);
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

// Quadratic curve definition for compatibility
typedef struct {
    point_t control;
    point_t end;
} quadratic_curve_t;

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

    brush_stroke_t *stroke = draw_shape(path, brush, red);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/sharp_corner.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    cleanup_rendering();
}

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

    brush_stroke_t *stroke = draw_shape(path, brush, green);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/color_change.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    cleanup_rendering();
}

/**************************** Scenario 3: Overlapping Strokes ****************************/
static void example_overlap(void) {
    init_rendering(800, 600);

    /* First group: Red horizontal + Blue vertical (intersection = purple) */
    
    /* First semi-transparent red horizontal stroke */
    knot_t *path1 = moveto((point_t){100.0, 200.0});
    path1 = lineto(path1, (point_t){350.0, 200.0});

    color_t red = {255, 0, 0, 180};   /* alpha < 255 to enable blending */
    knot_t *brush1 = brush_make_square();
    brush_stroke_t *stroke1 = draw_shape(path1, brush1, red);
    render_stroke_to_image(stroke1);

    /* Second semi-transparent blue vertical stroke crossing the red one */
    knot_t *path2 = moveto((point_t){225.0, 80.0});
    path2 = lineto(path2, (point_t){225.0, 320.0});

    color_t blue = {0, 0, 255, 180};
    knot_t *brush2 = brush_make_square();
    brush_stroke_t *stroke2 = draw_shape(path2, brush2, blue);
    render_stroke_to_image(stroke2);

    /* Second group: Blue horizontal + Yellow vertical (intersection = green) */
    
    /* Third semi-transparent blue horizontal stroke */
    knot_t *path3 = moveto((point_t){450.0, 200.0});
    path3 = lineto(path3, (point_t){700.0, 200.0});

    color_t blue2 = {0, 0, 255, 180};
    knot_t *brush3 = brush_make_square();
    brush_stroke_t *stroke3 = draw_shape(path3, brush3, blue2);
    render_stroke_to_image(stroke3);

    /* Fourth semi-transparent yellow vertical stroke crossing the blue one */
    knot_t *path4 = moveto((point_t){575.0, 80.0});
    path4 = lineto(path4, (point_t){575.0, 320.0});

    color_t yellow = {255, 255, 0, 180};
    knot_t *brush4 = brush_make_square();
    brush_stroke_t *stroke4 = draw_shape(path4, brush4, yellow);
    render_stroke_to_image(stroke4);

    save_png(g_image, "output/overlap.png");
    
    // Cleanup
    brush_stroke_free(stroke1);
    brush_stroke_free(stroke2);
    brush_stroke_free(stroke3);
    brush_stroke_free(stroke4);
    cleanup_rendering();
}

/**************************** Scenario 4: Relative Moves ****************************/
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

    brush_stroke_t *stroke = draw_shape(path, brush, purple);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/relative.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    cleanup_rendering();
}

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

    brush_stroke_t *stroke = draw_shape(path, brush, green);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/quadratic.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    cleanup_rendering();
}

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

    /* Apply assorted transforms */
    brush_tf_resize(brush, 1.5, Z);  // Scale uniformly
    brush_tf_rotate(brush, 30.0, Z);
    brush_tf_translate(brush, (point_t){-50.0, -50.0});
    brush_tf_reflect(brush, Y);
    brush_tf_shear(brush, 0.25, X);

    brush_stroke_t *stroke = draw_shape(path, brush, orange);
    render_stroke_to_image(stroke);

    save_png(g_image, "output/transform.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    cleanup_rendering();
}

/**************************** Scenario 7: Miscellaneous API ****************************/
static void example_misc(void) {
    init_rendering(800, 600);

    /* Demonstrate rmoveto: start at (100,100) then move relatively by (50,50) */
    knot_t *path = moveto((point_t){100.0, 100.0});
    path = rmoveto(path, (point_t){50.0, 50.0});           /* relative move to (150,150) */
    path = lineto(path, (point_t){300.0, 150.0});
    path = pathclose(path);

    color_t red = {255, 0, 0, 255};
    color_t blue = {0, 0, 255, 255};

    /* Mix colors using color_mixer (additive, 0.5) */
    color_t *purple_ptr = color_mixer(&red, &blue, 0.5);
    color_t purple = purple_ptr ? *purple_ptr : (color_t){128, 0, 128, 255};
    if (purple_ptr) free(purple_ptr);

    /* Create brush with mixed color */
    knot_t *brush = brush_make_square();

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
    cleanup_rendering();
}

/**************************** Main ****************************/
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s sharp|color|overlap|relative|quadratic|transform|misc|all\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "sharp") == 0) {
        example_sharp();
    } else if (strcmp(argv[1], "color") == 0) {
        example_color();
    } else if (strcmp(argv[1], "overlap") == 0) {
        example_overlap();
    } else if (strcmp(argv[1], "relative") == 0) {
        example_relative();
    } else if (strcmp(argv[1], "quadratic") == 0) {
        example_quadratic();
    } else if (strcmp(argv[1], "transform") == 0) {
        example_transform();
    } else if (strcmp(argv[1], "misc") == 0) {
        example_misc();
    } else if (strcmp(argv[1], "all") == 0) {
        example_sharp();
        example_color();
        example_overlap();
        example_relative();
        example_quadratic();
        example_transform();
        example_misc();
    } else {
        fprintf(stderr, "Unknown scenario '%s'\n", argv[1]);
        return 1;
    }
    return 0;
} 