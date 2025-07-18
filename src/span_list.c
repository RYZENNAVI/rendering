/*
 * @file span_list.c
 * @authors Team 2 - Span List Implementation
 * @date 01.07.25
 *
 * Implementation of span list management functions
 */

#include "../../inc/common.h"
#include <stdlib.h>
#include <string.h>

/*
 * Create a new span list with initial capacity
 */
span_list_t *span_list_create(size_t initial_capacity) {
    span_list_t *list = malloc(sizeof(span_list_t));
    if (!list) {
        return NULL;
    }

    list->spans = malloc(initial_capacity * sizeof(span_t));
    if (!list->spans) {
        free(list);
        return NULL;
    }

    list->count = 0;
    list->capacity = initial_capacity;
    return list;
}

/*
 * Add a span to the list, expanding capacity if needed
 */
void span_list_add(span_list_t *list, span_t span) {
    if (!list) return;

    // Expand capacity if needed
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        span_t *new_spans =
            realloc(list->spans, new_capacity * sizeof(span_t));
        if (!new_spans) {
            return; // Failed to expand, skip this span
        }
        list->spans = new_spans;
        list->capacity = new_capacity;
    }

    // Add the span
    list->spans[list->count] = span;
    list->count++;
}

/*
 * Free a span list and its memory
 */
void span_list_free(span_list_t *list) {
    if (!list) return;

    if (list->spans) {
        free(list->spans);
    }
    free(list);
}

/*
 * Create a new brush stroke list with initial capacity
 */
brush_stroke_list_t *brush_stroke_list_create(size_t initial_capacity) {
    brush_stroke_list_t *list = malloc(sizeof(brush_stroke_list_t));
    if (!list) {
        return NULL;
    }

    list->strokes = malloc(initial_capacity * sizeof(brush_stroke_t *));
    if (!list->strokes) {
        free(list);
        return NULL;
    }

    list->count = 0;
    list->capacity = initial_capacity;
    return list;
}

/*
 * Add a brush stroke to the list, expanding capacity if needed
 */
void brush_stroke_list_add(brush_stroke_list_t *list,
                           brush_stroke_t *stroke) {
    if (!list || !stroke) return;

    // Expand capacity if needed
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        brush_stroke_t **new_strokes =
            realloc(list->strokes, new_capacity * sizeof(brush_stroke_t *));
        if (!new_strokes) {
            return; // Failed to expand, skip this stroke
        }
        list->strokes = new_strokes;
        list->capacity = new_capacity;
    }

    // Add the stroke
    list->strokes[list->count] = stroke;
    list->count++;
}

/*
 * Free a brush stroke list (not the individual strokes)
 */
void brush_stroke_list_free(brush_stroke_list_t *list) {
    if (!list) return;

    if (list->strokes) {
        free(list->strokes);
    }
    free(list);
}

/*
 * Free a brush stroke and its memory
 */
void brush_stroke_free(brush_stroke_t *stroke) {
    if (!stroke) return;

    if (stroke->beziers) {
        free(stroke->beziers);
    }
    free(stroke);
}
