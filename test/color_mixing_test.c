/*
 * @file color_mixing_test.c
 * @authors Team 4 - Jannis
 * @date 08.07.2025
 *
 * Testing file for functionality of function color_mixer().
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../inc/common.h"

color_t *make_color(int r, int g, int b, int a) {
    color_t *result = malloc (sizeof(color_t));
    result->red = r;
    result->green = g;
    result->blue = b;
    result->alpha = a;
    
    return result;
}

int test_color_combination(const color_t *color1, const color_t *color2,
        const double mix, color_t *result_color, uint8_t e_r, uint8_t e_g,
        uint8_t e_b, uint8_t e_a) {
    int failed = 0;
    uint8_t r1 = color1->red;
    uint8_t r2 = color2->red;
    uint8_t g1 = color1->green;
    uint8_t g2 = color2->green;
    uint8_t b1 = color1->blue;
    uint8_t b2 = color2->blue;
    uint8_t a1 = color1->alpha;
    uint8_t a2 = color2->alpha;

    *result_color = *color_mixer(color1, color2, mix);
    uint8_t rr = result_color->red;
    uint8_t rg = result_color->green;
    uint8_t rb = result_color->blue;
    uint8_t ra = result_color->alpha;

    printf("Testing (%d,%d,%d,%d) + (%d,%d,%d,%d) with mix %.*f. "
           "Result should be (%d,%d,%d,%d).\n\tActual result: "
           "(%d,%d,%d,%d)\n", r1, g1, b1, a1, r2, g2, b2, a2, 
           2, mix, e_r, e_g, e_b, e_a, rr, rg, rb, ra);

    if (rr == e_r && rg == e_g && rb == e_b && ra == e_a) {
        printf("Test passed.\n\n");
    } else {
        printf("TEST FAILED!\n\n");
        failed = 1;
    }
    
    return failed;
}

int main() {
    color_t *red = make_color(255, 0, 0, 255);
    color_t *darkred = make_color(127, 0, 0, 255);
    color_t *green = make_color(0, 255, 0, 255);
    color_t *darkgreen = make_color(0, 127, 0, 255);
    color_t *blue = make_color(0, 0, 255, 255);
    color_t *darkblue = make_color(0, 0, 127, 255);
    color_t *yellow = make_color(255, 255, 0, 255);
    color_t *darkyellow = make_color(127, 127, 0, 255);
    color_t *teal= make_color(0, 255, 255, 255);
    color_t *darkteal = make_color(0, 127, 127, 255);
    color_t *purple = make_color(255, 0, 255, 255);
    color_t *darkpurple = make_color(127, 0, 127, 255);
    color_t *white = make_color(255, 255, 255, 255);
    color_t *grey = make_color(127, 127, 127, 255);
    color_t *black = make_color(0, 0, 0, 255);

    int fails = 0;
    color_t *mixing_color = malloc (sizeof(color_t));

    printf("TESTING SUITE FOR COLOR MIXING\n\n");
    printf("The following colors are defined:\n");
    printf("Red (255,0,0,255), dark red (127,0,0,255), green (0,255,0,255),"
           " dark green (0,127,0,255), blue (0,0,255,255), dark blue (0,0,127,"
           "255), yellow (255,255,0,255), dark yellow (127,127,0,255), teal ("
           "0,255,255,255), dark teal (0,127,127,255), purple (255,0,255,255),"
           " dark purple (127,0,127,255), white (255,255,255,255), grey (127,"
           "127,127,255), black (0,0,0,255).\n\n");

    // Simple tests of basic colors.
    fails += test_color_combination(red, green, 0.5, mixing_color, 
                                    128, 128, 0, 255);
    fails += test_color_combination(red, blue, 0.5, mixing_color, 
                                    128, 0, 128, 255);
    fails += test_color_combination(green, blue, 0.5, mixing_color, 
                                    0, 128, 128, 255);
    fails += test_color_combination(white, black, 0.5, mixing_color, 
                                    128, 128, 128, 255);

    // Tests involving colors at values other than 0 or 255.
    fails += test_color_combination(red, darkgreen, 0.5, mixing_color, 
                                    128, 64, 0, 255);
    fails += test_color_combination(grey, blue, 0.5, mixing_color, 
                                    64, 64, 191, 255);
    fails += test_color_combination(darkpurple, white, 0.5, mixing_color, 
                                    191, 128, 191, 255);
    fails += test_color_combination(green, darkyellow, 0.5, mixing_color, 
                                    64, 191, 0, 255);

    // Tests with mix proportions other than 0.5.
    fails += test_color_combination(white, black, 0.9, mixing_color, 
                                    230, 230, 230, 255);
    fails += test_color_combination(black, white, 0.9, mixing_color, 
                                    25, 25, 25, 255);
    fails += test_color_combination(red, blue, 0.75, mixing_color, 
                                    191, 0, 64, 255);
    fails += test_color_combination(green, darkpurple, 0.2, mixing_color, 
                                    102, 51, 102, 255);
    fails += test_color_combination(yellow, red, 0.66, mixing_color, 
                                    255, 168, 0, 255);
    fails += test_color_combination(black, teal, 0.1, mixing_color, 
                                    0, 230, 230, 255);
    fails += test_color_combination(green, darkgreen, 0.35, mixing_color, 
                                    0, 172, 0, 255);
    fails += test_color_combination(teal, darkyellow, 0.25, mixing_color, 
                                    95, 159, 64, 255);

    printf("End of testing. Number of failed tests:\n\t%d\n\nExiting.\n", fails);

    free(red);
    free(darkred);
    free(green);
    free(darkgreen);
    free(blue);
    free(darkblue);
    free(yellow);
    free(darkyellow);
    free(teal);
    free(darkteal);
    free(purple);
    free(darkpurple);
    free(black);
    free(grey);
    free(white);
    free(mixing_color);

    if (fails > 0) {
        return 0;
    } else {
        return 1;
    }
}
