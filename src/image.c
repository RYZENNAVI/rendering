/*
 * @file image.c
 * @authors Team 2 - Image Output Functions
 * @date 01.07.25
 *
 * Implementation of image creation and BMP output functions
 */

#include "../../inc/common.h"
#include <string.h>

/*
 * Create a new image with specified dimensions
 */
image_t *create_image(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    image_t *img = malloc(sizeof(image_t));
    if (!img) {
        return NULL;
    }

    img->width = width;
    img->height = height;

    // Allocate data (RGBA format, 4 bytes per pixel)
    img->data = malloc(width * height * 4);
    if (!img->data) {
        free(img);
        return NULL;
    }

    // Initialize to transparent black
    memset(img->data, 0, width * height * 4);

    return img;
}

/*
 * Set a pixel color in the image
 */
void set_pixel(image_t *img, int x, int y, color_t color) {
    if (!img || !img->data || x < 0 || y < 0 || x >= img->width ||
        y >= img->height) {
        return;
    }

    int index = (y * img->width + x) * 4;
    img->data[index] = color.red;
    img->data[index + 1] = color.green;
    img->data[index + 2] = color.blue;
    img->data[index + 3] = color.alpha;
}

/*
 * Get a pixel color from the image
 */
color_t get_pixel(image_t *img, int x, int y) {
    color_t black = {0, 0, 0, 0};

    if (!img || !img->data || x < 0 || y < 0 || x >= img->width ||
        y >= img->height) {
        return black;
    }

    int index = (y * img->width + x) * 4;
    color_t color = {img->data[index], img->data[index + 1],
                     img->data[index + 2], img->data[index + 3]};

    return color;
}

/*
 * Render spans to an image
 */
void render_spans_to_image(span_list_t *spans, image_t *img) {
    if (!spans || !img) {
        return;
    }

    for (size_t i = 0; i < spans->count; i++) {
        span_t *span = &spans->spans[i];

        for (int x = span->x_start; x <= span->x_end; x++) {
            set_pixel(img, x, span->y, span->color);
        }
    }
}

/*
 * Save image as BMP file
 */
void save_bmp(image_t *img, const char *filename) {
    if (!img || !img->data || !filename) {
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    int width = img->width;
    int height = img->height;
    int row_size = ((width * 3 + 3) / 4) * 4; // 4-byte aligned
    int image_size = row_size * height;
    int file_size = 54 + image_size; // 54 = BMP header size

    // BMP Header (14 bytes)
    uint8_t bmp_header[14] = {
        'B',
        'M', // Signature
        file_size & 0xFF,
        (file_size >> 8) & 0xFF,
        (file_size >> 16) & 0xFF,
        (file_size >> 24) & 0xFF,
        0,
        0,
        0,
        0, // Reserved
        54,
        0,
        0,
        0 // Data offset
    };
    fwrite(bmp_header, 1, 14, file);

    // DIB Header (40 bytes)
    uint8_t dib_header[40] = {
        40,
        0,
        0,
        0, // Header size
        width & 0xFF,
        (width >> 8) & 0xFF,
        (width >> 16) & 0xFF,
        (width >> 24) & 0xFF,
        height & 0xFF,
        (height >> 8) & 0xFF,
        (height >> 16) & 0xFF,
        (height >> 24) & 0xFF,
        1,
        0, // Planes
        24,
        0, // Bits per pixel
        0,
        0,
        0,
        0, // Compression
        image_size & 0xFF,
        (image_size >> 8) & 0xFF,
        (image_size >> 16) & 0xFF,
        (image_size >> 24) & 0xFF,
        0,
        0,
        0,
        0, // X pixels per meter
        0,
        0,
        0,
        0, // Y pixels per meter
        0,
        0,
        0,
        0, // Colors used
        0,
        0,
        0,
        0 // Important colors
    };
    fwrite(dib_header, 1, 40, file);

    // Write pixel data (bottom-up, BGR format)
    uint8_t *row_data = malloc(row_size);
    if (row_data) {
        for (int y = height - 1; y >= 0; y--) {
            memset(row_data, 0, row_size);

            for (int x = 0; x < width; x++) {
                int src_index = (y * width + x) * 4;
                int dst_index = x * 3;

                // Convert RGBA to BGR
                row_data[dst_index] = img->data[src_index + 2];     // Blue
                row_data[dst_index + 1] = img->data[src_index + 1]; // Green
                row_data[dst_index + 2] = img->data[src_index];     // Red
            }

            fwrite(row_data, 1, row_size, file);
        }

        free(row_data);
    }

    fclose(file);
    printf("BMP saved: %s (%dx%d)\n", filename, width, height);
}

/*
 * Save image as PPM file
 */
void save_ppm(image_t *img, const char *filename) {
    if (!img || !img->data || !filename) {
        return;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // PPM header
    fprintf(file, "P3\n");
    fprintf(file, "%d %d\n", img->width, img->height);
    fprintf(file, "255\n");

    // Write pixel data
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            color_t pixel = get_pixel(img, x, y);
            fprintf(file, "%d %d %d ", pixel.red, pixel.green, pixel.blue);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("PPM saved: %s (%dx%d)\n", filename, img->width, img->height);
}

/*
 * Free image memory
 */
void free_image(image_t *img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
}
