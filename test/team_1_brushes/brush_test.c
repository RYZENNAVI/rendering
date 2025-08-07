/*
@file brush_test.c
@authors Matthias, Toni
@date 10.07.25
*/


#include "common.h"

void print_brush_stroke(const brush_stroke_t *bs)
{
  if (!bs)
  {
    printf("brush_stroke pointer is NULL\n");
    return;
  }

  printf("Brush stroke: length = %zu, color = (r=%u, g=%u, b=%u, a=%u)\n", bs->length,
         bs->color.red, bs->color.green, bs->color.blue, bs->color.alpha);

  for (size_t i = 0; i < bs->length; i++)
  {
    const return_cubic_t *c = &bs->beziers[i];
    printf("  Bezier[%zu]:\n", i);
    printf("    start = (%f, %f)\n", c->start.x, c->start.y);
    printf("    c1    = (%f, %f)\n", c->c1.x, c->c1.y);
    printf("    c2    = (%f, %f)\n", c->c2.x, c->c2.y);
    printf("    end   = (%f, %f)\n", c->end.x, c->end.y);
  }
}

void test_case1(int output)
{
  knot_t *path;
  knot_t *brush;
  brush_stroke_t *stroke;

  color_t color = {255, 0, 0, 255};
  knots_init();

  brush = brush_make_square();
  brush_tf_rotate(brush, 45.0, Z);
  brush_tf_resize(brush, 3.0, Z);

  path = moveto((point_t) {3.0, 3.0});
  path = lineto(path, (point_t) {3.0, 17.0});

  stroke = draw_shape(path, brush, color);

  if (output) {
    print_brush_stroke(stroke);
  }
  free(stroke->beziers);
  free(stroke);
}

void test_case2(int output)
{
  knot_t *path;
  knot_t *brush;
  brush_stroke_t *stroke;

  color_t color = {125, 0, 0, 0};
  knots_init();

  brush = moveto((point_t) {1.0, 1.0});
  brush = rlineto(brush, (point_t) {-0.5, 0.5});
  brush = rlineto(brush, (point_t) {-0.5, -0.5});
  brush = rlineto(brush, (point_t) {0.0, -1.0});
  brush = rlineto(brush, (point_t) {1.0, 0.0});
  pathclose(brush);

  brush_tf_reflect(brush, X);

  path = moveto((point_t) {0.0, 0.0});
  path = rlineto(path, (point_t) {0.0, 5.0});

  stroke = draw_shape(path, brush, color);

  if (output) {
    print_brush_stroke(stroke);
  }
  free(stroke->beziers);
  free(stroke);
}

void test_case3(int output)
{
  knot_t *path;
  knot_t *brush;
  brush_stroke_t *stroke;

  color_t color = {125, 0, 0, 0};
  knots_init();

  brush = brush_make_square();

  path = moveto((point_t) {0.0, 0.0});
  path = curveto(path, (point_t) {0.0, 5.0}, (point_t) {5.0, 0.0}, (point_t) {5.0, 5.0});

  stroke = draw_shape(path, brush, color);

  if (output) {
    print_brush_stroke(stroke);
  }
  free(stroke->beziers);
  free(stroke);
}

int main(int argc, char **argv)
{
  /* Run with gprof and higher repetition to view performance data */
  unsigned int i;
  for (i = 0; i < 1; i++)
    test_case1(1);
}
