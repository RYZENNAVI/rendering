# Rendering Library

A comprehensive C library for 2D vector graphics rendering with Bézier curves, brush strokes, and PNG output capabilities. This project implements a complete rendering pipeline from path construction to rasterization and image export.

## Features

- **Vector Graphics**: Create complex shapes using Bézier curves and path operations
- **Brush System**: Configurable brush strokes with transformations (resize, rotate, translate, reflect, shear)
- **Color Mixing**: Advanced color blending and mixing capabilities
- **Rasterization**: High-quality scanline rendering with anti-aliasing support
- **Image Export**: PNG output with libpng integration
- **Mathematical Functions**: Comprehensive 2D geometry and vector operations
- **Memory Management**: Efficient knot and object pooling system

## Project Structure

```
rendering/
├── src/                    # Source files
│   ├── bezier.c           # Bézier curve implementation
│   ├── brush.c            # Brush stroke system
│   ├── color_mixing.c     # Color blending operations
│   ├── draw.c             # Path construction (moveto, lineto, etc.)
│   ├── image.c            # Image buffer management
│   ├── image_png.c        # PNG export functionality
│   ├── knots.c            # Memory management for knots
│   ├── list.c             # Doubly-linked list implementation
│   ├── math_functions.c   # 2D geometry operations
│   ├── rasterization.c    # Scanline rasterization
│   ├── scanline_fill.c    # Polygon filling
│   ├── span_list.c        # Span-based rendering
│   └── transforms.c       # Geometric transformations
├── inc/                   # Header files
│   └── common.h           # Main API header
├── examples/              # Demo applications
│   ├── user_examples.c    # Comprehensive demo program
│   └── Makefile          # Build script for examples
├── test/                  # Test files
├── doc/                   # Documentation
└── Makefile              # Main build script
```

## Prerequisites

- **Compiler**: GCC or Clang with C99 support
- **Libraries**: 
  - libpng (for PNG export)
  - libm (math library)
- **Build Tools**: Make

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential libpng-dev
```

**macOS:**
```bash
brew install libpng
```

**Windows (MSYS2):**
```bash
pacman -S mingw-w64-x86_64-libpng mingw-w64-x86_64-gcc
```

## Building the Project

### Main Library
```bash
# Build the main curves executable
make

# Run basic test
make test

# Clean build artifacts
make clean
```

### Examples
```bash
# Navigate to examples directory
cd examples

# Build the demo program
make

# Run all demo scenarios
./user_examples all

# Run specific scenario
./user_examples sharp

# Clean examples
make clean
```

## Usage

### Basic API Usage

```c
#include "inc/common.h"

int main() {
    // Initialize the rendering system
    knots_init();
    image_t *img = create_image(800, 600);
    
    // Create a simple path
    knot_t *start = moveto((point_t){100, 100});
    knot_t *end = lineto(start, (point_t){200, 200});
    pathclose(end);
    
    // Create a brush and draw
    brush_t *brush = brush_make_square(10.0);
    brush_stroke_t *stroke = brush_stroke_create(start, brush, 
                                               (color_t){255, 0, 0, 255});
    
    // Render to image
    draw_shape(img, stroke);
    
    // Save as PNG
    save_png(img, "output.png");
    
    // Cleanup
    brush_stroke_free(stroke);
    free_image(img);
    return 0;
}
```

### Demo Scenarios

The `examples/user_examples` program demonstrates 12 different rendering scenarios:

| Scenario | Command | Description |
|----------|---------|-------------|
| sharp | `./user_examples sharp` | Polyline with 90° sharp corner |
| color | `./user_examples color` | Same geometry with different colors |
| letter | `./user_examples letter` | Blue letter L shape |
| relative | `./user_examples relative` | Triangle using relative coordinates |
| quadratic | `./user_examples quadratic` | Quadratic Bézier curve |
| transform | `./user_examples transform` | Brush transformations demo |
| misc | `./user_examples misc` | Miscellaneous API features |
| letters | `./user_examples letters` | Word "TEAM" rendering |
| boolean | `./user_examples boolean` | Boolean operations on shapes |
| spans | `./user_examples spans` | Manual span list rendering |
| abcd | `./user_examples abcd` | Colored letters A B C D |
| xspan | `./user_examples xspan` | Overlapping shapes with blending |

### Key API Functions

**Path Construction:**
- `moveto(point_t)` - Start a new path
- `lineto(knot_t*, point_t)` - Add line segment
- `curveto(knot_t*, cubic_curve_t)` - Add cubic Bézier curve
- `pathclose(knot_t*)` - Close current path

**Brush Operations:**
- `brush_make_square(double)` - Create square brush
- `brush_tf_resize(brush_t*, double)` - Resize brush
- `brush_tf_rotate(brush_t*, double)` - Rotate brush
- `brush_tf_translate(brush_t*, point_t)` - Translate brush

**Rendering:**
- `draw_shape(image_t*, brush_stroke_t*)` - Render stroke to image
- `rasterize(brush_stroke_t*, int)` - Convert to spans
- `save_png(image_t*, const char*)` - Export PNG

## Testing

### Unit Tests
```bash
# Run math function tests
cd src
gcc -I../inc example_math_function.c math_functions.c -lm -o test_math
./test_math
```

### Integration Tests
```bash
# Test PNG output
gcc -I inc test_shapes_to_png.c src/image.c src/image_png.c -lpng -o test_png
./test_png
```

## Troubleshooting

### macOS PNG Issues
If you encounter `png.h not found` errors on macOS:

```bash
# Install libpng
brew install libpng

# Build with explicit paths
make CFLAGS="-I/opt/homebrew/opt/libpng/include" \
     LDFLAGS="-L/opt/homebrew/opt/libpng/lib -lpng"
```

### Memory Issues
- Ensure `knots_init()` is called before any path operations
- Always call cleanup functions (`free_image`, `brush_stroke_free`)
- Use valgrind for memory leak detection: `valgrind ./user_examples all`

## Documentation

Detailed documentation is available in the `doc/` directory:
- `Brush_Overview.md` - Brush system documentation
- `rasterization_overview.md` - Rasterization pipeline
- `color_mixing.md` - Color blending algorithms
- `curves_design_rationale.md` - Bézier curve implementation

## Contributing

1. Follow C99 standard
2. Use consistent indentation (4 spaces)
3. Add unit tests for new functions
4. Update documentation for API changes
5. Test on multiple platforms before submitting

## License

This project is developed as part of an academic course. See individual source files for specific license information.

## Authors

- Team 1: Rendering Pipeline
- Team 2: Rasterization
- Team 3: Image Output
- Team 4: API Integration

For questions or issues, please refer to the documentation in the `doc/` directory or contact the development teams.
