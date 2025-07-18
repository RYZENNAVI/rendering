<!-- Authors: Jannis -->

# Team 4 Function Documentation
## Overview
The responsibilities of team 4 have shifted multiple times over the project, meaning that original outlines are not valid anymore. At this point, the coding responsibility lies with a function that mixes two given colors and returns the resulting color.

_Note: While `uint8\_t` itself restricts values to 8 bits, legal value ranges are still given explicitely for clarity and completion._

## Color representation
Colors are defined as a struct `color_t` with four members according to the RGBA color values for `red`, `green`, `blue` and `alpha` (transparency). The representation uses 8 bits with the value range accordingly being 0 to 255 for each member.

## Functions
### color_mixer()
This is the main function. It receives two colors as well as a mixing proportion for the first color in relation to total coverage.

All parameters remain unchanged and are thus `constant`.

**Signature:**

```c
color_t *color_mixer(const color_t *color1, const color_t *color2, const double mix)
```

**Parameters:**

- `*color1` (`color_t`) - Pointer to the first color to be mixed. The given mix refers to the proportion of this color in the result.
- `*color2` (`color_t`) - Pointer to the second color to be mixed. One minus the given mix gives the proportion of this color in the result.
- `mix` (`double`) - The mixture proportion of the first color in the result. As an example, `0.7` would mean that the resulting color is made up by 70% of the first given color and by 30% of the second given color.
    - Value range: `0.0` to `1.0` referring to 0% to 100% respectively.

**Preconditions:**

- `color1` and `color2` are not `NULL`.
- The values for each individual member of either color are between `0` and `255` (inclusive).
- `mix` is a value between `0.0` and `1.0` (inclusive).

**Invariants:**

- `color1` and `color2` are not changed.

**Postconditions:**

- The pointer returned by the function is valid for struct `color_t`.
- The values for each individual member of the color at the pointer are between `0` and `255` (inclusive).

### mix_color_values()
_This is an implementation internal function and not exposed to the end user._

This function mixes two individual color values by a given proportion. It is applied to calculate the member values of the resulting color of `color_mixer()`.

**Signature:**

```c
uint8_t mix_color_values(const uint8_t color_value1, const uint8_t color_value1, const double mix)
```

**Parameters:**

- `color_value1` (`uint8_t`) - The first color value to be mixed. The given mix refers to the proportion of this value in the result.
    - Value range: `0` to `255`.
- `color_value2` (`uint8_t`) - The second color value to be mixed. One minus the given mix gives the proportion of this value in the result.
    - Value range: `0` to `255`.
- `mix` (`double`) - The mixture proportion of the first color value in the result. As an example, `0.7` would mean that the resulting value is made up by 70% of the first given value and by 30% of the second given value.
    - Value range: `0.0` to `1.0` referring to 0% to 100% respectively.

**Preconditions:**

- The values for `color_value1` and `color_value2` are each between `0` and `255` (inclusive).
- `mix` is a value between `0.0` and `1.0` (inclusive).

**Invariants:**

- None.

**Postconditions:**

- The returned result is a valid `uint8_t`.
- The returned value is between `0` and `255`.
