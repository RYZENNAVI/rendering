#include "common.h"
#include "math_functions.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

void display_point(knot_t p) {
    printf("x:%.10f, y: %.10f\n", p.x, p.y);
}

void same_origin_and_destination_vector_has_length_of_zero() {
    knot_t O;
    O.x = 0;
    O.y = 0;
    assert(0 == euclidean_distance(O, O));
    printf("same_origin_and_destination_vector_has_length_of_zero "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void horizontal_vec_from_origin_has_destination_x_as_length() {
    knot_t O;
    O.x = 0;
    O.y = 0;

    knot_t A;
    A.x = 2;
    A.y = 0;
    assert(A.x == euclidean_distance(O, A));
    printf("horizontal_vec_from_origin_has_destination_x_as_length "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void vertical_vec_from_origin_has_destination_y_as_length() {
    knot_t O;
    O.x = 0;
    O.y = 0;

    knot_t A;
    A.x = 0;
    A.y = 2;

    assert(A.y == euclidean_distance(O, A));
    printf("vertical_vec_from_origin_has_destination_y_as_length "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void euclidean_distance_tests() {
    same_origin_and_destination_vector_has_length_of_zero();
    horizontal_vec_from_origin_has_destination_x_as_length();
    vertical_vec_from_origin_has_destination_y_as_length();
}

void adding_two_points_with_opposite_coordinates_gives_the_origin() {
    knot_t p1, p2, O;

    p1.x = 2;
    p1.y = 3;

    p2.x = -2;
    p2.y = -3;

    O.x = 0;
    O.y = 0;

    knot_t result = add_knot(p1, p2);

    assert(are_points_the_same(result, O));
    printf("adding_two_points_with_opposite_coordinates_gives_the_origin "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void adding_two_points_with_the_origin_as_coordinates_gives_the_origin() {
    knot_t p1, p2, O;

    p1.x = 0;
    p1.y = 0;

    p2.x = 0;
    p2.y = 0;

    O.x = 0;
    O.y = 0;

    knot_t result = add_knot(p1, p2);
    assert(are_points_the_same(result, O));

    printf("adding_two_points_with_the_origin_as_coordinates_gives_the_"
           "origin \e[32mPASSED!\e[32m\e[0m\n");
}

void adding_two_points_with_opposite_coordinates_with_very_small_values() {
    knot_t p1, p2, O;
    p1.x = 1e-9;
    p1.y = -1e-9;

    p2.x = -1e-9;
    p2.y = 1e-9;

    O.x = 0;
    O.y = 0;

    knot_t result = add_knot(p1, p2);
    assert(are_points_the_same(result, O));

    printf("adding_two_points_with_opposite_coordinates_with_very_small_"
           "values \e[32mPASSED!\e[32m\e[0m\n");
}

void add_knot_tests() {
    adding_two_points_with_opposite_coordinates_gives_the_origin();
    adding_two_points_with_the_origin_as_coordinates_gives_the_origin();
    adding_two_points_with_opposite_coordinates_with_very_small_values();
}

void sub_two_points_with_same_coordinates_gives_the_origin() {
    knot_t p1, p2, O;

    p1.x = -1;
    p1.y = -2;

    p2.x = -1;
    p2.y = -2;

    O.x = 0;
    O.y = 0;

    knot_t result = sub_knot(p1, p2);
    assert(are_points_the_same(result, O));

    printf("sub_two_points_with_same_coordinates_gives_the_origin "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void sub_two_points_with_coordinates_0_0_gives_the_origin() {
    knot_t p1, p2, O;

    p1.x = 0;
    p1.y = 0;

    p2.x = 0;
    p2.y = 0;

    O.x = 0;
    O.y = 0;

    knot_t result = sub_knot(p1, p2);
    assert(are_points_the_same(result, O));

    printf("sub_two_points_with_coordinates_0_0_gives_the_origin "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void sub_two_points_with_opposite_coordinates_with_very_small_values() {
    knot_t p1, p2, O;
    p1.x = 1e-9;
    p1.y = -1e-9;

    p2.x = 1e-9;
    p2.y = -1e-9;

    O.x = 0;
    O.y = 0;

    knot_t result = sub_knot(p1, p2);
    assert(are_points_the_same(result, O));
    printf("sub_two_points_with_opposite_coordinates_with_very_small_values "
           "\e[32mPASSED!\e[32m\e[0m\n");
}

void sub_knot_tests() {
    sub_two_points_with_same_coordinates_gives_the_origin();
    sub_two_points_with_coordinates_0_0_gives_the_origin();
    sub_two_points_with_opposite_coordinates_with_very_small_values();
}

void scalar_mul_with_mu_gives_0_0() {
    knot_t p1, O;

    double mu = 0;

    p1.x = 4;
    p1.y = 2;

    O.x = 0;
    O.y = 0;

    knot_t result = scale(p1, mu);
    assert(are_points_the_same(result, O));
    printf("scalar_mul_with_mu_gives_0_0 \e[32mPASSED!\e[32m\e[0m\n");
}

void scalar_mul_with_random_mu() {
    knot_t p1, A;

    double mu = 2;

    p1.x = -4;
    p1.y = 2;

    A.x = -8;
    A.y = 4;

    knot_t result = scale(p1, mu);
    assert(are_points_the_same(result, A));
    printf("scalar_mul_with_random_mu \e[32mPASSED!\e[32m\e[0m\n");
}

void scalar_tests() {
    scalar_mul_with_mu_gives_0_0();
    scalar_mul_with_random_mu();
}

void same_direction_angle() {
    knot_t p1, p2;
    p1.x = 1;
    p1.y = 0;

    p2.x = 2;
    p2.y = 0;

    double e = 1e-9;
    double expected_result = 0;
    double actual_result = angle_between(p1, p2);

    assert(expected_result - actual_result < e);
    printf("same_direction_angle \e[32mPASSED!\e[32m\e[0m\n");
}

void angle_between_with_very_small_values() {
    knot_t p1, p2;
    p1.x = 1e-9;
    p1.y = 0;

    p2.x = 0;
    p2.y = 1e-9;

    double expected_result = M_PI / 2;
    double actual_result = angle_between(p1, p2);

    assert(expected_result == actual_result);
    printf("angle_between_with_very_small_values \e[32mPASSED!\e[32m\e[0m\n");
}

void angle_between_for_oppositee_direction() {
    knot_t p1, p2;

    p1.x = 1;
    p1.y = 0;

    p2.x = -1;
    p2.y = 0;

    double expected_result = M_PI;
    double actual_result = angle_between(p1, p2);

    assert(expected_result == actual_result);
    printf(
        "angle_between_for_oppositee_direction \e[32mPASSED!\e[32m\e[0m\n");
}

void angle_between_slightly_exceeding_1() {
    knot_t p1, p2;

    p1.x = 1;
    p1.y = 0;

    p2.x = 1 + 1e-10;
    p2.y = 0;

    double expected_result = 0.0;
    double actual_result = angle_between(p1, p2);

    double epsilon = 1e-9;
    assert((expected_result - actual_result) < epsilon);

    printf("angle_between_slightly_exceeding_1 \e[32mPASSED!\e[32m\e[0m\n");
}

void angle_between_tests() {
    same_direction_angle();
    angle_between_with_very_small_values();
    angle_between_for_oppositee_direction();
    angle_between_slightly_exceeding_1();
}

void norm_of_0_vector_is_0() {
    knot_t p1, p2;

    p1.x = 0;
    p1.y = 0;

    p2.x = 0;
    p2.y = 0;

    double expected_result = 0;
    double actual_result = norm(p1, p2);

    assert(expected_result == actual_result);
    printf("norm_of_0_vector_is_0 \e[32mPASSED!\e[32m\e[0m\n");
}

void norm_of_same_coordinates_is_0() {
    knot_t p1, p2;

    p1.x = -1;
    p1.y = 2;

    p2.x = -1;
    p2.y = 2;

    double expected_result = 0;
    double actual_result = norm(p1, p2);

    assert(expected_result == actual_result);
    printf("norm_of_same_coordinates_is_0 \e[32mPASSED!\e[32m\e[0m\n");
}

void norm_for_two_random_points() {
    knot_t p1, p2;

    p1.x = 3;
    p1.y = 3;

    p2.x = 6;
    p2.y = 7;

    double expected_result = 5;
    double actual_result = norm(p1, p2);
    assert(expected_result == actual_result);
    printf("norm_for_two_random_points \e[32mPASSED!\e[32m\e[0m\n");
}

void norm_function_tests() {
    norm_of_0_vector_is_0();
    norm_of_same_coordinates_is_0();
    norm_for_two_random_points();
}

void rotate_vectors_for_0_length_vectors() {
    knot_t p1, p2;

    p1.x = 0;
    p1.y = 0;

    p2.x = 0;
    p2.y = 0;

    double theta = M_PI / 4;

    knot_t expected_result;

    expected_result.x = 0;
    expected_result.y = 0;

    knot_t actual_result = rotate_vectors(p1, p2, theta);
    assert(are_points_the_same(expected_result, actual_result));
    printf("rotate_vectors_for_0_length_vectors \e[32mPASSED!\e[32m\e[0m\n");
}

void rotate_vectors_by_90_degrees() {
    knot_t p1, p2;

    p1.x = 0;
    p1.y = 0;

    p2.x = 1;
    p2.y = 0;

    double theta = M_PI / 2;

    knot_t expected_result;
    expected_result.x = 0;
    expected_result.y = 1;

    double epsilon = 1e-9;

    knot_t actual_result = rotate_vectors(p1, p2, theta);

    assert(fabs(expected_result.x - actual_result.x) < epsilon);
    assert(fabs(expected_result.y - actual_result.y) < epsilon);
    printf("rotate_vectors_by_90_degrees \e[32mPASSED!\e[32m\e[0m\n");
}

void rotate_vectors_by_full_circle() {
    knot_t p1, p2;

    p1.x = 0;
    p1.y = 0;

    p2.x = 1;
    p2.y = 0;

    double theta = 2 * M_PI;

    knot_t expected_result;

    expected_result.x = 1;
    expected_result.y = 0;

    double epsilon = 1e-9;

    knot_t actual_result = rotate_vectors(p1, p2, theta);

    assert(fabs(expected_result.x - actual_result.x) < epsilon);
    assert(fabs(expected_result.y - actual_result.y) < epsilon);
    printf("rotate_vectors_by_full_circle \e[32mPASSED!\e[32m\e[0m\n");
}

void rotate_vector_around_a_non_origin_point() {
    knot_t p1, p2;

    p1.x = 1;
    p1.y = 1;

    p2.x = 3;
    p2.y = 1;

    double theta = M_PI / 4;
    double epsilon = 1e-9;

    knot_t expected_result;

    expected_result.x = 1 + (2 * cos(M_PI / 4));
    expected_result.y = 1 + (2 * sin(M_PI / 4));

    knot_t actual_result = rotate_vectors(p1, p2, theta);

    assert(fabs(expected_result.x - actual_result.x) < epsilon);
    assert(fabs(expected_result.y - actual_result.y) < epsilon);

    printf(
        "rotate_vector_arond_a_non_origin_point \e[32mPASSED!\e[32m\e[0m\n");
}

void rotate_vector_with_negative_values() {
    knot_t p1, p2;

    p1.x = 0;
    p1.y = 0;

    p2.x = -1;
    p2.y = 0;

    double theta = M_PI / 2;
    double epsilon = 1e-9;

    knot_t expected_result;
    expected_result.x = 0;
    expected_result.y = -1;

    knot_t actual_result = rotate_vectors(p1, p2, theta);

    assert(fabs(expected_result.x - actual_result.x) < epsilon);
    assert(fabs(expected_result.y - actual_result.y) < epsilon);

    printf("rotate_vector_with_negative_values \e[32mPASSED!\e[32m\e[0m\n");
}

void rotate_vector_tests() {
    rotate_vectors_for_0_length_vectors();
    rotate_vectors_by_90_degrees();
    rotate_vectors_by_full_circle();
    rotate_vector_around_a_non_origin_point();
    rotate_vector_with_negative_values();
}

int main(void) {
    euclidean_distance_tests();
    add_knot_tests();
    sub_knot_tests();
    scalar_tests();
    angle_between_tests();
    norm_function_tests();
    rotate_vector_tests();
    return 0;
}
