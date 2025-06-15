/*
 * @file common.h
 * @brief Color structure and mixing function declarations.
 *
 * Color Struct provides RGBA color representation 
 * Function for mixing two colors using additive or subtractive blending.
 *
 * Part of Team 4
 *
 * @author André Hempel
 * @date 2025-06-08
 */
#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>  /* for uint8_t */

/*
 * Color represented as RGBA (Red, Green, Blue, Alpha).
 * From team brushes
 */
typedef struct {
    uint8_t r; /*red component (0-255)*/
    uint8_t g; /*green component (0-255)*/
    uint8_t b; /*blue component (0-255)*/
    uint8_t a; /*transparent component (0-255)*/
} Color;

/*
 * Mixes two colors using additive or subtractive blending.
 *
 * @param color1 Pointer to the first color.
 * @param color2 Pointer to the second color.
 * @param mix A float value:
 *        - Range 0.0 to 1.0: for additive mixing (weighted toward color1)
 *        - Range 0.0 to -1.0: for subtractive mixing (weighted toward color2)
 * 
 * @return A newly allocated Color that must be freed by the caller using free().
 */
Color *color_mixer(const Color *color1, const Color *color2, float mix);

#endif // COMMON_H