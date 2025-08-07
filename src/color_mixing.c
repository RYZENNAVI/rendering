/*
 * @file color_mixing.c
 * @authors Team 4 - André, Sun, Jannis
 * @date 06.07.2025
 *
 * Returns pointer to the mixed RGBA result.
 */

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "../inc/common.h"

/*
 * @brief 
 * Mixes two 8-bit color values based on given weight.
 * 
 * @param
 * color_value1, color_value2   color compenent value (0-255)
 * mix                          weight factor for first color (0.0-1.0)
 * 
 * @pre
 * mix in range[0.0, 1.0], color values in range[0,255].
 * 
 * @post
 * Returns blended and rounded color component.
 */

uint8_t mix_color_values(const uint8_t color_value1,
                         const uint8_t color_value2, const double mix) {
    const double mix_result = color_value1 * mix + color_value2 * (1 - mix);
    const uint8_t rounded_result = round(mix_result);
    return rounded_result;
}

/*
 * @brief 
 * Blends two colors using linear interpolation.
 * 
 * @param
 * color1, color2                   pointers to input colors (RGBA)
 * mix                              weight factor for first color (0.0-1.0)
 * 
 * @pre
 * color1 and color2 is not NULL.
 * mix in range[0.0, 1.0].
 * 
 * @post
 * allocated pointer to new color structure color_t which holds result of blend.
 */

color_t *color_mixer(const color_t *color1, const color_t *color2,
                     const double mix) {

    /* Create a new color to be filled with the mixed values. */
    color_t *result_color = malloc(sizeof(color_t));

    /* Fill the members with the mixed color values. */
    result_color->red = mix_color_values(color1->red, color2->red, mix);
    result_color->green = mix_color_values(color1->green, color2->green, mix);
    result_color->blue = mix_color_values(color1->blue, color2->blue, mix);
    result_color->alpha = mix_color_values(color1->alpha, color2->alpha, mix);

    /* Return resulting mixed color. */
    return result_color;
}
