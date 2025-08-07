/*
@file transforms_test.c
@authors Matthias, Toni
@date 10.07.25
*/

#include "common.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define EPS 1e-6

/* Test brush_tf (generic 3×3 matrix) */
static void test_brush_tf()
{
  knot_t *brush;
  brush = moveto((point_t) {2.0, 3.0});
  double M[3][3] = {{2.0, 0.0, 0.0}, {0.0, 3.0, 0.0}, {0.0, 0.0, 1.0}};
  brush_tf(brush, M);
  assert(fabs(brush->x - 4.0) < EPS);
  assert(fabs(brush->y - 9.0) < EPS);
}

/* Test translation to an absolute point */
static void test_brush_tf_translate()
{
  knot_t *brush;
  brush = moveto((point_t) {1.0, 1.0});
  point_t dest = {.x = 4.0, .y = 5.0};
  brush_tf_translate(brush, dest);
  assert(fabs(brush->x - 4.0) < EPS);
  assert(fabs(brush->y - 5.0) < EPS);
}

/* Test X‐shear: x' = x + shear*y */
static void test_brush_tf_shear_x()
{
  knot_t *brush;
  brush = moveto((point_t) {2.0, 3.0});
  brush_tf_shear(brush, 1.0, X);
  assert(fabs(brush->x - 5.0) < EPS);
  assert(fabs(brush->y - 3.0) < EPS);
}

/* Test Y‐shear: y' = y + shear*x */
static void test_brush_tf_shear_y()
{
  knot_t *brush;
  brush = moveto((point_t) {2.0, 3.0});
  brush_tf_shear(brush, 2.0, Y);
  assert(fabs(brush->x - 2.0) < EPS);
  assert(fabs(brush->y - 7.0) < EPS);
}

/* Test Z‐rotation by 90°: (x,y) → (–y, x) */
static void test_brush_tf_rotate_z_90()
{
  knot_t *brush;
  brush = moveto((point_t) {1.0, 0.0});
  brush_tf_rotate(brush, 90.0, Z);
  assert(fabs(brush->x - 0.0) < EPS);
  assert(fabs(brush->y - 1.0) < EPS);
}

/* Test X‐reflection: y → –y (and order reversal is a no‐op for single knot) */
static void test_brush_tf_reflect_x()
{
  knot_t *brush;
  brush = moveto((point_t) {5.0, -2.0});
  brush_tf_reflect(brush, X);
  assert(fabs(brush->x - 5.0) < EPS);
  assert(fabs(brush->y - 2.0) < EPS);
}

/* Test Y‐reflection: x → –x */
static void test_brush_tf_reflect_y()
{
  knot_t *brush;
  brush = moveto((point_t) {5.0, -2.0});
  brush_tf_reflect(brush, Y);
  assert(fabs(brush->x + 5.0) < EPS);
  assert(fabs(brush->y + 2.0) < EPS);
}

/* Test uniform resize (Z axis): (x,y) → (scale·x, scale·y) */
static void test_brush_tf_resize_z()
{
  knot_t *brush;
  brush = moveto((point_t) {1.5, -2.0});
  brush_tf_resize(brush, 2.0, Z);
  assert(fabs(brush->x - 3.0) < EPS);
  assert(fabs(brush->y + 4.0) < EPS);
}

/* Test X‐only resize: (x,y) → (scale·x, y) */
static void test_brush_tf_resize_x()
{
  knot_t *brush;
  brush = moveto((point_t) {2.0, 3.0});
  brush_tf_resize(brush, 3.0, X);
  assert(fabs(brush->x - 6.0) < EPS);
  assert(fabs(brush->y - 3.0) < EPS);
}

/* Test Y‐only resize: (x,y) → (x, scale·y) */
static void test_brush_tf_resize_y()
{
  knot_t *brush;
  brush = moveto((point_t) {2.0, 3.0});
  brush_tf_resize(brush, 4.0, Y);
  assert(fabs(brush->x - 2.0) < EPS);
  assert(fabs(brush->y - 12.0) < EPS);
}

int main(void)
{
  knots_init();

  test_brush_tf();
  test_brush_tf_translate();
  test_brush_tf_shear_x();
  test_brush_tf_shear_y();
  test_brush_tf_rotate_z_90();
  test_brush_tf_reflect_x();
  test_brush_tf_reflect_y();
  test_brush_tf_resize_z();
  test_brush_tf_resize_x();
  test_brush_tf_resize_y();

  printf("All transform tests passed\n");
  return 0;
}