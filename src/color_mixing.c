/*
 * @file color_mixing.c
 * @authors Team 4 - André, Sun, Jannis
 * @date 06.07.2025
 *
 * Implementation of color_mixer() for value ranges 0-255 RGB.
 */

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

// Mixes the individual color values and returns the resulting single value.
uint8_t mix_color_values(const uint8_t color_value1,
                         const uint8_t color_value2, const double mix) {
    const double mix_result = color_value1 * mix + color_value2 * (1 - mix);
    const uint8_t rounded_result = round(mix_result);
    return rounded_result;
}

// Mixes two colors and returns a pointer to the result.
color_t *color_mixer(const color_t *color1, const color_t *color2,
                     const double mix) {
    // Create a new color to be filled with the mixed values.
    color_t *result_color = malloc(sizeof(color_t));
    // Fill the members with the mixed color values.
    result_color->red = mix_color_values(color1->red, color2->red, mix);
    result_color->green = mix_color_values(color1->green, color2->green, mix);
    result_color->blue = mix_color_values(color1->blue, color2->blue, mix);
    result_color->alpha = mix_color_values(color1->alpha, color2->alpha, mix);

    // Return resulting mixed color.
    return result_color;
}
